/*
    GNU linker script for NRF51822 AC w/ S120 2.1.0 SoftDevice
*/
/* Specify the memory areas */
SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x00000000, LENGTH = 256K /* entire flash */
    FLASH_TEXT (rx) : ORIGIN = 0x0001D000, LENGTH = 130K /* app */
    FLASH_USER (rx) : ORIGIN = 0x0003D800, LENGTH = 10K  /* app data, filesystem */
    RAM (xrw)       : ORIGIN = 0x20002800, LENGTH = 22K  /* app RAM */
}
 
/* produce a link error if there is not this amount of RAM for these sections */
_stack_size = 4K;
_minimum_heap_size = 4K;

INCLUDE "boards/common.ld"
