/*
    GNU linker script for STM32L476XG
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 1024K
    FLASH_ISR (rx)  : ORIGIN = 0x08000000, LENGTH = 0x0004000 /* sectors 0-7, 16 KiB */
    FLASH_TEXT (rx) : ORIGIN = 0x08004000, LENGTH = 0x007C000 /* sectors 8-255, 496 KiB */
    FLASH_FS (r)    : ORIGIN = 0x08080000, LENGTH = 0x0080000 /* sectors 256-511 512 KiB */
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 96K
    SRAM2 (xrw)     : ORIGIN = 0x10000000, LENGTH = 32K
}

ENTRY(Reset_Handler)

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

_flash_fs_start = ORIGIN(FLASH_FS);
_flash_fs_end   = ORIGIN(FLASH_FS) + LENGTH(FLASH_FS);
