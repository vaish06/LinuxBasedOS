/**********************************
* paging.h 
* ECE 391 MP3 
* donnNNN_doo_DAT_2_me
* Header for paging.c 
***********************************/
#include "types.h"
#ifndef PAGING_H
#define PAGING_H

#define VIDMEMADDR      0x000B8000 
#define KERNELADDR      0x00400000  
#define USRVIDMAPADDR   0x20000000  

/* Make sure that you align your pages (page directory and page tables)
   on 4 kB boundaries. */
#define ONEKB 1024
#define FOURKB 4096
#define BYTESTOALIGNTO 4096

uint32_t pageFaultAddr; 
uint32_t pageDir[ONEKB] __attribute__((aligned (BYTESTOALIGNTO)));
uint32_t pageTable[ONEKB] __attribute__((aligned (BYTESTOALIGNTO)));
uint32_t pageTableUserVideo[ONEKB] __attribute__((aligned (BYTESTOALIGNTO)));


 void paging_init(uint32_t);
 void paging_exec();
int32_t page_allocate(int32_t physical_addr, int32_t PDE_index, int32_t PDE_flags);
void flush_TLB();

/**********************************************************
	* Set PG (Paging) bit 31 of CR0 to 1 to enable paging. 
	* Set PE (Protection Enable) bit 0 of CR0 to 1 to enable
	*	 segment-level protection. 
	* Load Page Directory Base Address into Page Directory 
	*    Base Register bits 12-31 of CR3. 
	* Set PCD (Page-level Cache Disable) bit 4 of CR3
	*    to 0 to cache page directory
	*	 directory. 
	* Set PWT (Page-level Writes Transparent) bit 3 of CR3
	*    to 0 to enable write-back caching.
	* Set PVI (Protected-mode Virtual Interrupts) bit 1 
	*    of CR4 to 1 to enable hardware support for VI flag
	* Set PSE (Page Sign Extension) bit 4 of CR4 to 0 for 
	*    1000 pages of 4kB from 0MB - 4MB. Set PSE to 1 for 
	*    a single 4MB page from 4MB - 8MB.
	* Mark all pages in 0MB - 4MB not in vid memory to not 
	*    present. Also mark 8MB - 4GB as not present. 
	* Align the pages. 
	* Set PGE (Page Global Enable) bit 7 of CR4 to 1 to 
	*    enable global page feature. We need global page 
	*    directory entry with Supervisor bit set to 1. 
	* Load 1st user level program (shell) at physical 8MB. 
	* Load 2nd user level program (run by shell) at 
	*     phys 12MB. 
	***********************************************************/
	
#endif
