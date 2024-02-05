/* Specify the memory areas */
MEMORY
{
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 128K
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 352K
    FLASH_FS (rx)   : ORIGIN = 0x08058000, LENGTH = 160K  /* starting @ 352K */
}

/* Generate a link error if heap and stack don't fit into RAM */
_minimum_heap_size = 0x200; /* required amount of heap  */
_minimum_stack_size = 0x400; /* required amount of stack */

_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);

_micropy_hw_internal_flash_storage_ram_cache_end = ORIGIN(RAM) + LENGTH(RAM);
_micropy_hw_internal_flash_storage_ram_cache_start = _micropy_hw_internal_flash_storage_ram_cache_end - 2K; /* fs cache = 2K RAM */

/* Define the stack.  The stack is full descending so begins just above last byte
   of RAM.  Note that EABI requires the stack to be 8-byte aligned for a call. */
_estack = _micropy_hw_internal_flash_storage_ram_cache_start - _estack_reserve;
_sstack = _estack - 8K; /* tunable */

_heap_start = _ebss; /* heap starts just after statically allocated memory */
_heap_end = _sstack;

_micropy_hw_internal_flash_storage_start = ORIGIN(FLASH_FS);
_micropy_hw_internal_flash_storage_end   = ORIGIN(FLASH_FS) + LENGTH(FLASH_FS);
