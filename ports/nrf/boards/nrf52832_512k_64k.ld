/*
    GNU linker script for NRF52832
*/

_flash_size = 512K;
_ram_size   = 64K;
_micropy_hw_romfs_part0_size = 128K;

/* produce a link error if there is not this amount of RAM for these sections */
_stack_size = 8K;
_minimum_heap_size = 32K;
