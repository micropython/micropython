/*
    GNU linker script for NRF52840
*/

_flash_size = 1M;
_ram_size   = 256K;

/* produce a link error if there is not this amount of RAM for these sections */
_stack_size = 8K;
_minimum_heap_size = 128K;
