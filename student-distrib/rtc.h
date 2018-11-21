#include "types.h"

#ifndef _RTC_H
#define _RTC_H

void rtc_init();
void rtc_interrupt();
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t rtc_open(const uint8_t* filename);
int32_t rtc_close(int32_t fd);

#define RTC_REG_PORT		0x70
#define RTC_CMOS_PORT		0X71

#define DISABLE_NMI		0x80
#define RTC_REG_A		0x0A
#define RTC_REG_B 		0x0B
#define RTC_REG_C		0x0C

#define SET_BIT_6	0x40
#define RTC_IRQ		8

#define HIGH_RATE_MASK	0xF0
#define LOW_RATE_MASK	0x0F
#define BYTES4 			4
/* Default frequency */
#define RTC_DEF_RATE	0xF //1111 2Hz

/* Frequency constants. */
#define DEFAULT_FREQUENCY 2
#define HZ0			0x00
#define HZ2			0x0F
#define HZ4			0x0E
#define HZ8			0x0D
#define HZ16		0x0C
#define HZ32		0x0B
#define HZ64		0x0A
#define HZ128		0x09
#define HZ256		0x08
#define HZ512		0x07
#define HZ1024		0x06



#endif
