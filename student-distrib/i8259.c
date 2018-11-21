/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask = 0xff; /* IRQs 0-7  */
uint8_t slave_mask = 0xff;  /* IRQs 8-15 */


/* 
 * i8259_init
 *   DESCRIPTION: Initiatializing the PIC master and slave
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Enables the cascade on irq2
 */

void i8259_init(void) {

	outb(MASK_ALL_PORTS, MASTER_8259_DATA);
	outb(MASK_ALL_PORTS, SLAVE_8259_DATA);

	// Master initialization
	outb(ICW1, MASTER_8259_PORT); //selects master
	outb(ICW2_MASTER, MASTER_8259_DATA);
	outb(ICW3_MASTER, MASTER_8259_DATA);
	outb(ICW4, MASTER_8259_DATA);

	// Slave Slave PIC
	outb(ICW1, SLAVE_8259_PORT); // selects slave
	outb(ICW2_SLAVE, SLAVE_8259_DATA);
	outb(ICW3_SLAVE, SLAVE_8259_DATA);
	outb(ICW4, SLAVE_8259_DATA);

	//restore cached masks
	outb(master_mask, MASTER_8259_DATA);
	outb(slave_mask, SLAVE_8259_DATA);

	enable_irq(CASCADE_IRQ); //enable slave cascade


}

/* 
 * enable_irq
 *   DESCRIPTION: Enables the selected irq on input port
 *   INPUTS: irq_num - irq port number to activate
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void enable_irq(uint32_t irq_num) {

	//get the current mask from PIC
	uint8_t mask;
	if (irq_num & SLAVE_OFFSET){
		mask = ~(1 << (irq_num-SLAVE_OFFSET)); // TODO not sure if it works 
		slave_mask &= mask;
		outb(slave_mask, SLAVE_8259_DATA);
		
	}
	else{
		mask = ~(1 << irq_num);
		master_mask &= mask;
		outb(master_mask, MASTER_8259_DATA);
	}
}

/* 
 * disable_irq
 *   DESCRIPTION: Disables the selected irq on input port
 *   INPUTS: irq_num - irq port number to deactivate
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void disable_irq(uint32_t irq_num) {
	uint8_t mask;

	if (irq_num & SLAVE_OFFSET){
		mask = 1 << (irq_num-SLAVE_OFFSET); // TODO same as above
		slave_mask |= mask;
		outb(slave_mask, SLAVE_8259_DATA);
	}
	else{
		mask = 1 << irq_num;
		master_mask |= mask;
		outb(master_mask, MASTER_8259_DATA);
	}
}

/* 
 * send_eoi
 *   DESCRIPTION: Sends the EOI
 *   INPUTS: irq_num - irq port number to send EOI to
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void send_eoi(uint32_t irq_num) {
		unsigned int mask;


	if (irq_num & SLAVE_OFFSET){
		mask = EOI | (irq_num-SLAVE_OFFSET); // TODO same as above
		outb(mask, SLAVE_8259_PORT);
		outb(EOI | IRQ_SLAVE, MASTER_8259_PORT);

	}
	else{
		mask = EOI | irq_num;
		outb(mask, MASTER_8259_PORT);
	}

}
