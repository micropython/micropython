/*
    GNU linker script for STM32L476XG
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 512K /* sectors 0-255 */
    FLASH_FS (r)    : ORIGIN = 0x08080000, LENGTH = 512K /* sectors 256-511 */
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 96K
    SRAM2 (xrw)     : ORIGIN = 0x10000000, LENGTH = 32K /* not contiguous with RAM */
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 2K;
_minimum_heap_size = 16K;

/* Define the stack.  The stack is full descending so begins just above last byte of RAM,
   or bottom of FS cache..  Note that EABI requires the stack to be 8-byte aligned for a call. */

/* RAM extents for the garbage collector */
_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);

_ram_fs_cache_end = ORIGIN(SRAM2) + LENGTH(SRAM2); /* fs_cache in SRAM2 */
_ram_fs_cache_start = _ram_fs_cache_end - 2K; /* fs cache = 2K */

_estack = _ram_fs_cache_start - _estack_reserve; /* stack in SRAM2 */
_sstack = ORIGIN(SRAM2); /* stack = 30K */

_heap_start = _ebss; /* heap starts just after statically allocated memory */
_heap_end = _ram_end; /* bss + heap = 96K, tunable by adjusting stack size */

_flash_fs_start = ORIGIN(FLASH_FS);
_flash_fs_end   = ORIGIN(FLASH_FS) + LENGTH(FLASH_FS);
