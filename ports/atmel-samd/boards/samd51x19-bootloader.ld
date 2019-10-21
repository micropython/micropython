/*
    GNU linker script for SAMD51x19 (512K flash, 192K RAM)
*/

/* Specify the memory areas */
MEMORY
{
    /* Leave 16KiB for the bootloader, 256KiB for the internal file system, and 8KiB for user binary data. */
    FLASH (rx) : ORIGIN = 0x00000000 + 16K, LENGTH = 512K - 16K - 256K - 8K
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 192K
}

INCLUDE "samd51-common.ld"
