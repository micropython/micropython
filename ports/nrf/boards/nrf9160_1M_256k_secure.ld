/* Specify the memory areas */
MEMORY
{
    FLASH_TEXT (rx) : ORIGIN = 0x00000000,     LENGTH = 32K
    RAM (xrw)       : ORIGIN = 0x20000000,     LENGTH = 64K
}
