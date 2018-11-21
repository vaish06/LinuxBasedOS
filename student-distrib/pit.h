/**********************************************
* scheduler.h 
* ECE 391 MP3 
* donnNNN_doo_DAT_2_me
* Header for scheduler.c 
*----------------------------------------------
* References for Concepts:
* https://wiki.osdev.org
	/Programmable_Interval_Timer
* http://www.jamesmolloy.co.uk/tutorial_html/
	5.-IRQs%20and%20the%20PIT.html
**********************************************/
#include "types.h"
#ifndef SCHEDULER_H
#define SCHEDULER_H

void pit_init();
void pit_update(uint32_t frequency);

#endif
