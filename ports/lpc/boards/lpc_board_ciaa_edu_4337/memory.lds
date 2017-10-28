/* Specify the memory areas */
MEMORY
{
    FLASH (rx) : ORIGIN = 0x1a000000, LENGTH = 512K
    RAM (xrw) : ORIGIN = 0x20000000, LENGTH = 64K

    FLASHB (rx) : ORIGIN = 0x1b000000, LENGTH = 512K
    RAMB (rwx) : ORIGIN = 0x10000000, LENGTH = 32K
}
