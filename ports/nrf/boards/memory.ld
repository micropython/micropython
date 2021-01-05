
/* Flash layout: bootloader_head | softdevice     | application     | filesystem    | bootloader_tail */
/* RAM layout:   bootloader RAM  | softdevice RAM  | application RAM */
_bootloader_head_size = DEFINED(_bootloader_head_size) ? _bootloader_head_size : 0;
_bootloader_tail_size = DEFINED(_bootloader_tail_size) ? _bootloader_tail_size : 0;
_bootloader_head_ram_size = DEFINED(_bootloader_head_ram_size) ? _bootloader_head_ram_size : 0;
_head_size     = DEFINED(_sd_size) ? _sd_size : _bootloader_head_size;
_head_ram      = DEFINED(_sd_ram)  ? _sd_ram : _bootloader_head_ram_size;
_sd_size       = DEFINED(_sd_size) ? _sd_size : 0;
_sd_ram        = DEFINED(_sd_ram)  ? _sd_ram : 0;
_fs_size       = DEFINED(_fs_size) ? _fs_size : 64K; /* TODO: set to 0 if not using the filesystem */
_app_size      = _flash_size - _head_size - _fs_size  - _bootloader_tail_size;
_app_start     = _head_size;
_fs_start      = _head_size + _app_size;
_fs_end        = _fs_start + _fs_size;
_app_ram_start = 0x20000000 + _head_ram;
_app_ram_size  = _ram_size - _head_ram;
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
