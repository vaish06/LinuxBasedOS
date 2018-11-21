/**********************************
* keyboard.h 
* ECE 391 MP3 
* donnNNN_doo_DAT_2_me
* Header for keyboard.c 
***********************************/

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "i8259.h"
#include "lib.h"
#include "terminal.h"

//codes for keyboard
#define LSHIFT	42
#define RSHIFT	54
#define CTRL	29
#define CAPSLOCK	58
#define ALT		56
#define BACKSPACE 14
#define TAB 15
#define ENTER 28
#define ESC 1
#define F1 59
#define F2 60
#define F3 61
#define TABLESIZE 128 
#define INPUTBUFFER 0x60
#define MASKONE 0x80
#define MASKTWO 0x7F



unsigned char keyboardUS[TABLESIZE];
unsigned char keyboardUSShift[TABLESIZE];
extern void enableKeyboard();
extern void keyboard_handler();
int32_t keyboard_read(int32_t fd, uint8_t* buf, int32_t nbytes);
int32_t keyboard_write(int32_t fd, const uint8_t* buf, int32_t nbytes);
int32_t keyboard_open(const uint8_t* filename);
int32_t keyboard_close(int32_t fd);
void clear_keyboardBuffer(uint32_t);

#endif /* _KEYBOARD_H */
