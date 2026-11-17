/*
    GNU linker script for the OpenMV Pure Thermal (STM32H743).

    The first 256K of flash hold the OpenMV bootloader and its filesystem
    sectors; the bootloader starts the firmware at 0x08040000.  The filesystem
    and the ROMFS partition are in the QSPI flash.
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)       : ORIGIN = 0x08000000, LENGTH = 2048K   /* Total available flash */
    FLASH_BL (rx)    : ORIGIN = 0x08000000, LENGTH = 256K    /* OpenMV bootloader */
    FLASH_TEXT (rx)  : ORIGIN = 0x08040000, LENGTH = 1792K   /* Firmware */
    FLASH_ROMFS (rx) : ORIGIN = 0x91800000, LENGTH = 8M      /* ROMFS partition in QSPI flash */
    DTCM (xrw)       : ORIGIN = 0x20000000, LENGTH = 128K
    RAM (xrw)        : ORIGIN = 0x24000000, LENGTH = 512K    /* AXI SRAM */
    RAM_D2 (xrw)     : ORIGIN = 0x30000000, LENGTH = 288K    /* SRAM1-3 */
    SDRAM (xrw)      : ORIGIN = 0xC0000000, LENGTH = 64M
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

/* Location of romfs filesystem */
_micropy_hw_romfs_part0_start = ORIGIN(FLASH_ROMFS);
_micropy_hw_romfs_part0_size = LENGTH(FLASH_ROMFS);

INCLUDE common_blifs.ld

SECTIONS
{
    /* Extra GC blocks: SRAM1-3 and the SDRAM. */
    .gc.blocks.table (READONLY) : {
        . = ALIGN(4);
        _gc_blocks_table_start = .;
        LONG (ORIGIN(RAM_D2));
        LONG (LENGTH(RAM_D2));
        LONG (ORIGIN(SDRAM));
        LONG (LENGTH(SDRAM));
        _gc_blocks_table_end = .;
        . = ALIGN(4);
    } > FLASH_TEXT
}
