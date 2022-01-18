/*
    GNU linker script for STM32F439
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 2048K /* entire flash */
    FLASH_ISR (rx)  : ORIGIN = 0x08000000, LENGTH = 16K /* sector 0 */
    FLASH_TEXT (rx) : ORIGIN = 0x08020000, LENGTH = 896K /* sectors 5-11 are 128K */
    FLASH_FS (rx)   : ORIGIN = 0x08100000, LENGTH = 192K /* sectors 12-15 are 16K, 16 is 64K, 17 is 128K (64K used) */
    FLASH_FS2 (rx)  : ORIGIN = 0x08140000, LENGTH = 64K /* sector 18 is 128K (64K used) */
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 192K
    CCMRAM (xrw)    : ORIGIN = 0x10000000, LENGTH = 64K
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
_micropy_hw_internal_flash_storage_ram_cache_start = ORIGIN(CCMRAM);
_micropy_hw_internal_flash_storage_ram_cache_end = ORIGIN(CCMRAM) + LENGTH(CCMRAM);
_micropy_hw_internal_flash_storage_start = ORIGIN(FLASH_FS);
_micropy_hw_internal_flash_storage_end = ORIGIN(FLASH_FS) + LENGTH(FLASH_FS);
_micropy_hw_internal_flash_storage2_start = ORIGIN(FLASH_FS2);
_micropy_hw_internal_flash_storage2_end = ORIGIN(FLASH_FS2) + LENGTH(FLASH_FS2);
