/*
    GNU linker script for NRF52 w/ s132 5.0.0 SoftDevice

    MEMORY MAP
    ------------------------------------------------------------------------
    START ADDR  END ADDR   SIZE    DESCRIPTION
    ----------  ---------- ------- -----------------------------------------
    0x0007F000..0x0007FFFF (  4KB) Bootloader Settings
    0x0007E000..0x0007EFFF (  4KB) Master Boot Record Params
    0x00074000..0x0007DFFF ( 40KB) Serial + OTA Bootloader
    
    0x00073000..0x00073FFF ( 4KB ) Private Config Data (Bonding, Keys, etc.)
    0x00072000..0x00072FFF ( 4KB ) User NVM data
    0x00059000..0x00071FFF ( 100KB) User Filesystem

    0x00023000..0x00058FFF (216KB) Application Code
    0x00001000..0x00022FFF (136KB) SoftDevice
    0x00000000..0x00000FFF   (4KB) Master Boot Record
*/

/* Specify the memory areas (S132 5.0.0) */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x00000000, LENGTH = 0x080000 /* entire flash, 512 KiB */
    FLASH_ISR (rx)  : ORIGIN = 0x00023000, LENGTH = 0x001000 /* sector 0, 4 KiB */
    FLASH_TEXT (rx) : ORIGIN = 0x00024000, LENGTH = 0x036000 /* APP - ISR, 216 KiB */
    FLASH_FATFS (r) : ORIGIN = 0x00059000, LENGTH = 0x019000 /* File system 100KB KB */
    RAM (xrw)       : ORIGIN = 0x200039c0, LENGTH = 0x0c640  /* 49.5 KiB, give 8KiB headroom for softdevice */
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 2K;
_minimum_heap_size = 0 /*16K Circuit Python use static variable for HEAP */;

/* top end of the stack */

/*_stack_end = ORIGIN(RAM) + LENGTH(RAM);*/
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* RAM extents for the garbage collector */
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
_heap_end = 0x20007000; /* tunable */

INCLUDE "boards/common.ld"
