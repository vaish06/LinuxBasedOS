#ifndef _SYSTEM_CALLS_H
#define _SYSTEM_CALLS_H

#include "types.h"

#define MAX_FD 8
#define MIN_FD 2
#define FOURMB 0x00400000
#define EIGHTMB 0x00800000
#define EIGHTKB 0x00002000
#define USR_LOW_BOUND 0x08000000
#define CP_VADDR 0x08048000
/* Bit mask sets Page Size to 4MB, 
Privilege is User, Read/Write Enable, Present */
#define FOURMB_USER_PAGE 0x87 
#define CP_INDEX 32
#define USERSTACKADDR 0x08400000
#define PROCESSNUMMAX 8
#define NOT_IN_USE  0
#define IN_USE  1
#define started 0
#define ready 1
#define running 2
#define waiting 3
#define terminated 4

int32_t current_PCB();
int32_t halt (unsigned char status);
int32_t execute(const unsigned char* command);
int32_t read(int fd, unsigned char* buf, int nbytes);
int32_t write(int fd, const unsigned char* buf, int nbytes);
int32_t open(const unsigned char* filename);
int32_t close(int fd);
int32_t getargs(unsigned char* buf, int nbytes);
int32_t vidmap(unsigned char** screen_start);
int32_t set_handler(int signum, void* arg); //extraC
int32_t sigreturn(void); //extraC

typedef int32_t int32_function();

typedef struct file_descriptor
{
 int32_t* fd_jumptable; //open, read, write, close
 int32_t inode_indx;
 int32_t file_position;
 int32_t flags;
 
} file_descriptor_t;

typedef struct PCB
{
	uint32_t pid;
	uint32_t processState;
	uint32_t terminal_id; //terminal_id
	uint32_t sesp;
	uint32_t sebp;
	uint32_t kernelStack;
	uint32_t kernelSS; 
	uint32_t kesp; 
	uint32_t kebp; 
	uint32_t n_arg;
	char args[128];
	file_descriptor_t fileArray[8];
	int32_t parent_addr;	
} PCB_t;

#endif

