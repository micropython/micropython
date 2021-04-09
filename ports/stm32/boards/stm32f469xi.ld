/*
    GNU linker script for STM32F469xI (2Mbyte)
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 2048K   /* Entire flash */
    FLASH_ISR (rx)  : ORIGIN = 0x08000000, LENGTH = 16K     /* Sector 0 */
    FLASH_RSV (r)   : ORIGIN = 0x08004000, LENGTH = 16K     /* Sector 1 (reserved) */
    FLASH_FS (r)    : ORIGIN = 0x08008000, LENGTH = 96K     /* Sectors 2, 3, 4 */
    FLASH_TEXT (rx) : ORIGIN = 0x08020000, LENGTH = 1920K   /* Sectors 5 - 23 */
    DTCM (rw)       : ORIGIN = 0x10000000, LENGTH = 64K     /* CCM RAM used for storage cache */
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 320K    /* SRAM1, SRAM2, SRAM3 */
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 2K;
_minimum_heap_size = 16K;

/* Define the stack.  The stack is full descending so begins just above last byte
   of RAM.  Note that EABI requires the stack to be 8-byte aligned for a call. */
_estack = ORIGIN(RAM) + LENGTH(RAM) - _estack_reserve;
_sstack = _estack - 16K; /* tunable */

/* RAM extents for the garbage collector */
_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
_heap_start = _ebss; /* heap starts just after statically allocated memory */
_heap_end = _sstack;
