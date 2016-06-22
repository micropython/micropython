/*
    GNU linker script for NRF52 blank w/ no SoftDevice
*/
/* Specify the memory areas */
SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x00000000, LENGTH = 0x040000 /* entire flash, 256 KiB */
    FLASH_ISR (rx)  : ORIGIN = 0x00000000, LENGTH = 0x000400 /* sector 0, 1 KiB */
    FLASH_TEXT (rx) : ORIGIN = 0x00000400, LENGTH = 0x03F000 /* 255 KiB */
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 0x008000 /* 32 KiB */
}
 
/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 4K;
_minimum_heap_size = 24K;
 
/* top end of the stack */

/*_stack_end = ORIGIN(RAM) + LENGTH(RAM);*/
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* RAM extents for the garbage collector */
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
_heap_end = 0x20006000; /* tunable */

INCLUDE "boards/common.ld"
