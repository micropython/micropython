/*
    GNU linker script for SAMD21x18 (256K flash, 32K RAM)
*/

/* Specify the memory areas */
MEMORY
{
    /* Leave 8KiB for the bootloader and 256b for user config. */
    FLASH (rx) : ORIGIN = 0x00000000 + 8K, LENGTH = 256K - 8K - 256
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 32K /* 32 KiB RAM */
}

INCLUDE "samd21-common.ld"
