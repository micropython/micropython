/*
    Linker script fragment for mboot on an STM32N6xx MCU.
    This defines the memory sections for the bootloader to use.

    On N6, the hardware bootloader loads the first 512k of external flash into
    the upper part of SRAM2 AXI S, starting at 0x34180000.  The first 1024 bytes
    is a header. Then comes the actual code, starting with the vector table.
*/

MEMORY
{
    FLASH_BL (rx)   : ORIGIN = 0x34180400, LENGTH = 31744   /* AXISRAM2_S */
    RAM (xrw)       : ORIGIN = 0x341e0000, LENGTH = 128K    /* AXISRAM2_S */
}

/* Location of protected flash area which must not be modified, because mboot lives there. */
_mboot_protected_flash_start = ORIGIN(FLASH_BL);
_mboot_protected_flash_end_exclusive = ORIGIN(FLASH_BL) + LENGTH(FLASH_BL);
