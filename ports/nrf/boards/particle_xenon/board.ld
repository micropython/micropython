/*
    GNU linker script for NRF52840, Particle bootloader layout
*/

_flash_size = 1M;
_ram_size   = 252K;

/* produce a link error if there is not this amount of RAM for these sections */
_stack_size = 8K;
_minimum_heap_size = 128K;


/* Flash layout: bootloader | softdevice     | application     | filesystem */
/* RAM layout:   softdevice RAM | application RAM */
_sd_size       = DEFINED(_sd_size) ? _sd_size : 0;
_sd_ram        = DEFINED(_sd_ram)  ? _sd_ram : 0;
_fs_size       = DEFINED(_fs_size) ? _fs_size : 64K; /* TODO: set to 0 if not using the filesystem */
_app_size      = 512K - _fs_size;
_app_start     = 192K;
_fs_start      = _sd_size + _app_size;
_fs_end        = _fs_start + _fs_size;
_app_ram_start = 0x20000000 + _sd_ram;
_app_ram_size  = _ram_size - _sd_ram;
_heap_start    = _ebss;
_heap_end      = _ram_end - _stack_size;
_heap_size     = _heap_end - _heap_start;

ASSERT(_heap_size >= _minimum_heap_size, "not enough RAM left for heap")

/* Specify the memory areas */
MEMORY
{
    FLASH_TEXT (rx) : ORIGIN = _app_start,     LENGTH = _app_size     /* app */
    RAM (xrw)       : ORIGIN = _app_ram_start, LENGTH = _app_ram_size
}
