/**********************************
* schedule.h 
* ECE 391 MP3 
* donnNNN_doo_DAT_2_me
* Header for paging.c 
***********************************/
#include "types.h"

#ifndef SCHEDULE_H
#define SCHEDULE_H


int32_t priority_list_pid[7];
int32_t current_schedule;
int32_t schedule_started;

void schedule(void);
void scheduler_init(void);


#endif
