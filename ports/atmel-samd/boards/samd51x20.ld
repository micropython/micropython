/*
    GNU linker script for SAMD51x20 (1MB flash, 256K RAM)
*/

/* Specify the memory areas */
MEMORY
{
    /* 1024 KiB minus 512KiB for the internal file system and 8KiB for the user nvm. */
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 1M - 512K - 8K
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 256K
}

INCLUDE "samd51-common.ld"
