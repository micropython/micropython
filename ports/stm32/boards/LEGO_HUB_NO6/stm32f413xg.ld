/*
    GNU linker script for STM32F413xg (1MB flash, 320kB RAM)
*/

/* Specify the memory areas */
/* FLASH_FS2 is placed before FLASH_TEXT to support 1MB and 1.5MB FLASH with common code in flashbdev.c */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 1024K /* entire flash */
    FLASH_APP (rx)  : ORIGIN = 0x08010000, LENGTH = 976K
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 320K /* SRAM1 + SRAM2 */
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 2K;
_minimum_heap_size = 16K;

/* Define the stack.  The stack is full descending so begins just above last byte
   of RAM.  Note that EABI requires the stack to be 8-byte aligned for a call. */
_estack = ORIGIN(RAM) + LENGTH(RAM) - _estack_reserve;
_sstack = _estack - 12K; /* tunable */

/* RAM extents for the garbage collector */
_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
_heap_start = _ebss; /* heap starts just after statically allocated memory */
_heap_end = _sstack;
