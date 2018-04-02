
/* Flash layout: softdevice     | application     | filesystem */
/* RAM layout:   softdevice RAM | application RAM */
_sd_size       = DEFINED(_sd_size) ? _sd_size : 0;
_sd_ram        = DEFINED(_sd_ram)  ? _sd_ram : 0;
_fs_size       = DEFINED(_fs_size) ? _fs_size : 64K; /* TODO: set to 0 if not using the filesystem */
_app_size      = _flash_size - _sd_size - _fs_size;
_app_start     = _sd_size;
_fs_start      = _sd_size + _app_size;
_fs_end        = _fs_start + _fs_size;
_app_ram_start = 0x20000000 + _sd_ram;
_app_ram_size  = _ram_size - _sd_ram;

/* Specify the memory areas */
MEMORY
{
    FLASH_TEXT (rx) : ORIGIN = _app_start,     LENGTH = _app_size     /* app */
    RAM (xrw)       : ORIGIN = _app_ram_start, LENGTH = _app_ram_size
}
