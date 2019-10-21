/*
    GNU linker script for SAMD51x20 (1MB flash, 256K RAM)
*/

/* Specify the memory areas */
MEMORY
{
    /* Leave 16KiB for the bootloader, 512k for the filesystem and 8k for user config data. */
    FLASH (rx) : ORIGIN = 0x00000000 + 16K, LENGTH = 1M - 16K - 512K - 8K
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 256K
}
INCLUDE "samd51-common.ld"
