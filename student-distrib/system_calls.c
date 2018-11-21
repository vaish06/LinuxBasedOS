#include "types.h"
#include "paging.h"
#include "lib.h"
#include "idt.h"
#include "rtc.h"
#include "terminal.h"
#include "keyboard.h"
#include "system_calls_wrapper.h"
#include "x86_desc.h"
#include "system_calls.h"
#include "filesystem.h"
#include "schedule.h"

int32_t PCB_init(PCB_t*, uint32_t , char*);
extern int32_t exception_halt;

int32_t rtc_jumptable[4] = { (int32_t)rtc_open,  (int32_t)rtc_read, (int32_t) rtc_write, (int32_t) rtc_close};
int32_t filesystem_reg_jumptable[4] = {(int32_t) open_fs, (int32_t) read_fs, (int32_t) write_fs, (int32_t) close_fs};
int32_t filesystem_dir_jumptable[4] = {(int32_t) open_fs, (int32_t) read_fs_dir, (int32_t) write_fs, (int32_t) close_fs};
int32_t keyboard_jumptable[4] = {(int32_t) keyboard_open, (int32_t) keyboard_read, (int32_t) keyboard_write, (int32_t) keyboard_close};
int32_t terminal_jumptable[4] = { (int32_t)terminal_open,  (int32_t)terminal_read, (int32_t) terminal_write, (int32_t) terminal_close};
int32_t* fop_tables[5] = {rtc_jumptable, filesystem_dir_jumptable, filesystem_reg_jumptable, keyboard_jumptable, terminal_jumptable};

uint32_t total_tasks = 1; 
uint32_t pid_bitpattern[] = {0,0,0,0,0,0,0};
uint32_t current_task;
uint32_t terminal_current_pid[3];
uint32_t active_terminal = 0;


/**
 * @brief      terminates the current process and replace it with it's parent
 *
 * @param status The status of terminating process sent by the process or exception handlers
 *
 * @return     this function doesn't return to it's caller, instead it will return to where
 * 				it's parent process executed it.
 */
int32_t halt (uint8_t status)
{
	cli();
	PCB_t* current;
	int32_t i;
	// current = (PCB_t*) (EIGHTMB - (current_task * EIGHTKB) - EIGHTKB);
	current = (PCB_t*) current_PCB();
	// kernelESP = current->kesp;
	// kernelEBP = current->kebp;
	total_tasks--;
	PCB_t* parentPCB;

	// putc('\n', current->terminal_id); // todo: make sure this will be printed into the right terminal maybe with putc(c, terminal)

	// clearing the file descriptor array
	for (i=0;i<MAX_FD;i++)
	{
		current -> fileArray[i].fd_jumptable = NULL;
		current -> fileArray[i].file_position = 0;
		current -> fileArray[i].inode_indx = 0;
	    current -> fileArray[i].flags = NOT_IN_USE;
	}

    if(current->pid > 3) { //has parent
    	parentPCB = (PCB_t*) current->parent_addr;

        tss.ss0 = KERNEL_DS;
        tss.esp0 = parentPCB->kernelStack;
        page_allocate((((parentPCB->pid)-1)* FOURMB + EIGHTMB), CP_INDEX, FOURMB_USER_PAGE);
        flush_TLB();

        current_task = parentPCB->pid;

    } else { //last shell reset
    	pid_bitpattern[(current->pid)-1] = 0;
        execute((unsigned char*) "shell");
    }

    for (i=0;i<7;i++)
	{
		if (priority_list_pid[i] == current->pid)
		{
			priority_list_pid[i] = current_task; //parent's pid
			break;
		}
	}

    if (exception_halt == 1)
    {
    	exception_halt = 0;
    	asm volatile("				\n\
    	movl   %0, %%esp 			\n\
		movl   %1, %%ebp			\n\
		movzbl   %%bl, %%ebx		\n\
		movl      %2, %%eax			\n\
		leave			   			\n\
    	ret 						\n\
    	"							\
    	::"r" (current->kesp), "r" (current->kebp), "r" (256));
    }


    asm volatile("					\n\
    	movl   %0, %%esp 			\n\
		movl   %1, %%ebp			\n\
		movzbl   %%bl, %%ebx		\n\
		movzbl   %2, %%eax			\n\
		leave			   			\n\
    	ret 						\n\
    	"							\
    	::"r" (current->kebp), "r" (current->kebp), "r" (status));

    return -1; 
}



/***********************************************************
* execute
* Description: Attempts to load and execute a new program, 
			   handing off the processor to the new program 
			   until it terminates.
* Inputs: pid, newPCB
* Outputs: none
* Side Effects: none
************************************************************/
int32_t execute(const uint8_t* command)
{
	cli();

	uint32_t pid, i;
	uint32_t physical_page_addr;
	uint32_t kernelStackAddr;
	uint32_t PCBAddr; 
	uint32_t kesp,kebp;
	PCB_t newPCB;
	PCB_t* newPCB_ptr;
	uint8_t k_command[1024]; 
	uint32_t peip;
	// int32_t flags;

    char args[128];
    uint8_t* scan;

    asm volatile ("                                     	\n\
 		movl  %%esp, %0										\n\
 		movl  %%ebp, %1										\n\
 		"                                                    \
		:  "=q" (kesp), "=q" (kebp));

	if ( strlen((char*) command) < 1) //null check
		return -1;

	if (total_tasks >= 7) // limited concurrent processes
		return -1;
	
	strcpy( (char*) k_command, (char*) command); //copy from user to kernel to prevent security leaks
	
	// ***Steps from Discussion***
	// 1. Parse args & Executable check ??!!
		/*checkExcutable will take the command and: 
		  -return -1 if program does not exist or
		  filename specified is not executable 
		  -return 256 if program dies by exception
		  -else return
		 */

	if (k_command == NULL)
		return -1;

	for (i=0;i<128;i++)
	{
		args[i] = '\0';
	}
	
	/* manage arguments */
	i =0;
	for (scan = k_command; '\0' != *scan && ' ' != *scan && '\n' != *scan; scan++);
    if ('\0' != *scan){
	    *scan++ = '\0';
	    for (; '\0' != *scan && ' ' != *scan && '\n' != *scan; scan++, i++)
	    {
	    	args[i] = *(char *)scan;
	    }
	    if ('\0' != *scan)
	        *scan++ = '\0';
	}
		  //   while ('\0' != *scan || '\n' != *scan) {
		  //       args[i] = *(char*)scan++;
		  //       i++;
		  //   }
		  //   if ('\0' != args[i])
		  //       args[i] = '\0';

	
	

	if (!is_executable(k_command))
		return -1;

	// 2. Set up program paging 
	// assigns a unique pid and increment number of running tasks
	total_tasks++; 
	for (i=0;i<7;i++)
	{
		if (pid_bitpattern[i] == 0)
		{
			pid_bitpattern[i] = 1;
			pid = i+1;
			break;
		}
	}

	
	/* 8MB is kernel page end then current 
	process given 4MB page */
	physical_page_addr = ((pid-1) * FOURMB) + EIGHTMB;
	page_allocate(physical_page_addr, CP_INDEX, FOURMB_USER_PAGE);
	
	flush_TLB();

	// 3. User-level Program Loader  ??!!
		/* Loads the new process to allocated VM at CP_VADDR */
	peip = loadProcess(k_command, (int32_t) CP_VADDR);
	
	// 4. PCB and kernel-mode stack
	kernelStackAddr = EIGHTMB - (pid * EIGHTKB);
	PCBAddr = kernelStackAddr - EIGHTKB;
	newPCB_ptr = (PCB_t*) PCBAddr;

	//copy to designated location at the kerenl page
	PCB_init(&newPCB, pid, args);
	memcpy((void*) newPCB_ptr, (void*) &newPCB, sizeof(PCB_t)); 

	// 5. Context Switch - create own stack  
	tss.ss0 = KERNEL_DS;
	tss.esp0 = kernelStackAddr; 
	newPCB_ptr->kernelStack = kernelStackAddr;
	newPCB_ptr->kernelSS = KERNEL_DS;
	current_task = pid;

	newPCB_ptr->kesp = kesp;
	newPCB_ptr->kebp = kebp;
	newPCB_ptr->processState = running;
	 
	terminal_current_pid[active_terminal] = pid;
		
	//call IRET 	
	//setup the kernel stack for fake IRET
	 asm volatile ("                                        \n\
		pushl %0											\n\
		pushl %1											\n\
		pushl $0x0200										\n\
		pushl %2											\n\
		pushl %3											\n\
		iret 												\n\
		"                                                    \
		: 												     \
		: "r" (USER_DS), "r" (USERSTACKADDR-4), "r" (USER_CS), "r" (peip));


    return 0;

}

/**
 * @brief      wrapper and checker to read from various drivers
 *
 * @param[in]  fd      file descriptor index
 * @param      buf     The buffer to be filled
 *
 * @return     0 on success
 */
int32_t read(int32_t fd, uint8_t* buf, int32_t nbytes)
{
	int32_t retval;
	PCB_t* current;

	if (fd >= MAX_FD || fd < 0)
		return -1; // invalid fd

	if (fd == 1)
		return -1; // terminal read is not allowed

	current = (PCB_t*) current_PCB();
	if (!(current->fileArray[fd].flags))
		return -1; // fd is not in use (not opened yet, wrong fd)

	if (buf == NULL)
		return -1; //null pointer

   	retval = ((int32_function*)(current->fileArray[fd].fd_jumptable[1])) (fd, buf, nbytes);
	
	return retval;
}

/**
 * @brief      wrapper and checker to read from various drivers
 *
 * @param[in]  fd      file descriptor index
 * @param[in]  buf     The buffer to be filled
 * @param[in]  nbytes  The nbytes
 *
 * @return     return 0 on success
 */
int32_t write(int32_t fd, const uint8_t* buf, int32_t nbytes)
{
	int32_t retval;
	PCB_t* current;

	if (fd >= MAX_FD || fd < 0)
		return -1; // invalid fd

	if (fd == 0)
		return -1; // keyboard write is not allowed
	
	current = (PCB_t*) current_PCB();
	if (!(current->fileArray[fd].flags))
		return -1; // fd is not in use (not opened yet, wrong fd)

	if (buf == NULL)
		return -1; //null pointer

   	retval = ((int32_function*)(current->fileArray[fd].fd_jumptable[2])) (fd, buf, nbytes);
	
	return retval;
}

/**
 * @brief     opens a new file descriptor structure inside file array of the current 
 *				proccess.
 * @param[in]  file on the filesystem
 *
 * @return     fd index on success and -1 on fail
 */
int32_t open(const uint8_t* filename)
{
	int32_t length = strlen((char*) filename);

	if (filename == NULL) // null pointer or empty string
		return -1;

	if (!strncmp( (char*)filename, (char*) "",length))
		return -1;

	uint16_t fd; //file descriptor index
	int32_t retval;
	dentry_t file_dentry;
	PCB_t* current;

	if (read_dentry_by_name(filename, &file_dentry)){
		// printf("%s was not found\n",filename);
		return -1;
	}
	
	current = (PCB_t*) current_PCB();
	for (fd=MIN_FD;fd<MAX_FD;fd++) // Min is 2 and max is 8 discarding 0 and 1 
	    if (!(current->fileArray[fd].flags))
	   		break;
	
	if (fd == MAX_FD)
		return -1;

	current->fileArray[fd].fd_jumptable = fop_tables[file_dentry.filetype];
    current->fileArray[fd].flags = IN_USE;
    current->fileArray[fd].file_position = 0;
	if (file_dentry.filetype == 2)
    	current->fileArray[fd].inode_indx = file_dentry.idx_inode;
    else
    	current->fileArray[fd].inode_indx = 0;

    retval = ((int32_function*)(current->fileArray[fd].fd_jumptable[0])) (filename);

	return (int32_t) fd;
}

/**
 * @brief      
 * close a file from file array of current process
 *
 * @param[in]  fd    file descriptor index
 *
 * @return     0 upon success
 */
int32_t close(int32_t fd)
{
	PCB_t* current;

	if (fd >= MAX_FD || fd < MIN_FD)
		return -1; // invalid fd

	current = (PCB_t*) current_PCB();
	if (!current->fileArray[fd].flags)
		return -1; // fd is not in use (not opened yet, wrong fd)

	current->fileArray[fd].fd_jumptable = NULL;
    current->fileArray[fd].flags = NOT_IN_USE;
    current->fileArray[fd].file_position = 0;
    current->fileArray[fd].inode_indx = 0;

	return 0;
}

/**
 * @brief      This function used in combination with execute
 * 				to give access command's argument to user process.
 *
 * @param      buf     address to fill argument string
 * @param[in]  nbytes  number of bytes to read from arguments
 *
 * @return     0 on success, -1 on failure
 */
int getargs(unsigned char* buf, int nbytes)
{
	PCB_t* current;
	char* scan;

	if (buf == NULL) //null pointer
		return -1;

	current = (PCB_t*) current_PCB();
	scan = current->args;

	if (*scan == '\0')
		return -1;


	while(nbytes > 0 && scan != '\0')
	{
		*buf++ = *scan++;
		nbytes--;
	}

	return 0; 

}
/***********************************************************
* vidmap
* Description: Maps the text-mode video memory into user 
			   space at a pre-set virtual address.
* Inputs: screen_start
* Outputs: none
* Side Effects: Manipulates video memory. 
************************************************************/
int32_t vidmap (uint8_t** screen_start)
{
	if (screen_start == NULL) // null pointer
		return -1;
	
	//Parse the input and catch error if out of bounds
	uint32_t  lowerBound;
	uint32_t  upperBound;
	lowerBound =  USR_LOW_BOUND;
	upperBound = (USR_LOW_BOUND + FOURMB); 
	
	if((uint32_t) screen_start <  lowerBound || (uint32_t) screen_start > upperBound)
	{
		return -1;
	}

	pageTableUserVideo[0] = (uint32_t) (VIDMEMADDR) | 0x07; //user present
	clear(pcb_terminal);
	flush_TLB();

	*screen_start = (uint8_t*) (USRVIDMAPADDR);
	
	return 0;
}

int set_handler(int signum, void* arg)
{
	return -1; //not implemented yet

} //extraC
int sigreturn(void)
{
	return -1; //not implemented yet


}

/**
 * helpers
 */


int32_t current_PCB(){

	int32_t current;

	// pcb is placed on top a the 8kb section for each process kernel stack reserved space
	 asm volatile ("									\n\
 		movl   %%esp,%%eax 								\n\
 		andl   $0xFFFFE000,%%eax 						\n\
 		movl   %%eax,%0 								\n\
 		"                                                \
		:  "=r" (current));

	 return current;
}

/***************************************************************
* PCB_init
* Description: Initializes Process Control Block (PCB).
* Inputs: newPCB, pid, args, n_arg
* Outputs: none
* Side Effects: none
***************************************************************/
int32_t PCB_init(PCB_t* newPCB, uint32_t pid, char* args) 
{
    int32_t i,j;
	newPCB -> pid = pid;
	newPCB -> processState = started; 
	newPCB -> terminal_id = active_terminal;
	newPCB -> parent_addr = -1;

    for(i = 2; i < PROCESSNUMMAX; i++) 
	{
		newPCB -> fileArray[i].file_position = 0;
        newPCB -> fileArray[i].flags = NOT_IN_USE;
		newPCB -> fileArray[i].fd_jumptable = 0;
		newPCB -> fileArray[i].inode_indx = 0;
    }

	
	if (pid>3) //child of the shell
 	{
 		int32_t parent_PCB;
 		PCB_t* parent_PCB_ptr;
 		parent_PCB = current_PCB();
 		parent_PCB_ptr = (PCB_t*) parent_PCB;
 		newPCB->terminal_id = parent_PCB_ptr->terminal_id;
 		newPCB->parent_addr = parent_PCB;
 		
		for(i=0;i<7;i++)
			if (priority_list_pid[i]==parent_PCB_ptr->pid)
				break;
		
 	}
 	else
 	{
 		i = 0;
		j = 0;
 		while (priority_list_pid[i] > 0) 
		{
			i++;
			j++;
			// end of the priority array
			if (i > 6) i = 0; 
			// limit reached
			if (j>6) return -1;
		}
 	}

 	priority_list_pid[i] = pid;
 	current_schedule = i;
 	strcpy(newPCB->args,args);

	/* When a process is started,the kernel should automatically
	open stdin and stdout, which correspond to ﬁle descriptors 0
	and 1, respectively. stdin is a read-only ﬁle which corresponds
	to keyboard input. stdout is a write-only ﬁle corresponding to
	terminal output. */


	newPCB -> fileArray[0].fd_jumptable = keyboard_jumptable;
	newPCB -> fileArray[0].file_position = 0;
	newPCB -> fileArray[0].inode_indx = 0;
	newPCB -> fileArray[0].flags = IN_USE;

	newPCB -> fileArray[1].fd_jumptable = terminal_jumptable;
	newPCB -> fileArray[1].file_position = 0;
	newPCB -> fileArray[1].inode_indx = 0;
    newPCB -> fileArray[1].flags = IN_USE;
	
    return 0;
}
