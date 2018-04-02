/*
    GNU linker script for NRF51 AC
*/
SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

_flash_size = 256K;
_ram_size = 32K;

/* Default stack size when there is no SoftDevice */
_stack_size        = 4K;
_minimum_heap_size = 24K;
