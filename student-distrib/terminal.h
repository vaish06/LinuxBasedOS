/**********************************
* terminal.h 
* ECE 391 MP3 
* donnNNN_doo_DAT_2_me
* Header for terminal.c 
***********************************/

#include "types.h"
#ifndef TERMINAL_H
#define TERMINAL_H

#define BUFFER_SIZE 128
#define TERMINAL_QUANTITY 3

int32_t terminal_read(int32_t fd, uint8_t* buf, int32_t nbytes);
int32_t terminal_write(int32_t fd, const uint8_t* buf, int32_t nbytes);
int32_t terminal_open(const uint8_t* filename);
int32_t terminal_close(int32_t fd);

//some globals for keyboard shenanigans
unsigned char keyboardBuffer[TERMINAL_QUANTITY][BUFFER_SIZE];
volatile unsigned int keyboardBufferIdx[TERMINAL_QUANTITY];
volatile unsigned int enterStatus;
uint32_t pcb_terminal;
uint32_t process_request;
//terminal1 terminal;

#endif
