#ifndef _IDT_H
#define _IDT_H

#define PIT_VECTOR     		0x20
#define KB_VECTOR 			0x21
#define RTC_VECTOR 			0x28
#define SYSTEMCALL_VECTOR 	0x80

extern void idt_init();

#endif
