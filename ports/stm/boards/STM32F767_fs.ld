/*
    GNU linker script for STM32F767 with filesystem
*/

/* Entry Point */
ENTRY(Reset_Handler)

_ld_default_stack_size = 24K;

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)          : ORIGIN = 0x08000000, LENGTH = 2048K /* entire flash */
    FLASH_ISR (rx)      : ORIGIN = 0x08000000, LENGTH = 32K /* sector 0 */
    FLASH_FS (rx)       : ORIGIN = 0x08008000, LENGTH = 96K /* sectors 1,2,3 are 32K */
    FLASH_FIRMWARE (rx) : ORIGIN = 0x08020000, LENGTH = 1920K /* sector 4 is 128K, sectors 5,6,7 are 256K */
    DTCM (xrw)          : ORIGIN = 0x20000000, LENGTH = 128K
    RAM (xrw)           : ORIGIN = 0x20020000, LENGTH = 384K    /* AXI SRAM */
    ITCM (xrw)          : ORIGIN = 0x00000000, LENGTH = 16K
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 24K;
_minimum_heap_size = 16K;

/* Define tho top end of the stack.  The stack is full descending so begins just
   above last byte of RAM.  Note that EABI requires the stack to be 8-byte
   aligned for a call. */
_estack = ORIGIN(DTCM) + LENGTH(DTCM);

/* RAM extents for the garbage collector */
_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
