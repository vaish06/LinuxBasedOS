#ifndef _SYSTEM_CALLS_WRAPPER_H
#define _SYSTEM_CALLS_WRAPPER_H

#include "system_calls.h"
#include "lib.h"

int32_t system_calls_linkage();
extern int32_t halt (uint8_t status);
extern int32_t execute(const uint8_t* command);
extern int32_t read(int32_t fd, uint8_t* buf, int32_t nbytes);
extern int32_t write(int32_t fd, const uint8_t* buf, int32_t nbytes);
extern int32_t open(const uint8_t* filename);
extern int32_t close(int32_t fd);
extern int32_t getargs(uint8_t* buf, int32_t nbytes);
extern int32_t vidmap(uint8_t** screen_start);
extern int32_t set_handler(int32_t signum, void* arg); //extraC
extern int32_t sigreturn(void); //extraC

#endif
