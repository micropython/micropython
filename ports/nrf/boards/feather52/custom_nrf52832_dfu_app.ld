/*
    GNU linker script for NRF52 w/ s132 2.0.1 SoftDevice
*/

/* Specify the memory areas */
/* Memory map: https://learn.adafruit.com/bluefruit-nrf52-feather-learning-guide/memory-map */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x00000000, LENGTH = 512K     /* entire flash */
    FLASH_TEXT (rx) : ORIGIN = 0x0001C000, LENGTH = 162K     /* app */
    FLASH_TEMP (rx) : ORIGIN = 0x00044800, LENGTH = 162K     /* temporary storage area for DFU */
    FLASH_USER (rx) : ORIGIN = 0x0006D000, LENGTH = 28K      /* app data, filesystem */
    RAM (xrw)       : ORIGIN = 0x200039C0, LENGTH = 0x0C640  /* 49.5 KiB, give 8KiB headroom for softdevice */
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
