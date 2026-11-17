/*
    GNU linker script for NRF51 AA
*/
SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

_flash_size = 256K;
_ram_size = 16K;
_micropy_hw_romfs_part0_size = 12K;

/* Default stack size when there is no SoftDevice */
_stack_size        = 4K;
_minimum_heap_size = 8K;
