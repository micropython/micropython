/*
    GNU linker script for STM32L476XE
*/

/* Specify the memory areas */
MEMORY
{                                                   
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 512K
    FLASH_ISR (rx)  : ORIGIN = 0x08000000, LENGTH = 0x0000800 /* sector 0, 2 KiB */
    FLASH_FS (r)    : ORIGIN = 0x08000800, LENGTH = 0x001F800 /* sectors 1-63 (2K each = 126 KiB) */
    FLASH_TEXT (rx) : ORIGIN = 0x08020000, LENGTH = 0x0060000 /* Sector starting @ 64 */
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 96K
    SRAM2 (xrw)     : ORIGIN = 0x10000000, LENGTH = 32K
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 2K;
_minimum_heap_size = 16K;
 
/* Define the top end of the stack.  The stack is full descending so begins just
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
