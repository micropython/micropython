/*
    GNU linker script for STM32N657x0

    Note: upper 512k of SRAM2 is copied from external flash upon reset.
*/

/* Specify the memory areas */
MEMORY
{
    FLEXRAM_S (xrw)     : ORIGIN = 0x34000000, LENGTH = 80K
    SRAM2_S_RAM (xrw)   : ORIGIN = 0x34100000, LENGTH = 512K /* only use first half, second half may contain firmware */
    SRAM2_S_FSBL (xrw)  : ORIGIN = 0x34180400, LENGTH = 511K /* firmware loaded from SPI flash upon reset */
    EXT_FLASH (rx)      : ORIGIN = 0x70080000, LENGTH = 1536K
}

REGION_ALIAS("IRAM", FLEXRAM_S);
REGION_ALIAS("RAM", SRAM2_S_RAM);
REGION_ALIAS("FLASH", SRAM2_S_FSBL);
REGION_ALIAS("FLASH_APP", EXT_FLASH);

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
