/*
    GNU linker script for NRF9160 NS
*/

_flash_size = 1M;
_ram_size   = 256K;
_sd_size = 0x00008000;
_sd_ram  = 0x00020000;
_fs_size = 80K;

/* produce a link error if there is not this amount of RAM for these sections */
_stack_size = 32K;
_minimum_heap_size = 64K;
