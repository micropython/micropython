/*
    GNU linker script for STM32F722, STM32F723, STM32F732, STM32F733
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 512K
    FLASH_ISR (rx)  : ORIGIN = 0x08000000, LENGTH = 32K     /* sectors 0,1 */
    FLASH_APP (rx)  : ORIGIN = 0x08008000, LENGTH = 480K    /* sectors 2-7 */
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 256K    /* DTCM+SRAM1+SRAM2 */
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
_heap_end = 0x20038000; /* tunable */
