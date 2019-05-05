/*
    GNU linker script for STM32F413xh (1.5MB flash, 320kB RAM)
*/

/* Specify the memory areas */
/* FLASH_FS2 is placed before FLASH_TEXT to support 1MB and 1.5MB FLASH with common code in flashbdev.c */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 1536K /* entire flash */
    FLASH_ISR (rx)  : ORIGIN = 0x08000000, LENGTH = 16K /* sector 0 */
    FLASH_FS (rx)   : ORIGIN = 0x08004000, LENGTH = 176K /* sectors 1,2,3 are 16K, 4 is 64K, 5 is 128K (64K used) for filesystem */
    FLASH_FS2 (rx)  : ORIGIN = 0x08040000, LENGTH = 128K /* sector 6 is 128K (64K used) for filesystem, Total filesystem 240K */
    FLASH_TEXT (rx) : ORIGIN = 0x08060000, LENGTH = 1152K /* sectors 7,8,9,10,11,12,13,14,15  are 128K*/
    SRAM2 (xrw)     : ORIGIN = 0x10000000, LENGTH = 64K
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 256K
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
_heap_end = _ram_end - 16K; /* 240K, tunable */
