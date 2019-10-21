/*
    GNU linker script for SAMD21x18 (256K flash, 32K RAM)
*/

/* Specify the memory areas */
MEMORY
{
    /* 256 KiB but leave 256b for internal config and 256b for user config (protected eeprom) */
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 256K - 256 - 256
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 32K
}

INCLUDE "samd21-common.ld"
