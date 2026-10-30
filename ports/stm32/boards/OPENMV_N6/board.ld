/*
    Linker script for OPENMV_N6.

    Note: upper 512k of SRAM2 is copied from external flash upon reset.
*/

/* Specify the memory areas */
MEMORY
{
    ITCM_S (xrw)            : ORIGIN = 0x10000000, LENGTH = 64K
    SRAM_S_RAM (xrw)        : ORIGIN = 0x34000000, LENGTH = 2048K /* FLEXRAM, AXISRAM1 and AXISRAM2 */
    EXT_FLASH (rx)          : ORIGIN = 0x70080000, LENGTH = 3584K
    EXT_FLASH_FS (rx)       : ORIGIN = 0x70400000, LENGTH = 4M
    EXT_FLASH_ROMFS (rx)    : ORIGIN = 0x70800000, LENGTH = 24M
}

REGION_ALIAS("IRAM", ITCM_S);
REGION_ALIAS("RAM", SRAM_S_RAM);
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

/* ROMFS location */
_micropy_hw_romfs_part0_start = ORIGIN(EXT_FLASH_ROMFS);
_micropy_hw_romfs_part0_size = LENGTH(EXT_FLASH_ROMFS);
