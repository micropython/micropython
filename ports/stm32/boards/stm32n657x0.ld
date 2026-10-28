/*
    GNU linker script for STM32N657x0

    Upon reset the upper 512k of SRAM2 (AXISRAM2) is filled from external flash by
    the ROM bootloader.  When the application firmware runs from external flash, that
    512k contains mboot, which isn't needed once the application runs.  Otherwise it
    contains the application firmware itself (FLASH).

    IRAM is in ITCM, which is retained in standby.  RAM is FLEXRAM, AXISRAM1 and
    AXISRAM2, which are contiguous.  It includes the upper 512k of AXISRAM2 when the
    firmware runs from external flash, and stops below it otherwise.  Boards can
    optionally add the NPU's RAM (AXISRAM3-6 and its cache) to the heap with a
    _gc_blocks_table entry in a linker script.
*/

/* Specify the memory areas */
MEMORY
{
    ITCM_S (xrw)        : ORIGIN = 0x10000000, LENGTH = 64K
    SRAM_S_RAM (xrw)    : ORIGIN = 0x34000000, LENGTH = _micropy_hw_runs_from_ext_flash ? 2048K : 1536K
    SRAM2_S_FSBL (xrw)  : ORIGIN = 0x34180400, LENGTH = 511K /* firmware loaded from SPI flash upon reset */
    NPU_RAM_S (xrw)     : ORIGIN = 0x34200000, LENGTH = 2048K
    EXT_FLASH (rx)      : ORIGIN = 0x70080000, LENGTH = 1536K
}

REGION_ALIAS("IRAM", ITCM_S);
REGION_ALIAS("RAM", SRAM_S_RAM);
REGION_ALIAS("FLASH", SRAM2_S_FSBL);
REGION_ALIAS("NPU_RAM", NPU_RAM_S);
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
