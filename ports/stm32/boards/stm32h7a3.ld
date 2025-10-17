/*
    GNU linker script for STM32H7A3
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 2048K   /* sectors   0- 15,  128K */
    FLASH_APP  (rx) : ORIGIN = 0x08020000, LENGTH = 1664K   /* sectors  16-223, 1664K */
    FLASH_FS (r)    : ORIGIN = 0x081c0000, LENGTH =  256K   /* sectors 224-255,  256K */
    DTCM (xrw)      : ORIGIN = 0x20000000, LENGTH =  120K   /* Used for storage cache */
    FS_CACHE (xrw)  : ORIGIN = 0x2001e000, LENGTH =    8K   /* Used for storage cache */
    RAM (xrw)       : ORIGIN = 0x24000000, LENGTH = 1024K
    RAM_CD (xrw)    : ORIGIN = 0x30000000, LENGTH =  128K
    RAM_SRD (xrw)   : ORIGIN = 0x38000000, LENGTH =   32K
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
_micropy_hw_internal_flash_storage_ram_cache_start = ORIGIN(FS_CACHE);
_micropy_hw_internal_flash_storage_ram_cache_end = ORIGIN(FS_CACHE) + LENGTH(FS_CACHE);

/* Location of filesystem flash storage */
_micropy_hw_internal_flash_storage_start = ORIGIN(FLASH_FS);
_micropy_hw_internal_flash_storage_end = ORIGIN(FLASH_FS) + LENGTH(FLASH_FS);
