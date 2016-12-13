/*
    GNU linker script for NRF52 w/ s132 3.0.0 SoftDevice
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x00000000, LENGTH = 0x080000 /* entire flash, 512 KiB */
    FLASH_ISR (rx)  : ORIGIN = 0x0001f000, LENGTH = 0x001000 /* sector 0, 4 KiB */
    FLASH_TEXT (rx) : ORIGIN = 0x00020000, LENGTH = 0x060000 /* 396 KiB */
    RAM (xrw)       : ORIGIN = 0x200039c0, LENGTH = 0x0c640  /* 57.89 KiB, give 8KiB headroom for softdevice */
}
 
/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 8K;
_minimum_heap_size = 8K;
 
/* top end of the stack */

/*_stack_end = ORIGIN(RAM) + LENGTH(RAM);*/
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* RAM extents for the garbage collector */
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
_heap_end = 0x200069c0; /* tunable */

__data_start__ = ORIGIN(RAM);

SECTIONS
{
  .fs_data :
  {
    PROVIDE(__start_fs_data = .);
    KEEP(*(.fs_data))
    PROVIDE(__stop_fs_data = .);
  } > RAM
} INSERT AFTER .data;


INCLUDE "boards/common.ld"
