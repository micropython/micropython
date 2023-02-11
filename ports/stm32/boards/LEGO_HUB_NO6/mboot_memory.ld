/*
    Linker script fragment for mboot on an STM32xxx MCU.
    This defines the memory sections for the bootloader to use.
*/
MEMORY
{
    FLASH_BL (rx)   : ORIGIN = 0x08008000, LENGTH = 32K
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 120K
}

/* Location from which mboot is allowed to write to flash.
   Must be the start of a flash erase sector. */
_mboot_writable_flash_start = ORIGIN(FLASH_BL) + LENGTH(FLASH_BL);
