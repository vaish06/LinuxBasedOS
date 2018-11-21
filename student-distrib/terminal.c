/**********************************
* terminal.c 
* ECE 391 MP3 
* donnNNN_doo_DAT_2_me
* This file contains the terminal 
* driver. 
***********************************/

#include "terminal.h"
#include "keyboard.h"
#include "lib.h"

/*******************************************************
* terminal_write
* Description: Writes desired bytes from buffer to screen. 
* Inputs: buffer & nbytes (max length of buffer)
* Outputs: -1 (error writing) or nbytes
* Side Effects: Writes to the screen. 
********************************************************/
int32_t terminal_write(int32_t fd, const uint8_t* buf, int32_t nbytes)
{
	//check for invaled nbytes, null input buf
	if (nbytes < 0 || buf == NULL )
		return -1;

	enterStatus = 0;

	int32_t i;
	//write nbytes from buf to screen
	for (i = 0; i < nbytes; i++){
		if (buf[i] != '\0' ){
			putc(buf[i], pcb_terminal); // change putc
		}
	}
	//finished, return number of bytes written
	return nbytes;
}

/*******************************************************
* terminal_open
* Description: Opens the terminal window. 
* Inputs: none
* Outputs: none
* Side Effects: Manipulates video memory to display. 
********************************************************/
int32_t terminal_open(const uint8_t* filename)
{
	enterStatus = 0;
	clear_keyboardBuffer(pcb_terminal);
	clear(pcb_terminal);
	return 0;
}

/*******************************************************
* terminal_read
* Description: terminal reading is not available. 
* Inputs: none
* Outputs: none
* Side Effects: none.
********************************************************/
//read nbytes from keyboard to buf
int32_t terminal_read(int32_t fd, uint8_t* buf, int32_t nbytes)
{
	return -1;
}

/*******************************************************
* terminal_close
* Description: Closes the terminal window. 
* Inputs: none
* Outputs: none
* Side Effects: none. 
********************************************************/
int32_t terminal_close(int32_t fd)
{
	return -1;
}
