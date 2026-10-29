/* STM32C5A3CG: 1 MiB flash, 256 KiB SRAM, factory 32 KiB TinyUF2 bootloader. */
MEMORY
{
    FLASH (rx)     : ORIGIN = 0x08008000, LENGTH = 736K
    FLASH_FS (rx)  : ORIGIN = 0x080C0000, LENGTH = 256K
    RAM (xrw)      : ORIGIN = 0x20000000, LENGTH = 248K
    FS_CACHE (xrw) : ORIGIN = 0x2003E000, LENGTH = 8K
}

_minimum_stack_size = 2K;
_minimum_heap_size = 16K;

_estack = ORIGIN(RAM) + LENGTH(RAM) - _estack_reserve;
_sstack = _estack - 16K;
_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
_heap_start = _ebss;
_heap_end = _sstack;

_micropy_hw_internal_flash_storage_ram_cache_start = ORIGIN(FS_CACHE);
_micropy_hw_internal_flash_storage_ram_cache_end = ORIGIN(FS_CACHE) + LENGTH(FS_CACHE);
_micropy_hw_internal_flash_storage_start = ORIGIN(FLASH_FS);
_micropy_hw_internal_flash_storage_end = ORIGIN(FLASH_FS) + LENGTH(FLASH_FS);

__StackTop = _estack;
