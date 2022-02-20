/*
    GNU linker script for STM32WB55xG
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 512K /* sectors 0-127 */
    FLASH_APP (rx)  : ORIGIN = 0x08004000, LENGTH = 496K /* sectors 4-127 */
    FLASH_FS (r)    : ORIGIN = 0x08080000, LENGTH = 256K /* sectors 128-191 */
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 192K /* SRAM1 */
    RAM2A (xrw)     : ORIGIN = 0x20030000, LENGTH = 10K  /* SRAM2A */
    RAM2B (xrw)     : ORIGIN = 0x20038000, LENGTH = 10K  /* SRAM2B */
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 2K;
_minimum_heap_size = 16K;

/* RAM extents for the garbage collector */
_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);

_micropy_hw_internal_flash_storage_ram_cache_end = ORIGIN(RAM) + LENGTH(RAM);
_micropy_hw_internal_flash_storage_ram_cache_start = _micropy_hw_internal_flash_storage_ram_cache_end - 4K; /* fs cache = 4K */

/* Define the stack.  The stack is full descending so begins at the bottom of FS cache.
   Note that EABI requires the stack to be 8-byte aligned for a call. */
_estack = _micropy_hw_internal_flash_storage_ram_cache_start - _estack_reserve;
_sstack = _estack - 16K;

_heap_start = _ebss; /* heap starts just after statically allocated memory */
_heap_end = _sstack;

_micropy_hw_internal_flash_storage_start = ORIGIN(FLASH_FS);
_micropy_hw_internal_flash_storage_end   = ORIGIN(FLASH_FS) + LENGTH(FLASH_FS);

SECTIONS
{
    /* Put all IPCC tables into SRAM2A. */
    .ram2a_bss :
    {
        . = ALIGN(4);
        . = . + 64; /* Leave room for the mb_ref_table_t (assuming IPCCDBA==0). */
        *rfcore.o(.bss.ipcc_mem_*)
        . = ALIGN(4);
    } >RAM2A

    /* Put all IPCC buffers into SRAM2B. */
    .ram2b_bss :
    {
        . = ALIGN(4);
        *rfcore.o(.bss.ipcc_membuf_*)
        . = ALIGN(4);
    } >RAM2B
}
