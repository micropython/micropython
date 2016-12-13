/*
    GNU linker script for NRF51822 AC w/ S130 2.0.1 SoftDevice
*/
/* Specify the memory areas */
SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x00000000, LENGTH = 0x040000 /* entire flash, 256 KiB */
    FLASH_ISR (rx)  : ORIGIN = 0x0001b000, LENGTH = 0x000400 /* sector 0, 1 KiB */
    FLASH_TEXT (rx) : ORIGIN = 0x0001b400, LENGTH = 0x024c00 /* 147 KiB */
    RAM (xrw)       : ORIGIN = 0x200013c8, LENGTH = 0x006c38 /* 27 KiB */
}
 
/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 2K;
_minimum_heap_size = 6K;
 
/* top end of the stack */

/*_stack_end = ORIGIN(RAM) + LENGTH(RAM);*/
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* RAM extents for the garbage collector */
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
_heap_end = 0x20002000; /* tunable */

INCLUDE "boards/common.ld"
