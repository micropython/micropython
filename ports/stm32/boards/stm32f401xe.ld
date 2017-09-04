/*
    GNU linker script for STM32F401xE
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 0x080000 /* entire flash, 512 KiB */
    FLASH_ISR (rx)  : ORIGIN = 0x08000000, LENGTH = 0x004000 /* sector 0, 16 KiB */
    FLASH_FS (rx)   : ORIGIN = 0x08004000, LENGTH = 0x01C000 /* sectors 1,2,3 are 16K, 4 is 64K */
    FLASH_TEXT (rx) : ORIGIN = 0x08020000, LENGTH = 0x060000 /* sectors 5,6,7 3*128KiB = 384 KiB */
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 0x018000 /* 96 KiB */
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 2K;
_minimum_heap_size = 16K;

/* Define tho top end of the stack.  The stack is full descending so begins just
   above last byte of RAM.  Note that EABI requires the stack to be 8-byte
   aligned for a call. */
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* define common sections and symbols */
INCLUDE common.ld

/* RAM extents for the garbage collector */
_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
_heap_start = _ebss; /* heap starts just after statically allocated memory */
_heap_end = 0x20014000; /* tunable */
