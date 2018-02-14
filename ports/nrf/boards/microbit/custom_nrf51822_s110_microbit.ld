/*
    GNU linker script for NRF51822 AA w/ S110 8.0.0 SoftDevice
*/
/* Specify the memory areas */
SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x00000000, LENGTH = 256K /* entire flash */
    FLASH_TEXT (rx) : ORIGIN = 0x00018000, LENGTH = 148K /* app */
    FLASH_USER (rx) : ORIGIN = 0x0003D000, LENGTH = 12K  /* app data, filesystem */
    RAM (xrw)       : ORIGIN = 0x20002000, LENGTH = 8K   /* app RAM */
}

/* produce a link error if there is not this amount of RAM for these sections */
_stack_size = 2K;
_minimum_heap_size = 1K;

INCLUDE "boards/common.ld"
