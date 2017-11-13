/*
    GNU linker script for NRF51 AA w/ no SoftDevice
*/
/* Specify the memory areas */
SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x00000000, LENGTH = 256K /* entire flash */
    FLASH_TEXT (rx) : ORIGIN = 0x00000000, LENGTH = 192K /* app */
    FLASH_USER (rx) : ORIGIN = 0x00030000, LENGTH = 64K  /* app data, filesystem */
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 16K  /* use all RAM */
}
 
/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 4K;
_minimum_heap_size = 8K;
 
/* top end of the stack */

/*_stack_end = ORIGIN(RAM) + LENGTH(RAM);*/
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* RAM extents for the garbage collector */
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
_heap_end = 0x20002000; /* tunable */

INCLUDE "boards/common.ld"
