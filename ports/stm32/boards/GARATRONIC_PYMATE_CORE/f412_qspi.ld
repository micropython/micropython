/*
    Linker script for PYBSTICK26-PRO with STM32F412 + QSPI flash

    Memory layout for mboot configuration (this here describes the app part):

    FLASH_APP   .isr_vector
    FLASH_APP   .text
    FLASH_APP   .data

    RAM         .data
    RAM         .bss
    RAM         .heap
    RAM         .stack
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 512K    /* sectors 0-1 */
    FLASH_APP (rx)  : ORIGIN = 0x08000000, LENGTH = 512K    /* sectors 2-7 */
    FLASH_QSPI (rx) : ORIGIN = 0x90000000, LENGTH = 1024K   /* external QSPI */
    RAM (rwx)       : ORIGIN = 0x20000000, LENGTH = 256K    /* SRAM */
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 2K;
_minimum_heap_size = 16K;

/* Define the stack.  The stack is full descending so begins just above last byte
   of RAM.  Note that EABI requires the stack to be 8-byte aligned for a call. */
_estack = ORIGIN(RAM) + LENGTH(RAM) - _estack_reserve;
_sstack = _estack - 16K;

/* RAM extents for the garbage collector */
_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
_heap_start = _ebss; /* heap starts just after statically allocated memory */
_heap_end = _sstack;

/* ROMFS location */
_micropy_hw_romfs_part0_start = ORIGIN(FLASH_QSPI);
_micropy_hw_romfs_part0_size = LENGTH(FLASH_QSPI);

_micropy_hw_internal_flash_storage_start  = ORIGIN (FLASH_QSPI);
_micropy_hw_internal_flash_storage_end = ORIGIN(FLASH_QSPI) + LENGTH(FLASH_QSPI);
