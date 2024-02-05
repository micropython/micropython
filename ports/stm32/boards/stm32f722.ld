/*
    GNU linker script for STM32F722, STM32F723, STM32F732, STM32F733
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 512K
    FLASH_ISR (rx)  : ORIGIN = 0x08000000, LENGTH = 16K     /* sector 0, 16K */
    FLASH_FS (r)    : ORIGIN = 0x08004000, LENGTH = 112K    /* sectors 1-4 3*16KiB 1*64KiB*/
    FLASH_TEXT (rx) : ORIGIN = 0x08020000, LENGTH = 384K    /* sectors 5-7 3*128KiB = 384K */
    DTCM (xrw)      : ORIGIN = 0x20000000, LENGTH = 64K     /* Used for storage cache */
    RAM (xrw)       : ORIGIN = 0x20010000, LENGTH = 192K    /* SRAM1 = 176K, SRAM2 = 16K */
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 2K;
_minimum_heap_size = 16K;

/* Define the stack.  The stack is full descending so begins just above last byte
   of RAM.  Note that EABI requires the stack to be 8-byte aligned for a call. */
_estack = ORIGIN(RAM) + LENGTH(RAM) - _estack_reserve;
_sstack = _estack - 32K; /* tunable */

/* RAM extents for the garbage collector */
_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
_heap_start = _ebss; /* heap starts just after statically allocated memory */
_heap_end = _sstack;

/* Filesystem cache in RAM, and storage in flash */
_micropy_hw_internal_flash_storage_ram_cache_start = ORIGIN(DTCM);
_micropy_hw_internal_flash_storage_ram_cache_end = ORIGIN(DTCM) + LENGTH(DTCM);
_micropy_hw_internal_flash_storage_start = ORIGIN(FLASH_FS);
_micropy_hw_internal_flash_storage_end = ORIGIN(FLASH_FS) + LENGTH(FLASH_FS);
