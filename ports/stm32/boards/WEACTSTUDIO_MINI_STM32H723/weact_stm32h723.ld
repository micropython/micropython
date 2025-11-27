/*
    GNU linker script for WeAct Studio STM32H723
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 1024K    /* sectors 0-7 */
    FLASH_APP  (rx) : ORIGIN = 0x08020000, LENGTH = 896K    /* sectors 1-7 */
    FLASH_ROMFS (rx): ORIGIN = 0x90000000, LENGTH = 8192K   /* external QSPI */
    DTCM (xrw)      : ORIGIN = 0x20000000, LENGTH = 128K    /* Used for FS storage cache */
    RAM (xrw)       : ORIGIN = 0x24000000, LENGTH = 320K    /* AXI SRAM (could extend +192K from ITCM) */
    RAM_SRAM1 (xrw) : ORIGIN = 0x30000000, LENGTH = 16K     /* SRAM1 */
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 2K;
_minimum_heap_size = 16K;

/* Define the stack. The stack is full descending so begins just above last byte
   of RAM. Note that EABI requires the stack to be 8-byte aligned for a call. */
_estack = ORIGIN(RAM) + LENGTH(RAM) - _estack_reserve;
_sstack = _estack - 16K; /* tunable */

/* RAM extents for the garbage collector */
_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
_heap_start = _ebss; /* heap starts just after statically allocated memory */
_heap_end = _sstack;

/* ROMFS location */
_micropy_hw_romfs_part0_start = ORIGIN(FLASH_ROMFS);
_micropy_hw_romfs_part0_size = LENGTH(FLASH_ROMFS);
