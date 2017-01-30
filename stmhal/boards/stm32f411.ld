/*
    GNU linker script for STM32F411
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 512K
    FLASH_ISR (rx)  : ORIGIN = 0x08000000, LENGTH = 16K     /* sector 0: 16K */
    FLASH_FS (rx)   : ORIGIN = 0x08004000, LENGTH = 64K     /* sectors 1-4: 3*16K+16K(of 64K) = 64K */
    FLASH_TEXT (rx) : ORIGIN = 0x08020000, LENGTH = 384K    /* sectors 5-7: 3*128K = 384K */
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 128K
}

/* define common sections and symbols */
INCLUDE common.ld
