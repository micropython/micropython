/*
    GNU linker script for STM32F439
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 2048K /* entire flash */
    FLASH_ISR (rx)  : ORIGIN = 0x08000000, LENGTH = 16K /* sector 0 */
    FLASH_TEXT (rx) : ORIGIN = 0x08020000, LENGTH = 896K /* sectors 5-11 are 128K */
    FLASH_FS (rx)   : ORIGIN = 0x08100000, LENGTH = 256K /* sectors 12-17 are 4*16K+64K+128K */
    FLASH_FS2 (rx)  : ORIGIN = 0x08140000, LENGTH = 128K /* sector 18 */
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 192K
    CCMRAM (xrw)    : ORIGIN = 0x10000000, LENGTH = 64K
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 2K;
_minimum_heap_size = 16K;

/* top end of the stack */
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* RAM extents for the garbage collector */
_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
_heap_start = _ebss; /* heap starts just after statically allocated memory */
_heap_end = 0x2002c000; /* tunable */
