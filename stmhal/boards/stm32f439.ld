/*
    GNU linker script for STM32F439
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 0x200000 /* entire flash, 2048 KiB */
    FLASH_ISR (rx)  : ORIGIN = 0x08000000, LENGTH = 0x004000 /* sector 0, 16 KiB */
    FLASH_TEXT (rx) : ORIGIN = 0x08020000, LENGTH = 0x080000 /* sectors 5,6,7,8, 4*128KiB = 512 KiB (could increase it more) */
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 0x030000 /* 192 KiB */
    CCMRAM (xrw)    : ORIGIN = 0x10000000, LENGTH = 0x010000 /* 64 KiB */
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 2K;
_minimum_heap_size = 16K;

/* top end of the stack */
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* define common sections and symbols */
INCLUDE common.ld

/* RAM extents for the garbage collector */
_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
_heap_start = _ebss; /* heap starts just after statically allocated memory */
_heap_end = 0x2002c000; /* tunable */
