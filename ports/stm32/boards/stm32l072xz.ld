/*
    GNU linker script for STM32F072xZ
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 192K
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 20K
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 2K;
_minimum_heap_size = 8K;

_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);

/* Define the top end of the stack.  The stack is full descending so begins just
   above last byte of RAM.  Note that EABI requires the stack to be 8-byte
   aligned for a call. */
_estack = ORIGIN(RAM) + LENGTH(RAM) - _estack_reserve;
_sstack = _estack - 4K;

/* RAM extents for the main heap */
_heap_start = _ebss;
_heap_end = _sstack;
