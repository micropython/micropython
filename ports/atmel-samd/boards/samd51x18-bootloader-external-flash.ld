/*
    GNU linker script for SAMD51x18 (256K flash, 128K RAM)
*/

/* Specify the memory areas */
MEMORY
{
    /* Leave 16KiB for the bootloader. 8K for user data*/
    FLASH (rx) : ORIGIN = 0x00000000 + 16K, LENGTH = 256K - 16K - 8K
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 128K
}

INCLUDE "samd51-common.ld"
