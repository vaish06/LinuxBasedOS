#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "idt.h"
#include "rtc.h"
#include "filesystem.h"
#include "system_calls.h"

#define PASS 1
#define FAIL 0

extern uint32_t active_terminal;

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}



// add more tests here

/* Checkpoint 2 tests */

/* rtc_test()
 * Description: Shows that the rtc functions, such as read, write, open works as they are expected.
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: rtc.c, rtc.h
 */
int rtc_test()
{
	TEST_HEADER;
	/*	A check for rtc_open for checking if the default frequency is set to 2 Hz */
	int result;
	result = rtc_open(NULL);
	int32_t default_frequency = DEFAULT_FREQUENCY;
	if (result == 0)
		printf(" Successfully opened RTC with %d Hz frequency.\n", default_frequency);

	/*	A check for rtc_write and rtc_4 read, changing frequency and printing digit to see change in frequencies */
	int i;
	/* A loop to see change from 2Hz to 1024 Hz, printing a digit for 20 times to see the 
	noticeable difference in frequencies change, and a bigger loop of 200 since 10 differenct frequency rate, (10*20=200).*/   
	for (i=0;i<200;i++)
    {
        if (i%20==0 && i){
            default_frequency = default_frequency << 1;
            printf("\nfrequency update: %d Hz\n", default_frequency);
        }
        rtc_write(NULL, &default_frequency, BYTES4);
        rtc_read(NULL, &default_frequency, BYTES4);
    	putc('1',active_terminal);
    }

    putc('\n',active_terminal);

    return PASS;

}

int filesystem_dir_test()
{
	TEST_HEADER;
	/*// extern file_desriptor_t kernel_file_desriptor[8];
	int32_t fd, i;
    int32_t frequency;
	uint8_t filename[] = ".";

		printf("Openening  %s",filename);

		frequency = 2;
		rtc_write(NULL, &frequency, 4);
		for(i=0;i<6;i++){
			rtc_read(NULL, NULL, 4);
			putc('.');
		}
		putc('\n');
	    

	    fd = open_fs(filename);

	    printf("Opened %s\n",filename);

	    if (fd != -1)
	    {
	    	read_fs_dir(fd, );
	    }

	    close_fs(fd);*/
	
    return PASS;
}

int filesystem_file_test()
{
	int32_t fd, i;
    int32_t frequency;
	uint8_t filename[] = "frame0.txt";
	int32_t length;

	printf("Openening  %s",filename);

		frequency = 2;
		rtc_write(NULL, &frequency, 4);
		for(i=0;i<6;i++){
			rtc_read(NULL, NULL, 4);
			putc('.', active_terminal);
		}
		putc('\n', active_terminal);
	    

    fd = open_fs(filename);


	length = get_filesize_by_name(filename);
	uint8_t fs_buf[length+1];
	read_fs(fd, fs_buf, 4);
	fs_buf[length]='\n';

	frequency = 1024;
	rtc_write(NULL, &frequency, 4);

	for (i=0;i<length+1;i++)
	{
		if(length < 500)
			rtc_read(NULL,NULL,4);
	    putc(fs_buf[i], active_terminal);
	}

    close_fs(fd);

 return PASS;
}






/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	clear(2);
	printf("\t\t\tWelcome to test mode\n");
	TEST_OUTPUT("idt_test", idt_test());
	TEST_OUTPUT("rtc_test", rtc_test());
	TEST_OUTPUT("filesystem_file_test", filesystem_file_test());
	TEST_OUTPUT("filesystem_dir_test", filesystem_dir_test());

	// launch your tests here
}
