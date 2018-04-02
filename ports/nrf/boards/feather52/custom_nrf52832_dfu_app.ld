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
_stack_size = 8K;
_minimum_heap_size = 16K;

_fs_start = ORIGIN(FLASH_USER);
_fs_end   = ORIGIN(FLASH_USER) + LENGTH(FLASH_USER);

INCLUDE "boards/common.ld"
