/*
    GNU linker script for STM32H747
*/

/* Specify the memory areas */
MEMORY
{
    ITCM (xrw)      : ORIGIN = 0x00000000, LENGTH = 64K
    DTCM (xrw)      : ORIGIN = 0x20000000, LENGTH = 128K
    RAM (xrw)       : ORIGIN = 0x24000000, LENGTH = 512K    /* AXI SRAM D1 */
    SRAM1 (xrw)     : ORIGIN = 0x30000000, LENGTH = 128K    /* SRAM1 D2    */
    SRAM2 (xrw)     : ORIGIN = 0x30020000, LENGTH = 128K    /* SRAM2 D2    */
    SRAM3 (xrw)     : ORIGIN = 0x30040000, LENGTH = 32K     /* SRAM3 D2    */
    SRAM4 (xrw)     : ORIGIN = 0x38000000, LENGTH = 64K     /* SRAM4 D3    */
    SDRAM (xrw)     : ORIGIN = 0x60000000, LENGTH = 8M      /* SDRAM */
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 2048K   /* Total available flash */
    FLASH_BL (rx)   : ORIGIN = 0x08000000, LENGTH = 128K    /* Arduino bootloader */
    FLASH_FS (r)    : ORIGIN = 0x08020000, LENGTH = 128K    /* filesystem */
    FLASH_TEXT (rx) : ORIGIN = 0x08040000, LENGTH = 1280K   /* CM7 firmware */
    FLASH_CM4 (rx)  : ORIGIN = 0x08180000, LENGTH = 512K    /* CM4 firmware */
    FLASH_ROMFS (rx) : ORIGIN = 0x90B00000, LENGTH = 4096K   /* romfs partition in QSPI flash */
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

/* Note the following varilables are only used if the filesystem flash storage is enabled */
/* Location of filesystem RAM cache */
_micropy_hw_internal_flash_storage_ram_cache_start = ORIGIN(DTCM);
_micropy_hw_internal_flash_storage_ram_cache_end = ORIGIN(DTCM) + LENGTH(DTCM);

/* Location of filesystem flash storage */
_micropy_hw_internal_flash_storage_start = ORIGIN(FLASH_FS);
_micropy_hw_internal_flash_storage_end = ORIGIN(FLASH_FS) + LENGTH(FLASH_FS);

/* Location of romfs filesystem */
_micropy_hw_romfs_part0_start = ORIGIN(FLASH_ROMFS);
_micropy_hw_romfs_part0_size = LENGTH(FLASH_ROMFS);

/* OpenAMP shared memory region */
_openamp_shm_region_start = ORIGIN(SRAM4);
_openamp_shm_region_end = ORIGIN(SRAM4) + LENGTH(SRAM4);

INCLUDE common_blifs.ld

SECTIONS
{
    /* GC blocks addresses and sizes */
    .gc.blocks.table (READONLY) : {
      . = ALIGN(4);
      _gc_blocks_table_start = .;

      LONG (ORIGIN(SRAM1));
      LONG (128K);

      LONG (ORIGIN(SDRAM) + 0M);
      LONG (2M);

      LONG (ORIGIN(SDRAM) + 2M);
      LONG (2M);

      LONG (ORIGIN(SDRAM) + 4M);
      LONG (4M);

      _gc_blocks_table_end = .;
      . = ALIGN(4);
    } > FLASH_TEXT
}
