/*
    GNU linker script for STM32F429i-Discovery kit with external 8MByte SDRam
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 2048K
    FLASH_ISR (rx)  : ORIGIN = 0x08000000, LENGTH = 16K     /* sector 0: 16K */
    FLASH_FS (rx)   : ORIGIN = 0x08004000, LENGTH = 112K    /* sectors 1-4: 3*16K+64K = 112K */
    FLASH_TEXT (rx) : ORIGIN = 0x08020000, LENGTH = 896K    /* sectors 5-11: 7*128K = 896K */
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 192K
    SDRAM(xrw)      : ORIGIN = 0xC0000000, LENGTH = 8192K
}

/* define common sections and symbols */
INCLUDE common.ld
