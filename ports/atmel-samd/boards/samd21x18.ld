/*
    GNU linker script for SAMD21x18 (256K flash, 32K RAM)
*/

/* Specify the memory areas */
MEMORY
{
    /* Leave 256b for internal config, 64k for the flash file system and 256b for user config. */
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 256K - 256 - 64K
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 32K
}

INCLUDE "samd21-common.ld"
