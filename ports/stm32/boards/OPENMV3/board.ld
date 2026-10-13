/*
    GNU linker script for the OpenMV Cam M7 (STM32F765).

    The first 128K of flash hold the OpenMV bootloader and the filesystem; the
    bootloader starts the firmware at 0x08020000.
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)       : ORIGIN = 0x08000000, LENGTH = 2048K
    FLASH_BL (rx)    : ORIGIN = 0x08000000, LENGTH = 32K     /* OpenMV bootloader, sector 0 */
    FLASH_FS (r)     : ORIGIN = 0x08008000, LENGTH = 96K     /* Filesystem, sectors 1-3 */
    FLASH_TEXT (rx)  : ORIGIN = 0x08020000, LENGTH = 1664K   /* Firmware, sectors 4-10 */
    FLASH_ROMFS (rx) : ORIGIN = 0x081C0000, LENGTH = 256K    /* ROMFS partition, sector 11 */
    DTCM (xrw)       : ORIGIN = 0x20000000, LENGTH = 128K    /* Filesystem cache */
    RAM (xrw)        : ORIGIN = 0x20020000, LENGTH = 384K    /* SRAM1 = 368K, SRAM2 = 16K */
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

/* Location of filesystem RAM cache */
_micropy_hw_internal_flash_storage_ram_cache_start = ORIGIN(DTCM);
_micropy_hw_internal_flash_storage_ram_cache_end = ORIGIN(DTCM) + LENGTH(DTCM);

/* Location of filesystem flash storage */
_micropy_hw_internal_flash_storage_start = ORIGIN(FLASH_FS);
_micropy_hw_internal_flash_storage_end = ORIGIN(FLASH_FS) + LENGTH(FLASH_FS);

/* Location of romfs filesystem */
_micropy_hw_romfs_part0_start = ORIGIN(FLASH_ROMFS);
_micropy_hw_romfs_part0_size = LENGTH(FLASH_ROMFS);

INCLUDE common_blifs.ld
