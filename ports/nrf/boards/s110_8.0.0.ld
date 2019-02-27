
/* GNU linker script for s110 SoftDevice version 8.0.0 */

_sd_size = 0x00018000;
_sd_ram  = 0x00002000;
_fs_size = DEFINED(_fs_size) ? _fs_size : 20K;

_stack_size = _ram_size > 16K ? 4K : 2K;
_minimum_heap_size = 4K;
