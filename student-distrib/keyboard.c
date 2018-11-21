/**********************************
* keyboard.c
* ECE 391 MP3 
* donnNNN_doo_DAT_2_me
* Keyboard functions
***********************************/

#include "types.h"
#include "keyboard.h"
#include "lib.h"
#include "terminal.h"



/* keyflags | alt, ctrl, shift | */
static int altStatus = 0;
static int F1Status = 0;
static int F2Status = 0;
static int F3Status = 0;
static int ctrlStatus = 0;
static int shiftStatus = 0;
static int capsStatus = 0;

extern uint32_t active_terminal;

/* This is US keyboard scancode lookup table
* info from; http://www.osdever.net/bkerndev/Docs/keyboard.htm
* scancodes from; https://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
*/
/*****************************************************
* keyboardUS
* Description: US keyboard scancode lookup table.
* Inputs: scancode
* Outputs: ASCII character to print
* Side Effects: none
******************************************************/
unsigned char keyboardUS[TABLESIZE] =
{
    0,  /*error*/ 27, /*esc*/ '1', '2', '3', '4', '5', '6', '7', '8',		/*9*/
  '9', '0', '-', '=', 0,	/*bckspc*/ '\t', /*tab*/ 'q', 'w', 'e', 'r',	/*19*/
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/*enter*/ 0,			/* 29-Ctrl*/
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/*42-Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/*54-Right shift */
  '*',
    0,	/*56-Alt*/
  ' ',	/*Space*/
    0,	/*Caps*/
    0,	/*59-F1 key...*/
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/*...F10*/
    0,	/*69-Num lock*/
    0,	/*Scroll Lock*/
    0,	/*Home key*/
    0,	/*Up Arrow*/
    0,	/*Page Up*/
  '-',
    0,	/*Left Arrow*/
    0,
    0,	/*Right Arrow*/
  '+',
    0,	/*79 - End key*/
    0,	/*Down Arrow */
    0,	/*Page Down */
    0,	/*Insert Key */
    0,	/*Delete Key */
    0,   0,   0,
    0,	/*F11 Key*/
    0,	/*F12 Key*/
    0,	/*other keys undefined*/
};

/*****************************************************
* keyboardUSShift
* Description: Shift pressed scancode lookup table.
* Inputs: scancode
* Outputs: shifted ASCII character to print
* Side Effects: none
******************************************************/
unsigned char keyboardUSShift[TABLESIZE] =
{
    0,  /*error*/ 27, /*esc*/ '!', '@', '#', '$', '%', '^', '&', '*',		/*9*/
  '(', ')', '_', '+', 0,	/*bckspc*/ '\t', /*tab*/ 'Q', 'W', 'E', 'R',	/*19*/
  'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',	/*enter*/ 0,			/* 29-Ctrl*/
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',	/* 39 */
 '\"', '~',   0,		/*42-Left shift */
 '|', 'Z', 'X', 'C', 'V', 'B', 'N',			/* 49 */
  'M', '<', '>', '?',   0,				/*54-Right shift */
  '*',
    0,	/*56-Alt*/
  ' ',	/*Space*/
    0,	/*Caps*/
    0,	/*59-F1 key...*/
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/*...F10*/
    0,	/*69-Num lock*/
    0,	/*Scroll Lock*/
    0,	/*Home key*/
    0,	/*Up Arrow*/
    0,	/*Page Up*/
  '-',
    0,	/*Left Arrow*/
    0,
    0,	/*Right Arrow*/
  '+',
    0,	/*79 - End key*/
    0,	/*Down Arrow */
    0,	/*Page Down */
    0,	/*Insert Key */
    0,	/*Delete Key */
    0,   0,   0,
    0,	/*F11 Key*/
    0,	/*F12 Key*/
    0,	/*other keys undefined*/
};		


/*************************************************************
* enableKeyboard
* Description: Enable interrupt requests for keyboard: irq_1 
* Inputs: none
* Outputs: none
* Side Effects: Enables interrupt request 1. 
**************************************************************/
void enableKeyboard()
{
	int32_t flags;
    cli_and_save(flags);
    enable_irq(1);
    restore_flags(flags);
}

/*************************************************************
* keyboard_handler
* Description: Keyboard interrupt handler. 
* Inputs: none
* Outputs: none
* Side Effects: Handles interrupt and sends EOI signal. 
**************************************************************/
void keyboard_handler()//(struct regs *r)
{
	cli();
	disable_irq(1); //temporary disable interrupts from keyboard irq line
    unsigned char scancode = 0;
	unsigned char returnChar = 0;

    /* Read keyboard buffer */
    scancode = inb(INPUTBUFFER);
	

    /* top bit of byte read from kb means key was just released */
    if (scancode & MASKONE)
    {
		unsigned char tempCode = scancode & MASKTWO; //mask out top bit
		
        /* left or right shift was just released */
		if (tempCode == LSHIFT || tempCode == RSHIFT){
			shiftStatus = 0;
			//printf("shift unpressed");
		}
		
		/* ctrl was just released */
		if (tempCode == CTRL){
			ctrlStatus = 0;
		}
		
		/* alt was just released */
		if (tempCode == ALT){
			altStatus = 0;
		}
		
		/* F1 was just released */
		if (tempCode == F1){
			F1Status = 0;
		}
		/* F2 was just released */
		if (tempCode == F2){
			F2Status = 0;
		}
		/* F3 was just released */
		if (tempCode == F3){
			F3Status = 0;
		}

    }
    else
    {
        /* kb key was just pressed */
		if (scancode == LSHIFT || scancode == RSHIFT){
			shiftStatus = 1;
			//printf("shift pressed");
		}
		if (scancode == CTRL){
			ctrlStatus = 1;
		}
		
		/* alt was just pressed */
		if (scancode == ALT){
			altStatus = 1;
		}

		/* F1 was just pressed */
		if (scancode == F1){
			F1Status = 1;
		}
		/* F2 was just pressed */
		if (scancode == F2){
			F2Status = 1;
		}
		/* F3 was just pressed */
		if (scancode == F3){
			F3Status = 1;
		}
		
		/* capsLock was just pushed, cycle on/off */
		if (scancode == CAPSLOCK){
			if (capsStatus == 0)
				capsStatus = 1;
			else
				capsStatus = 0;
		}
		
		if (scancode == ENTER){
			enterStatus  = 1;
		//	unsigned char buffer[128];
		// int i;
		// for (i = 0; i < 128; i++){
			// buffer[i] = '\0';
		// }
		// terminal_read(1, buffer, 128);
		// terminal_write(1, buffer, 128);
		}
		
		
		int capitilize = 0;
		/* if shift/caps pushed lookup scancode in appropriate lookup table */
		if (keyboardUS[scancode] >= 'a' && keyboardUS[scancode] <= 'z'){
			capitilize = shiftStatus ^ capsStatus; //if shift and caps pushed, don't capitilize, only capitilize if XOR
			if (capitilize)
				returnChar = keyboardUSShift[scancode];
			else 
				returnChar = keyboardUS[scancode];
		}
		else{
			if (shiftStatus)
				returnChar = keyboardUSShift[scancode];
			else 
				returnChar = keyboardUS[scancode];
		}
    }
	
	/* backspace */
	if (scancode == BACKSPACE){
		if (keyboardBufferIdx[active_terminal] > 0){
			//delete from video screen
			backspace();
			keyboardBuffer[active_terminal][keyboardBufferIdx[active_terminal]] = NULL;
			keyboardBufferIdx[active_terminal] = keyboardBufferIdx[active_terminal] - 1;
		}
	}

	/* tab */
	if (scancode == TAB){
		tab(active_terminal);
		returnChar = 0;
	}
	
	/* CTRL + L clear screen */
	if (ctrlStatus && (returnChar == 'l' || returnChar == 'L')){
		clear(active_terminal);
		returnChar = 0;
	}

	//add in some handlers for terminal switching
	if (altStatus && F1Status){
		active_terminal = 0;
		returnChar = 0;
		//printf("currentTerminal = 0");
	}
	else if (altStatus && F2Status){
		active_terminal = 1;
		returnChar = 0;
	}
	else if (altStatus && F3Status){
		active_terminal = 2;
		returnChar = 0;
	}

	
	if (returnChar != 0 && scancode != ESC && ctrlStatus == 0){
		/* have to fill the buffer */
		if (keyboardBufferIdx[active_terminal] < BUFFER_SIZE){
			keyboardBuffer[active_terminal][keyboardBufferIdx[active_terminal]++] = returnChar;
		}
		
		/* print to screen */
		putc(returnChar, active_terminal);
	}
	
	send_eoi(1); 
	enable_irq(1); //restore keyboard irq line
    sti();
}



/*******************************************************
* keyboard_read
* Description: Copies keyboard data to buffer. 
* Inputs: buffer & nbytes (max length of buffer)
* Outputs: i (iteration of read)
* Side Effects: Clears keyboard buffer.
********************************************************/
//read nbytes from keyboard to buf
int32_t keyboard_read(int32_t fd, uint8_t* buf, int32_t nbytes)
{

		while(!enterStatus || pcb_terminal != active_terminal)
		{
			cli();
			if (nbytes < 0 || buf == NULL )
					return -1;
			if (!nbytes)
				return 0;
			sti();
		}
				enterStatus = 0;
		int i;
		for (i = 0; i < keyboardBufferIdx[active_terminal] && i < nbytes; i++){
		buf[i] = keyboardBuffer[active_terminal][i];
		}
		clear_keyboardBuffer(active_terminal);

	return i;	
}

/*
* keyboard_open
* Description: keybaord writing is not available. 
* Inputs: none
* Outputs: none
* Side Effects: none
*/
int32_t keyboard_write(int32_t fd, const uint8_t* buf, int32_t nbytes)
{
	return -1;
}

/*
* keyboard_open
* Description: keybaord opening is not available. 
* Inputs: none
* Outputs: none
* Side Effects: none
*/
int32_t keyboard_open(const uint8_t* filename)
{
	return -1;
}

/*
* keyboard_close
* Description: keybaord closing is not available. 
* Inputs: none
* Outputs: none
* Side Effects: none
*/

int32_t keyboard_close(int32_t fd)
{
	return -1;
}

/*******************************************************
* clear_keyboardBuffer
* Description: Clears keyboard buffer and resets index.
* Inputs: none
* Outputs: none
* Side Effects: Resets keyboard buffer. 
********************************************************/
//clear the buffer, reset index
void clear_keyboardBuffer(uint32_t terminal){
	keyboardBufferIdx[terminal] = 0;
	int i;
	for (i = 0; i < BUFFER_SIZE; i++){
		keyboardBuffer[terminal][i] = NULL;
	}
}
