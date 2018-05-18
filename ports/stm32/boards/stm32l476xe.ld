/*
    GNU linker script for STM32L476XE
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 512K
    FLASH_ISR (rx)  : ORIGIN = 0x08000000, LENGTH = 16K /* sectors 0-7 */
    FLASH_TEXT (rx) : ORIGIN = 0x08004000, LENGTH = 368K /* sectors 8-191 */
    FLASH_FS (r)    : ORIGIN = 0x08060000, LENGTH = 128K /* sectors 192-255 */
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 96K
    SRAM2 (xrw)     : ORIGIN = 0x10000000, LENGTH = 32K
    FS_CACHE(xrw)   : ORIGIN = 0x10007800, LENGTH = 2K
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 2K;
_minimum_heap_size = 16K;

/* Define the top end of the stack.  The stack is full descending so begins just
   above last byte of RAM.  Note that EABI requires the stack to be 8-byte
   aligned for a call. */
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* RAM extents for the garbage collector */
_ram_fs_cache_start = ORIGIN(FS_CACHE);
_ram_fs_cache_block_size = LENGTH(FS_CACHE);
_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
_heap_start = _ebss; /* heap starts just after statically allocated memory */
_heap_end = 0x20014000; /* tunable */

_flash_fs_start = ORIGIN(FLASH_FS);
_flash_fs_end   = ORIGIN(FLASH_FS) + LENGTH(FLASH_FS);
