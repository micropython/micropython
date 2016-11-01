/*
    GNU linker script for STM32F439
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 2048K
    FLASH_ISR (rx)  : ORIGIN = 0x08000000, LENGTH = 16K     /* sector 0: 16K */
    FLASH_TEXT (rx) : ORIGIN = 0x08020000, LENGTH = 896K    /* sectors 5-11: 7*128K = 896K */
    FLASH_FS (rx)   : ORIGIN = 0x08100000, LENGTH = 192K    /* sectors 12-17: 4*16K+64K+64K(of 128K) = 192K */
    FLASH_FS2 (rx)  : ORIGIN = 0x08140000, LENGTH = 64K     /* sector 18: 64K(of 128K) */
    CCMRAM (xrw)    : ORIGIN = 0x10000000, LENGTH = 64K
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 192K
}

/* define common sections and symbols */
INCLUDE common.ld
