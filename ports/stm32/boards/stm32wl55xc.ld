/*
    GNU linker script for STM32WL55xC
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 256K /* sectors 0-127 */
    FLASH_APP (rx)  : ORIGIN = 0x08000000, LENGTH = 232K /* sectors 0-115 */
    FLASH_FS (r)    : ORIGIN = 0x0803a000, LENGTH = 24K  /* sectors 116-127 */
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 64K  /* SRAM1+SRAM2 */
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 2K;
_minimum_heap_size = 16K;

/* RAM extents for the garbage collector */
_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);

_micropy_hw_internal_flash_storage_ram_cache_end = ORIGIN(RAM) + LENGTH(RAM);
_micropy_hw_internal_flash_storage_ram_cache_start = _micropy_hw_internal_flash_storage_ram_cache_end - 2K; /* fs cache = 2K */

/* Define the stack.  The stack is full descending so begins at the bottom of FS cache.
   Note that EABI requires the stack to be 8-byte aligned for a call. */
_estack = _micropy_hw_internal_flash_storage_ram_cache_start - _estack_reserve;
_sstack = _estack - 6K;

_heap_start = _ebss; /* heap starts just after statically allocated memory */
_heap_end = _sstack;

_micropy_hw_internal_flash_storage_start = ORIGIN(FLASH_FS);
_micropy_hw_internal_flash_storage_end   = ORIGIN(FLASH_FS) + LENGTH(FLASH_FS);
