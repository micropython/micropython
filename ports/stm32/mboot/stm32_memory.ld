/*
    Linker script fragment for mboot on an STM32xxx MCU.
    This defines the memory sections for the bootloader to use.
*/

MEMORY
{
    FLASH_BL (rx)   : ORIGIN = 0x08000000, LENGTH = 32K
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 120K
}
