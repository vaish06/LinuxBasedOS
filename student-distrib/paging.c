/**********************************
* paging.c 
* ECE 391 MP3 
* donnNNN_doo_DAT_2_me
* This file creates a page directory 
* and page table with pageDirs and pageTables
* to map virtual memory to physical
* memory. 
***********************************/
#include "paging.h"
#include "lib.h"

uint32_t pageFaultAddr; 
uint32_t pageDir[ONEKB] __attribute__((aligned (BYTESTOALIGNTO)));
uint32_t pageTable[ONEKB] __attribute__((aligned (BYTESTOALIGNTO)));
uint32_t pageTableUserVideo[ONEKB] __attribute__((aligned (BYTESTOALIGNTO)));


/*****************************************************
* paging_init
* Description: Initializes the paging feature. 
* Inputs: none
* Outputs: none
* Side Effects: Sets the page directory and tables. 
******************************************************/
void paging_init(uint32_t pageDir)
{
		/******************************************************
		* In-line MACRO breakdown: 
		* 1. Move input (page directory)(%1) to EAX. 
		* 2. Mask such that bits 3 and 4 of CR3 are set to 0. 
		* 3. Move EAX contents to CR3. 
		*******************************************************/
		asm volatile ("                                               \n\
			movl %0, %%eax                                            \n\
			movl %%eax, %%cr3                                         \n\
			"                                                           \
			:                        /* no output */                    \
			: "r"(pageDir)           /* input */                        \
			: "%eax"                 /* clobbered register */           \
		);
}

/*****************************************************
* paging_exec
* Description: Executes the paging feature. 
* Inputs: none
* Outputs: none
* Side Effects: Flips on the paging enable flags. 
******************************************************/
void paging_exec()
{	
	int i;
	uint32_t pgDirEntry; 
	uint32_t pgTableEntry;
	
	for (i = 0; i < ONEKB; i++)
	{
		pageDir[i] = 0x0; 
		pageTable[i] = 0x0;	//initiate all pages to 0 
		pageTableUserVideo[i] = 0x0;
	}

	pgDirEntry = ((uint32_t) pageTable) | 0x3; // enable and kernel privilage first page directory (includes videomemory)
	pageDir[0] = pgDirEntry;

	pgDirEntry = (uint32_t) KERNELADDR | 0x83;
	pageDir[1] = pgDirEntry;
	
	pgTableEntry = (uint32_t) VIDMEMADDR | 0x3; // enable and kernel privilage first page Table
	pageTable[184] = pgTableEntry;
	pageTable[185] = (uint32_t) (VIDMEMADDR) | 0x3;
	
	pageTable[186] = (uint32_t) (VIDMEMADDR + 2*FOURKB) | 0x3;
	pageTable[187] = (uint32_t) (VIDMEMADDR + 3*FOURKB) | 0x3;

	pageTable[188] = (uint32_t) (VIDMEMADDR + 4*FOURKB) | 0x3;
	pageTable[189] = (uint32_t) (VIDMEMADDR + 5*FOURKB) | 0x3;

	pageTable[190] = (uint32_t) (VIDMEMADDR + 6*FOURKB) | 0x3;
	pageTable[191] = (uint32_t) (VIDMEMADDR + 7*FOURKB) | 0x3;


	pageDir[128] = ((uint32_t) pageTableUserVideo) | 0x7;

	// pgDirEntry = (uint32_t) (KERNELADDR * 2) | 0x87;
	// pageDir[2] = pgDirEntry;


	// for (i=3;i<ONEKB; i++)
	// {
	// 	pageDir[i] = 0x2; // 0 present on 3rd and forward Page Director
	// }

	
	paging_init((uint32_t) pageDir);
	
	/******************************************************
	* In-line MACRO breakdown: 
	* 1. Move CR4 into EAX. (We can't directly manipulate..
	*	 Control registers allow only read and load.)
	* 2. Mask such that bits 1, 4, 7 are set to 1. 
	* 3. Move EAX to CR4. 
	* 4. Move CR0 to EAX. 
	* 5. Mask such that bits 0, 1, 31 are set to 1. 
	* 6. Move EAX into CR0. 
	* 7. Move CR2 to EAX. 
	* 8. Move EAX to output (%0). (Outputs page fault addr)
	*******************************************************/
	
	asm volatile ("                                               \n\
		movl %%cr4, %%eax                                         \n\
        orl  $0x10, %%eax                                         \n\
		movl %%eax, %%cr4                                         \n\
		movl %%cr0, %%eax                                         \n\
        orl $0x80000003, %%eax                                    \n\
        movl %%eax, %%cr0                                         \n\
		movl %%cr2, %%eax                                         \n\
		movl %%eax, %0                                            \n\
        "                                                           \
        : "=r" (pageFaultAddr)   /* output */                       \
        :                        /* no input */                     \
        : "%eax"                 /* clobbered register */           \
    );                   
	
	printf("Paging is enabled!\n");

	
}

/*****************************************************
* page_allocate
* Description: Allocate and set the flags on given physical address and PDE index. 
* Inputs: physical_addr, PDE_index, PDE_flags
* Outputs: Return 0 on success, otherwise returns -1
* Side Effects: change the flags. 
******************************************************/
int32_t page_allocate(int32_t physical_addr, int32_t PDE_index, int32_t PDE_flags)
{	
	pageDir[PDE_index] = physical_addr | PDE_flags;
	return 0;
}

/**
 * { item_description }
 */
void flush_TLB()
{
	asm volatile ("                                               \n\
			movl %%cr3, %%eax                                            \n\
			movl %%eax, %%cr3                                         \n\
			"                                                           \
			:                        /* no output */                    \
			: 						/* no input */	                    \
			: "%eax"                 /* clobbered register */           \
		);
}
