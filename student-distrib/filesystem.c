#include "filesystem.h"
#include "lib.h"
#include "types.h"
#include "system_calls.h"

bootblock_t* bootblock_ptr;
uint32_t inode_start_addr;
uint32_t data_start_addr;

// extern file_desriptor_t kernel_file_desriptor[8]; //cp2 temporary fd

/*****************************************************
* filesystem_init
* Description: Initializes the file system.
* Inputs: none
* Outputs: none
* Side Effects: none
******************************************************/
void filesystem_init(uint32_t start_addr)
{
	bootblock_ptr = (bootblock_t*) start_addr;
	inode_start_addr = start_addr + 1 * FS_BLOCK; //starts after bootblock (+1 Block)
	data_start_addr = inode_start_addr + bootblock_ptr->num_inode * FS_BLOCK; //starts after inode blocks 
}

/*****************************************************
* open_fs
* Description: Opens a file. 
* Inputs: none
* Outputs: none
* Side Effects: none
******************************************************/
int32_t open_fs(const uint8_t* filename)
{
	return 0; //moved to systemcall
}

/*****************************************************
* read_fs
* Description: Reads a file. 
* Inputs: none
* Outputs: none
* Side Effects: Creates new PCB. 
******************************************************/
int32_t read_fs(int32_t fd, uint8_t* buf, int32_t nbytes)
{
	int32_t i, idx_inode, read_bytes;
	dentry_t file;


	// if (nbytes != 4)
	// 	return -1;

	inode_t* inode_ptr = NULL;
	PCB_t* current;
	current = (PCB_t*) current_PCB();
	idx_inode = current->fileArray[fd].inode_indx;

	for(i=0;bootblock_ptr->num_dir;i++){
		read_dentry_by_index(i,&file);
		if (file.idx_inode == idx_inode)
			break;
	}

	if (i==bootblock_ptr->num_dir)
		return -1;

	if (file.filetype == REG_FILE) //regualr dentry
	{
		inode_ptr = (inode_t*) (inode_start_addr+idx_inode*FS_BLOCK);
	    read_bytes = read_data(idx_inode, current->fileArray[fd].file_position, buf, nbytes);
	}
	else if (file.filetype == DIR_FILE) //directory dentry
	{
		read_bytes = read_fs_dir(fd, buf, nbytes);
	}

	current->fileArray[fd].file_position += read_bytes;

    return read_bytes;

}

/*****************************************************
* read_fs_dir
* Description: Reads file system directory.  
* Inputs: none
* Outputs: none
* Side Effects: Creates new PCB. 
******************************************************/
int32_t read_fs_dir(int32_t fd, uint8_t* buf, int32_t nbytes)
{

	PCB_t* current;
	current = (PCB_t*) current_PCB();
	dentry_t file;

	// printf("Name\t\t\t\t\tType\tSize\n");
	// for (i=0;i<56;i++)
	// 	printf("-");

	if (read_dentry_by_index(current->fileArray[fd].file_position++,&file) ==0){
		strncpy((char*) buf, file.filename, nbytes);
		return nbytes;
	}
		return 0;




	// printf("\n");


	/*
	int32_t i,j;
	int32_t  length;
	inode_t* inode_ptr = NULL;
	while ()
	for (i=0;i<bootblock_ptr->num_dir;i++)
	{
		read_dentry_by_index(i,&file);
		length = 6 - strlen(file.filename)/8;
		if (file.filetype == 0)
		{
			printf("%s", file.filename);
			for(j=0; j<length-1;j++)
				printf("\t");
			printf("R\n");
		}
		else if(file.filetype == 1)
		{
			printf("%s", file.filename);
			for(j=0; j<length-1;j++)
				printf("\t");
			printf("D\n");
		}
		else
		{
			inode_ptr = (inode_t*) (inode_start_addr+file.idx_inode*FS_BLOCK);
			printf("%s", file.filename);
			for(j=0; j<length-1;j++)
				printf("\t");
			printf("F\t");
			printf("%d B\n", inode_ptr->length);

		}

	}
	return 0;*/
}


/*****************************************************
* write_fs
* Description: Writes a file system.
* Inputs: none
* Outputs: none
* Side Effects: Prints a message. 
******************************************************/
int32_t write_fs(int32_t fd, uint8_t* buf, int32_t nbytes)
{
	printf("This filesystem is readonly\n");
	return -1;
}

/**
 * @brief      Abstracted close systemcall
 *
 * @param[in]  fd    file descriptor index
 *
 * @return     0
 */
int32_t close_fs(int32_t fd)
{
	return 0; 
}


/*****************************************************
* read_dentry_by_name
* Description: Reads a dentry by name. 
* Inputs: none
* Outputs: none
* Side Effects: 
******************************************************/
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry)
{
	uint32_t i = 0;
	uint8_t length = strlen((int8_t*) fname);

	//if (length >= FILENAME_LIMIT)
		length = FILENAME_LIMIT - 1;

	while(i <= bootblock_ptr->num_dir) 
	{
		if ( !strncmp( (int8_t*) fname, bootblock_ptr->dentry[i].filename, length ) )
		{
			strncpy(dentry->filename, (int8_t*) bootblock_ptr->dentry[i].filename, length);
			dentry->filetype = bootblock_ptr->dentry[i].filetype;
			dentry->idx_inode = bootblock_ptr->dentry[i].idx_inode;
			return 0;
		}
		i++;
	}
	return -1;
}

/*****************************************************
* get_filesize_by_name
* Description: Gets a filesize by name. 
* Inputs: none
* Outputs: none
* Side Effects: Returns inode pointer length.
******************************************************/
int32_t get_filesize_by_name(const uint8_t* fname)
{
	dentry_t file;
	read_dentry_by_name(fname, &file);
	if (file.filetype == 2)
	{
		inode_t* inode_ptr = (inode_t*) (inode_start_addr+file.idx_inode*FS_BLOCK);
		return inode_ptr->length;
	}
	return 0;
}

/*****************************************************
* read_dentry_by_index
* Description: Reads dentry by index. 
* Inputs: none
* Outputs: none
* Side Effects: Copies memory from user to kernel.
******************************************************/
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry)
{
	if (index >= bootblock_ptr->num_dir)
		return -1;

	// if (bootblock_ptr->dentry[index].filetype != REG_FILE)
		// return -1;

	dentry_t current_dir = bootblock_ptr->dentry[index];

	//return as as refrence
	strncpy(dentry->filename, current_dir.filename, FILENAME_LIMIT-1);
	dentry->filetype = current_dir.filetype;
	dentry->idx_inode = current_dir.idx_inode;

	return 0; //success
}

/*****************************************************
* read_data
* Description: Reads file data. 
* Inputs: none
* Outputs: none
* Side Effects: Returns written bytes. 
******************************************************/
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
{
	uint16_t current_block, total_blocks, offset_block, idx_block, n, written_bytes;
	uint32_t current_data_addr;
	inode_t* inode_ptr = (inode_t*) (inode_start_addr+inode*FS_BLOCK);

	written_bytes = 0;

	if (inode >= bootblock_ptr->num_inode) // index is out of range of available inodes
		return -1;
	if (offset >= inode_ptr->length) // data at the offset is not available in the inode
		return written_bytes; //read 0 byte
	if ( (offset+length) >= inode_ptr->length ) // reading beyond available data
		length = inode_ptr->length - offset; // correct/limit to size of available data

	current_block = offset / FS_BLOCK;
	total_blocks = (offset + length) / FS_BLOCK;

	do {
		offset_block = offset % FS_BLOCK;
		idx_block = inode_ptr->dblocks[current_block];

		//check for off datablock (Apendix A)
		if (idx_block >= bootblock_ptr->num_data)
			return -1;

		if (length >= FS_BLOCK- offset_block)
			n = FS_BLOCK - offset_block; //remaining bytes in current block
		else
			n = length; // remaining is less than one block

		current_data_addr = data_start_addr + idx_block * FS_BLOCK + offset_block;

		memcpy(buf, (uint8_t*) current_data_addr, n);
		buf += n;

		offset = offset + n;
		length = length - n;
		written_bytes = written_bytes + n;

		// offset_block = 0; //aligned in first iteration
		current_block++;


	} while (current_block <= total_blocks);

	return written_bytes;
}


/**
 * @brief      Loads a process to preset address in VM and 8MB or 12MB
 * 				in physical memory.
 *
 * @param[in]  command       process name
 * @param[in]  virtual_addr  preset virtual address
 *
 * @return     0 success, -1 fail
 */
uint32_t loadProcess(const uint8_t* command, int32_t virtual_addr)
{
	dentry_t process_dentry;
	int32_t i, length;
	uint32_t  e_entry = 0;

	read_dentry_by_name(command, &process_dentry);
	length = get_filesize_by_name(command);
	uint8_t entry_buff[50];

	read_data(process_dentry.idx_inode,0,entry_buff,40);

	//load program to allocated userspace page
	read_data(process_dentry.idx_inode,0,(uint8_t*) virtual_addr,length);

    for (i=0;i<4;i++) // little endian 4 bytes to uint
    	e_entry |= (uint32_t)(entry_buff[ELF_ENTRY+i]) << i*8;


	return e_entry;
}

/**
 * @brief      Determines if executable.
 *
 * @param[in]  filename  The filename
 *
 * @return     0 if not executable
 * 			   1 if executable
 */
int32_t is_executable(const uint8_t* filename)
 {
 	if (filename == NULL)
 		return 0;

 	dentry_t file;
	if (read_dentry_by_name(filename, &file) == 0)
	{
		if ((file.filetype == 2) && (file.idx_inode != 0))
		{
			uint8_t buffer[4];
			read_data(file.idx_inode, 0, buffer, 4);
			//ELF Check
			if (buffer[0] == 0x7F && buffer[1] == 0x45 && buffer[2] == 0x4C && buffer[3] ==0x46)
			{
				return 1;
			}	
		}
	}
	return 0;
 }

