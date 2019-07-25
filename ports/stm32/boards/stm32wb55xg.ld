/*
    GNU linker script for STM32WB55xG
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 512K /* sectors 0-127 */
    FLASH_FS (r)    : ORIGIN = 0x08080000, LENGTH = 256K /* sectors 128-191 */
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 192K /* SRAM1 */
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 2K;
_minimum_heap_size = 16K;

/* RAM extents for the garbage collector */
_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);

_ram_fs_cache_end = ORIGIN(RAM) + LENGTH(RAM);
_ram_fs_cache_start = _ram_fs_cache_end - 4K; /* fs cache = 4K */

/* Define the stack.  The stack is full descending so begins at the bottom of FS cache.
   Note that EABI requires the stack to be 8-byte aligned for a call. */
_estack = _ram_fs_cache_start - _estack_reserve;
_sstack = _estack - 16K;

_heap_start = _ebss; /* heap starts just after statically allocated memory */
_heap_end = _sstack;

_flash_fs_start = ORIGIN(FLASH_FS);
_flash_fs_end   = ORIGIN(FLASH_FS) + LENGTH(FLASH_FS);
