/*
    GNU linker script for NRF52840 blank w/ no SoftDevice
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x00000000, LENGTH = 0x100000 /* entire flash, 1 MiB */
    FLASH_ISR (rx)  : ORIGIN = 0x00000000, LENGTH = 0x001000 /* sector 0, 4 KiB */
    FLASH_TEXT (rx) : ORIGIN = 0x00001000, LENGTH = 0x0FF000 /* 1020 KiB */
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 0x040000 /* 256 KiB */
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 40K;
_minimum_heap_size = 128K;
 
/* top end of the stack */

/*_stack_end = ORIGIN(RAM) + LENGTH(RAM);*/
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* RAM extents for the garbage collector */
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
_heap_end = 0x20020000; /* tunable */

INCLUDE "boards/common.ld"
