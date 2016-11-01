/*
    GNU linker script for STM32L476XE
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 512K
    FLASH_ISR (rx)  : ORIGIN = 0x08000000, LENGTH = 16K     /* sectors 0-7: 8*2K = 16K */
    FLASH_TEXT (rx) : ORIGIN = 0x08004000, LENGTH = 368K    /* sectors 8-191: 184*2K = 368K */
    FLASH_FS (r)    : ORIGIN = 0x08060000, LENGTH = 128K    /* sectors 192-255: 64*2K = 128K */
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 96K
    SRAM2 (xrw)     : ORIGIN = 0x10000000, LENGTH = 32K
}

/* define common sections and symbols */
INCLUDE common.ld

_flash_fs_start = ORIGIN(FLASH_FS);
_flash_fs_end   = ORIGIN(FLASH_FS) + LENGTH(FLASH_FS);
