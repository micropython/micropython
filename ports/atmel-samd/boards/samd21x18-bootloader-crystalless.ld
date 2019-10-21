/*
    GNU linker script for SAMD21x18 (256K flash, 32K RAM)
*/

/* Specify the memory areas */
MEMORY
{
    /* Leave 8KiB for the bootloader, 256b for persistent config (clock), 64k for the flash file system and 256b for the user config. */
    FLASH (rx) : ORIGIN = 0x00000000 + 8K, LENGTH = 256K - 8K - 256 - 64K - 256
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 32K
}

INCLUDE "samd21-common.ld"
