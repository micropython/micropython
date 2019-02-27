/*
    GNU linker script for STM32F429i-Discovery kit with external 8MByte SDRam
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 2048K /* entire flash */
    FLASH_ISR (rx)  : ORIGIN = 0x08000000, LENGTH = 16K /* sector 0, 16 KiB */
    FLASH_FS (rx)   : ORIGIN = 0x08004000, LENGTH = 112K /* sectors 1-4: 3*16K+64K */
    FLASH_TEXT (rx) : ORIGIN = 0x08020000, LENGTH = 896K /* sectors 5-11 are 128K */
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 192K
    SDRAM(xrw)      : ORIGIN = 0xC0000000, LENGTH = 8192K
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 2K;
_minimum_heap_size = 16K;

/* Define tho top end of the stack.  The stack is full descending so begins just
   above last byte of RAM.  Note that EABI requires the stack to be 8-byte
   aligned for a call. */
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* RAM extents for the garbage collector */
_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
_heap_start = _ebss; /* heap starts just after statically allocated memory */
_heap_end = 0x2002c000; /* tunable */
