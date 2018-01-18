/*
    GNU linker script for NRF52 w/ s132 5.0.0 SoftDevice
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x00000000, LENGTH = 512K     /* entire flash */
    FLASH_TEXT (rx) : ORIGIN = 0x00023000, LENGTH = 308K     /* app */
    FLASH_USER (rx) : ORIGIN = 0x00070000, LENGTH = 64K      /* app data, filesystem */
    RAM (xrw)       : ORIGIN = 0x200039c0, LENGTH = 0x0c640  /* 49.5 KiB, give 8KiB headroom for softdevice */
}
 
/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 2K;
_minimum_heap_size = 16K;
 
/* top end of the stack */

/*_stack_end = ORIGIN(RAM) + LENGTH(RAM);*/
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* RAM extents for the garbage collector */
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
_heap_end = 0x20007000; /* tunable */

INCLUDE "boards/common.ld"
