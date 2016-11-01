/*
    GNU linker script for STM32F405
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 1024K
    FLASH_ISR (rx)  : ORIGIN = 0x08000000, LENGTH = 16K     /* sector 0: 16K */
    FLASH_FS (rx)   : ORIGIN = 0x08004000, LENGTH = 112K    /* sectors 1-4: 3*16K+64K = 112K */
    FLASH_TEXT (rx) : ORIGIN = 0x08020000, LENGTH = 896K    /* sectors 5-11: 7*128K = 896K */
    CCMRAM (xrw)    : ORIGIN = 0x10000000, LENGTH = 64K
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 128K
}

/* define common sections and symbols */
INCLUDE common.ld
