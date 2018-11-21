#include "idt.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"
#include "keyboard.h"
#include "system_calls_wrapper.h"
#include "schedule.h"

int32_t exception_halt;

/*void handle_rtc_interrupt()*/ 
/* Function: Helper function, whenever an interrupts happens its calling the rtc_interrupt
 * Inputs: none
 * Return Value: none
 */
void handle_rtc_interrupt()
{
    asm volatile ("           \
        pushal");
    rtc_interrupt();
    asm volatile ("popal    \n\
        leave               \n\
        iret");
}


void handle_pit_interrupt()
{
    // cli();
    asm volatile ("           \
        pushal");
    schedule();
    // sti();
    asm volatile ("popal    \n\
        leave               \n\
        iret");
}

void handle_keyboard_interrupt()
{
    asm volatile ("           \
        pushal");
	keyboard_handler();
	asm volatile ("popal    \n\
        leave               \n\
        iret");
}

#define HANDLER(exception,error)                \
void exception() {                              \
    printf("%s\n",#error);                      \
    while(1);                                   \
}

void page_handler()
{
    uint32_t pageFaultAddr; 
    uint32_t errorcode; 
    asm volatile ("                                               \n\
        movl %%cr2, %%eax                                         \n\
        movl %%eax, %0                                            \n\
        popl %1                                                   \n\
        "                                                           \
        : "=r" (pageFaultAddr), "=r" (errorcode)   /* output */     \
        :                        /* no input */                     \
        : "%eax"                 /* clobbered register */           \
    );   
    printf("Page fault address %x\n", pageFaultAddr);
    printf("ERROR 0x%x\n", errorcode);
    exception_halt = 1;
    while(1); 
}

/*Setting up the messages to be displayes for entries 0-18*/
HANDLER(EX_DE, "Divide Error"); 
HANDLER(EX_DB, "Debug");
HANDLER(EX_NMI, "Not Maskable Interrupt");
HANDLER(EX_BP, "Breakpoint");
HANDLER(EX_OF, "Overflow");
HANDLER(EX_BR, "BOUND Range Exceeded");
HANDLER(EX_UD, "Invalid/Undefined Opcode");
HANDLER(EX_NM, "Device Not Available (No Math coprocessor)");
HANDLER(EX_DF, "Double Fault");
HANDLER(EX_MF, "CoProcessor Segment Overrun");
HANDLER(EX_TS, "Invalid TSS");
HANDLER(EX_NP, "Segment Not Present");
HANDLER(EX_SS, "Stack Segment Fault");
HANDLER(EX_GP, "General Protection!");
HANDLER(EX_PF, "Page Fault");
HANDLER(EX_MA, "Floating-Point Error (Math Fault)");
HANDLER(EX_AC, "Alignment Check");
HANDLER(EX_MC, "Machine Check");
HANDLER(EX_XF, "SIMD Floating-Point Exception");

HANDLER(EX_RESERVED, "Reserved!");
/*void idt_init()*/ 
/* Function: This function would initiate the data table for exceptions and system calls
 * Inputs: none
 * Return Value: none
 */
void idt_init()
{
    void* exception_ptr_table[] = { EX_DE, EX_DB, EX_NMI,
                                    EX_BP, EX_OF, EX_BR,
                                    EX_UD, EX_NM, EX_DF,
                                    EX_MF, EX_TS, EX_NP,
                                    EX_SS, EX_GP, EX_PF, 
                                    NULL,  EX_MA, EX_AC,
                                    EX_MC, EX_XF};
    int i;
    idt_desc_t int_vector, exception_vector, syscall_vector; //int_vector for both interrupts and syscalls

    //set options for exception vectors in idt
    exception_vector.seg_selector = KERNEL_CS; // runs in kernel
    exception_vector.reserved4 = 0; //reserved4
    exception_vector.reserved3 = 1; //reserved3
    exception_vector.reserved2 = 1; //reserved2
    exception_vector.reserved1 = 1; //reserved1
    exception_vector.size = 1; // size
    exception_vector.reserved0 = 0; // reserved0
    exception_vector.dpl = 0; // Kernel privilage mode
    exception_vector.present = 1; // Present


    int_vector = exception_vector; // intrrupts have similar options with diffrence in reserve3
    int_vector.reserved3 = 0;

    syscall_vector = int_vector;
    syscall_vector.dpl = 3;

    //32 reserved exceptions defined by intel 
    for (i=0x0;i<0x20;i++)
    {
        idt[i] = exception_vector; // set options for exceptions
        if ((i<20) && (i != 15) && (i != 14)){
            SET_IDT_ENTRY(idt[i], exception_ptr_table[i]);
        }
        else if (i == 14)
        {
            SET_IDT_ENTRY(idt[i], (void*) page_handler);
        }
        else
        {
            SET_IDT_ENTRY(idt[i], EX_RESERVED);
        }
    }   

    
    //set and activate irq interrupt handlers on idt
    idt[RTC_VECTOR] = int_vector; //RTC
    idt[PIT_VECTOR] = int_vector; //PIT
    idt[KB_VECTOR] = int_vector; // Keyboard
    idt[SYSTEMCALL_VECTOR] = syscall_vector; // system_calls
	
    SET_IDT_ENTRY(idt[RTC_VECTOR], (void*) handle_rtc_interrupt); // rtc idt entry 
	SET_IDT_ENTRY(idt[PIT_VECTOR], (void*) handle_pit_interrupt); // rtc idt entry 
	SET_IDT_ENTRY(idt[KB_VECTOR], (void*) handle_keyboard_interrupt); // keyboard idt entry
    SET_IDT_ENTRY(idt[SYSTEMCALL_VECTOR], (int32_t*) system_calls_linkage); //system calls idt entry

 
    lidt(idt_desc_ptr); //refill idt with updated values

}
