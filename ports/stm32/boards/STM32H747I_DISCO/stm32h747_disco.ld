/*
    GNU linker script for STM32H747I-DISCO
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
    SDRAM (xrw)     : ORIGIN = 0xD0000000, LENGTH = 32M     /* SDRAM bank 1 */
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 1536K   /* CM7 firmware */
    FLASH_CM4 (rx)  : ORIGIN = 0x08180000, LENGTH = 512K    /* CM4 firmware */
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 2K;
_minimum_heap_size = 16K;

/* Define the stack.  The stack is full descending so begins just above last byte
   of RAM.  Note that EABI requires the stack to be 8-byte aligned for a call. */
_estack = ORIGIN(RAM) + LENGTH(RAM) - _estack_reserve;
_sstack = _estack - 16K; /* tunable */

/* RAM heap extents */
_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
_heap_start = _ebss; /* heap starts just after statically allocated memory */
_heap_end = _sstack;

/* OpenAMP shared memory region */
_openamp_shm_region_start = ORIGIN(SRAM4);
_openamp_shm_region_end = ORIGIN(SRAM4) + LENGTH(SRAM4);

INCLUDE common_basic.ld

SECTIONS
{
    /* GC blocks addresses and sizes */
    .gc.blocks.table (READONLY) : {
      . = ALIGN(4);
      _gc_blocks_table_start = .;

      LONG (ORIGIN(SRAM1));
      LONG (128K);

      LONG (ORIGIN(SDRAM) + 0M);
      LONG (8M);

      LONG (ORIGIN(SDRAM) + 8M);
      LONG (8M);

      LONG (ORIGIN(SDRAM) + 16M);
      LONG (16M);

      _gc_blocks_table_end = .;
      . = ALIGN(4);
    } > FLASH
}
