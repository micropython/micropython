/*
    GNU linker script for NRF52840 blank w/ no SoftDevice
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x00000000, LENGTH = 1M   /* entire flash */
    FLASH_TEXT (rx) : ORIGIN = 0x00000000, LENGTH = 960K /* app */
    FLASH_USER (rx) : ORIGIN = 0x000F0000, LENGTH = 64K  /* app data, filesystem */
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 256K /* use all RAM */
}

/* produce a link error if there is not this amount of RAM for these sections */
_stack_size = 8K;
_minimum_heap_size = 128K;
 
/* top end of the stack */

/*_stack_end = ORIGIN(RAM) + LENGTH(RAM);*/
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* RAM extents for the garbage collector */
_ram_end = ORIGIN(RAM) + LENGTH(RAM);

INCLUDE "boards/common.ld"
