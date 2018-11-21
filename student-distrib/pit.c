#include "pit.h"
#include "lib.h"
#include "i8259.h"
#include "types.h"

#define PIT_FREQUENCY 1193182
#define CHANNEL_ZERO  0x40
#define CHANNEL_ONE   0x41
#define CHANNEL_TWO   0x42
#define COMMAND_PORT  0x43
#define COMMAND_INPUT 0x36 //00110110 --Channel 0, Access Mode: lobyte/hibyte
						   // Operating Mode: Rate Generator, Binary Mode
						   // Rate Generator is a frequency divider. 
#define DEFAULT_FREQ  20 
#define BITMASK 0xFF //lobytes
#define SHIFT_VAL 8  //hibytes


void pit_init()
{	
	disable_irq(0);

	uint32_t divisor = PIT_FREQUENCY / DEFAULT_FREQ;


	outb(COMMAND_INPUT, COMMAND_PORT);
	outb(divisor & BITMASK, CHANNEL_ONE);
	outb(divisor >> SHIFT_VAL, CHANNEL_ONE);


	enable_irq(0);
}


void pit_update(uint32_t frequency)  
{
	disable_irq(0);
	int32_t divisor = PIT_FREQUENCY / frequency;

	// outb(COMMAND_INPUT, COMMAND_PORT);
	outb(divisor & BITMASK, CHANNEL_ONE);
	outb(divisor >> SHIFT_VAL, CHANNEL_ONE);
	enable_irq(0);
}
