/*
    GNU linker script for STM32L496XG
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 1024K
    FLASH_ISR (rx)  : ORIGIN = 0x08000000, LENGTH = 16K       /* sectors 0-7 */
    FLASH_TEXT (rx) : ORIGIN = 0x08004000, LENGTH = 596K      /* sectors 8-305 */
    FLASH_FS (r)    : ORIGIN = 0x08099000, LENGTH = 412K      /* sectors 306-511 412 KiB */
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 256K
    SRAM2 (xrw)     : ORIGIN = 0x20040000, LENGTH = 62K       /* leave 2K for flash fs cache */
    FS_CACHE(xrw)   : ORIGIN = 0x2004f800, LENGTH = 2K
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 2K;
_minimum_heap_size = 16K;

/* Define the stack.  The stack is full descending so begins just above last byte
   of RAM.  Note that EABI requires the stack to be 8-byte aligned for a call. */
_estack = ORIGIN(RAM) + LENGTH(RAM) + LENGTH(SRAM2);
_sstack = _estack - 206K; /* tunable */

/* RAM extents for the garbage collector */
_ram_fs_cache_start = ORIGIN(FS_CACHE);
_ram_fs_cache_end = ORIGIN(FS_CACHE) + LENGTH(FS_CACHE);
_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM) + LENGTH(SRAM2);
_heap_start = _ebss; /* heap starts just after statically allocated memory */
_heap_end = _sstack;

_flash_fs_start = ORIGIN(FLASH_FS);
_flash_fs_end   = ORIGIN(FLASH_FS) + LENGTH(FLASH_FS);
