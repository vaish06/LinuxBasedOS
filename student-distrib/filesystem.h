#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include "lib.h"
#include "types.h"

#define FILENAME_LIMIT 32
#define FS_BLOCK	4096
#define REG_FILE	2
#define DIR_FILE	1
#define RTC_FILE	0
#define ELF_ENTRY	24

//Based on Appendix A (8.1), Lecture 16


typedef struct dentry{
	int8_t		filename[FILENAME_LIMIT]; //string
	int32_t		filetype; // 0:file with user access to RTC, 1: directory, 2:file
	int32_t		idx_inode; //index of corresponding inode for the dentry
	int8_t		reserve[24];
} dentry_t;

typedef struct bootblock
{
	int32_t num_dir; //total directories
	int32_t num_inode; //total file(dir and regular files)
	int32_t num_data;  // total data block
	int8_t  reserve[52];
	dentry_t dentry[63]; 
} bootblock_t;

typedef struct inode
{
	int32_t length;
	int32_t dblocks[1023];
} inode_t;

extern void filesystem_init();
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
int32_t open_fs(const uint8_t* filename);
int32_t read_fs(int32_t fd, uint8_t* buf, int32_t nbytes);
int32_t read_fs_dir(int32_t fd, uint8_t* buf, int32_t nbytes);
int32_t write_fs(int32_t fd, uint8_t* buf, int32_t nbytes);
int32_t close_fs(int32_t fd);
int32_t get_filesize_by_name(const uint8_t* fname);
int32_t is_executable(const uint8_t* filename);
uint32_t loadProcess(const uint8_t* command, int32_t virtual_addr);

#endif

