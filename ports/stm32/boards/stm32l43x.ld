/*
    GNU linker script for STM32L431xC STM32L432KC STM32L433xC STM32L442KC STM32L443xC.
    FLASH has 2K blocks, RAM 64K.
    The MCU flash and file system size are defined for each board in mpconfigboard.mk.
*/

/* Specify the memory areas. */
MEMORY
{
    MCU_FLASH (rx)  : ORIGIN = 0x08000000,  LENGTH = MICROPY_HW_MCU_FLASH_SIZE
    FLASH (rx)      : ORIGIN = ORIGIN(MCU_FLASH), LENGTH = LENGTH(MCU_FLASH) - MICROPY_HW_FS_SIZE
    FLASH_FS (r)    : ORIGIN = ORIGIN(FLASH) + LENGTH(FLASH), LENGTH = MICROPY_HW_FS_SIZE
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 64K /* SRAM1, 48K + SRAM2, 16K */
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 2K;
_minimum_heap_size = 16K;

/* Define the stack.  The stack is full descending so begins just above last byte of RAM,
   or bottom of FS cache..  Note that EABI requires the stack to be 8-byte aligned for a call. */

/* RAM extents for the garbage collector */
_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);

_micropy_hw_internal_flash_storage_ram_cache_end = _ram_end;
_micropy_hw_internal_flash_storage_ram_cache_start = _micropy_hw_internal_flash_storage_ram_cache_end - 2K; /* fs cache = 2K */

_estack = _micropy_hw_internal_flash_storage_ram_cache_start - _estack_reserve;
_sstack = _estack - 10K; /* stack = 10K */

_heap_start = _ebss; /* heap starts just after statically allocated memory */
_heap_end = _sstack; /* bss + heap = 52K, tunable by adjusting stack size */

_micropy_hw_internal_flash_storage_start = ORIGIN(FLASH_FS);
_micropy_hw_internal_flash_storage_end   = ORIGIN(FLASH_FS) + LENGTH(FLASH_FS);
