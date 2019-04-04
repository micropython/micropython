/*
    GNU linker script for NRF52840 w/S140 6.x.x SoftDevice

    MEMORY MAP
    ------------------------------------------------------------------------
    START ADDR  END ADDR   SIZE    DESCRIPTION
    ----------  ---------- ------- -----------------------------------------
    0x000FF000..0x000FFFFF (  4KB) Bootloader Settings
    0x000FE000..0x000FEFFF (  4KB) Master Boot Record Params
    0x000F4000..0x000FDFFF ( 40KB) Bootloader (UF2 + CDC + OTA)

    0x000ED000..0x000F3FFF (28KB ) Private Config Data (Bonding, Keys, etc.)
    0x000AD000..0x000ECFFF (256KB) User Filesystem
    0x00026000..0x000ACFFF (540KB) Application Code (including ISR vector)
    0x00001000..0x00025FFF (148KB) SoftDevice
    0x00000000..0x00000FFF   (4KB) Master Boot Record
*/

/* Specify the memory areas (S140 6.x.x) */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x00000000, LENGTH = 0x100000

    FLASH_ISR (rx)  : ORIGIN = 0x00026000, LENGTH = 0x001000
    FLASH_TEXT (rx) : ORIGIN = 0x00027000, LENGTH = 0x086000
    FLASH_FATFS (r) : ORIGIN = 0x000AD000, LENGTH = 0x040000

    /* 0x2000000 - RAM:ORIGIN is reserved for Softdevice */
    RAM (xrw)       : ORIGIN = 0x20004000, LENGTH = 0x20040000 - 0x20004000
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 2K;
_minimum_heap_size = 0;

/* top end of the stack */

/*_stack_end = ORIGIN(RAM) + LENGTH(RAM);*/
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* RAM extents for the garbage collector */
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
_heap_end = 0x20020000; /* tunable */

INCLUDE "boards/common.ld"
