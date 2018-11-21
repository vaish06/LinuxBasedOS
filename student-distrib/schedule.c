#include "types.h"
#include "terminal.h"
#include "system_calls.h"
#include "lib.h"
#include "i8259.h"
#include "pit.h"
#include "paging.h"
#include "schedule.h"
#include "x86_desc.h"

int32_t counter = 0;
extern int32_t schedule_started;
extern uint32_t active_terminal;

void switch_tasks(void);

/**
 * @brief      scheduler wrapper (spinlocks)
 */
void schedule(void)
{
	cli();
	switch_tasks();
	sti();
}

void scheduler_init(void)
{
	int32_t i;
	schedule_started = 0;
	current_schedule = -1;
	active_terminal = 3;
	for (i=0;i<7;i++)
		priority_list_pid[i] = -1;
}

/**
 * @brief      reserve the current process stack pointer and segment register
 * 				and switch to the next task in the runqueue
 */
void switch_tasks(void)
{
	uint32_t physical_page_addr;
	uint32_t kernelStackAddr;
	PCB_t* PCB_ptr; 
	int32_t planned_schedule, next_pid;
	PCB_t* current;

	current = (PCB_t*) current_PCB();
	asm volatile ("                                     	\n\
		movl  %%esp, %0										\n\
		movl  %%ebp, %1										\n\
		"                                                    \
	:  "=q" (current->sesp), "=q" (current->sebp));


	planned_schedule = (current_schedule+1) % 7;

	if (schedule_started == 1)
	{
		while (priority_list_pid[planned_schedule] < 0) 
		{
			planned_schedule++;
			if (planned_schedule > 6)
				planned_schedule = 0; 
		}

		next_pid = priority_list_pid[planned_schedule];

		// switch the pagings
		physical_page_addr = ((next_pid - 1) * FOURMB) + EIGHTMB;
		page_allocate(physical_page_addr, CP_INDEX, FOURMB_USER_PAGE);
        flush_TLB();

        current_schedule = planned_schedule;

    	send_eoi(0);

		// 5. Context Switch - restore stack 
		kernelStackAddr = EIGHTMB - (next_pid * EIGHTKB);
		PCB_ptr = (PCB_t*) (kernelStackAddr - EIGHTKB);
		tss.ss0 = KERNEL_DS;
		tss.esp0 = kernelStackAddr; 

		current->processState = waiting;
		PCB_ptr->processState = running;

		pcb_terminal = current->terminal_id;

		asm volatile("				\n\
    	movl   %0, %%esp 			\n\
		movl   %1, %%ebp			\n\
    	"							\
    	::"r" (PCB_ptr->sesp), "r" (PCB_ptr->sebp));


	}
	else
	{

		active_terminal = active_terminal - 1;
		if (active_terminal==0)
			schedule_started = 1;

    	send_eoi(0);

		execute((unsigned char*) "shell"); //base shells
		
	}
}
