/*
    GNU linker script for NRF52 w/ s132 3.0.0 SoftDevice
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x00000000, LENGTH = 512K     /* entire flash */
    FLASH_TEXT (rx) : ORIGIN = 0x0001F000, LENGTH = 324K     /* app */
    FLASH_USER (rx) : ORIGIN = 0x00070000, LENGTH = 64K      /* app data, filesystem */
    RAM (xrw)       : ORIGIN = 0x200039c0, LENGTH = 0x0c640  /* 49.5 KiB, give 8KiB headroom for softdevice */
}
 
/* produce a link error if there is not this amount of RAM for these sections */
_stack_size = 8K;
_minimum_heap_size = 16K;

INCLUDE "boards/common.ld"
