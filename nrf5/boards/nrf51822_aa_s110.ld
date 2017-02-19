/*
    GNU linker script for NRF51822 AA w/ S110 8.0.0 SoftDevice
*/
/* Specify the memory areas */
SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x00000000, LENGTH = 0x040000 /* entire flash, 256 KiB */
    FLASH_ISR (rx)  : ORIGIN = 0x00018000, LENGTH = 0x000400 /* sector 0, 1 KiB */
    FLASH_TEXT (rx) : ORIGIN = 0x00018400, LENGTH = 0x027c00 /* 159 KiB */
    RAM (xrw)       : ORIGIN = 0x20001000, LENGTH = 0x003000 /* 12 KiB */
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 2K;
_minimum_heap_size = 8K;
 
/* top end of the stack */

/*_stack_end = ORIGIN(RAM) + LENGTH(RAM);*/
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* RAM extents for the garbage collector */
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
_heap_end = 0x20003000; /* tunable */

INCLUDE "boards/common.ld"
