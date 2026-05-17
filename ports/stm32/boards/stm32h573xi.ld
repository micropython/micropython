/*
    GNU linker script for STM32H573xI
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 1024K   /* sectors 0-127 (for common_basic.ld) */
    FLASH_APP (rx)  : ORIGIN = 0x08008000, LENGTH = 992K    /* sectors 4-127 (for common_bl.ld) */
    FLASH_FS (rx)   : ORIGIN = 0x08100000, LENGTH = 1024K   /* sectors 128-255 */
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 632K    /* SRAM1+SRAM2+SRAM3 - 8K */
    FS_CACHE (xrw)  : ORIGIN = 0x2009e000, LENGTH = 8K      /* last 8K of SRAM3 */
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

/* Filesystem cache in RAM, and storage in flash */
_micropy_hw_internal_flash_storage_ram_cache_start = ORIGIN(FS_CACHE);
_micropy_hw_internal_flash_storage_ram_cache_end = ORIGIN(FS_CACHE) + LENGTH(FS_CACHE);
_micropy_hw_internal_flash_storage_start = ORIGIN(FLASH_FS);
_micropy_hw_internal_flash_storage_end = ORIGIN(FLASH_FS) + LENGTH(FLASH_FS);
