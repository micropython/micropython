/*
    GNU linker script for NRF51822 AC w/ S110 8.0.0 SoftDevice
*/
/* Specify the memory areas */
SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x00000000, LENGTH = 256K /* entire flash */
    FLASH_TEXT (rx) : ORIGIN = 0x00018000, LENGTH = 140K /* app */
    FLASH_USER (rx) : ORIGIN = 0x0003B000, LENGTH = 20K  /* app data, filesystem */
    RAM (xrw)       : ORIGIN = 0x20002000, LENGTH = 24K  /* app RAM */
}
 
/* produce a link error if there is not this amount of RAM for these sections */
_stack_size = 4K;
_minimum_heap_size = 1K;

INCLUDE "boards/common.ld"
