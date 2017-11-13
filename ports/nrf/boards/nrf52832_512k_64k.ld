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
_minimum_stack_size = 2K;
_minimum_heap_size = 32K;
 
/* top end of the stack */

/*_stack_end = ORIGIN(RAM) + LENGTH(RAM);*/
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* RAM extents for the garbage collector */
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
_heap_end = 0x20008000; /* tunable */

INCLUDE "boards/common.ld"
