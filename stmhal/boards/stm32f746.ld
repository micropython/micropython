/*
    GNU linker script for STM32F746
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 1024K
    FLASH_ISR (rx)  : ORIGIN = 0x08000000, LENGTH = 32K     /* sector 0: 32K */
    FLASH_FS (rx)   : ORIGIN = 0x08008000, LENGTH = 96K     /* sectors 1-3: 3*32K = 96K */
    FLASH_TEXT (rx) : ORIGIN = 0x08020000, LENGTH = 896K    /* sectors 4-7: 1*128K + 3*256K = 896K */
    DTCM (xrw)      : ORIGIN = 0x20000000, LENGTH = 64K
    RAM (xrw)       : ORIGIN = 0x20010000, LENGTH = 256K    /* SRAM1 = 240K, SRAM2 = 16K */
}

/* define common sections and symbols */
INCLUDE common.ld
