/*
    GNU linker script for NRF52832 blank w/ no SoftDevice
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x00000000, LENGTH = 512K /* entire flash */
    FLASH_TEXT (rx) : ORIGIN = 0x00000000, LENGTH = 448K /* app */
    FLASH_USER (rx) : ORIGIN = 0x00070000, LENGTH = 64K  /* app data, filesystem */
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 64K  /* use all RAM */
}
 
/* produce a link error if there is not this amount of RAM for these sections */
_stack_size = 8K;
_minimum_heap_size = 32K;

INCLUDE "boards/common.ld"
