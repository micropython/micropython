/*
    GNU linker script for STM32F405, no filesystem
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)          : ORIGIN = 0x08000000, LENGTH = 1024K /* entire flash */
    FLASH_ISR (rx)      : ORIGIN = 0x08000000, LENGTH = 16K /* sector 0 */
    FLASH_FIRMWARE (rx) : ORIGIN = 0x08004000, LENGTH = 1008K /* sectors 0-7*/
    CCMRAM (xrw)        : ORIGIN = 0x10000000, LENGTH = 64K
    RAM (xrw)           : ORIGIN = 0x20000000, LENGTH = 128K
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 24K;
_minimum_heap_size = 16K;

/* Define tho top end of the stack.  The stack is full descending so begins just
   above last byte of RAM.  Note that EABI requires the stack to be 8-byte
   aligned for a call. */
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* RAM extents for the garbage collector */
_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
