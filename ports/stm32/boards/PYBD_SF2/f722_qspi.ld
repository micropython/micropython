/*
    Linker script for PYBD with STM32F722/STM32F723/STM32F732/STM32F733

    Memory layout for mboot configuration (this here describes the app part):

    FLASH_APP   .isr_vector
    FLASH_APP   .text
    FLASH_APP   .data

    FLASH_EXT   .big_const

    RAM         .data
    RAM         .bss
    RAM         .heap
    RAM         .stack
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 512K
    FLASH_ISR (rx)  : ORIGIN = 0x08000000, LENGTH = 32K     /* sectors 0,1 */
    FLASH_APP (rx)  : ORIGIN = 0x08008000, LENGTH = 480K    /* sectors 2-7 */
    FLASH_EXT (rx)  : ORIGIN = 0x90000000, LENGTH = 2048K   /* external QSPI */
    RAM (rwx)       : ORIGIN = 0x20000000, LENGTH = 256K    /* DTCM+SRAM1+SRAM2 */
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 2K;
_minimum_heap_size = 16K;

/* Define tho top end of the stack.  The stack is full descending so begins just
   above last byte of RAM.  Note that EABI requires the stack to be 8-byte
   aligned for a call. */
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* RAM extents for the garbage collector */
_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
_heap_start = _ebss; /* heap starts just after statically allocated memory */
_heap_end = _ram_end - 16K; /* 16k stack */

ENTRY(Reset_Handler)

/* Define output sections */
SECTIONS
{
    .text_ext :
    {
        . = ALIGN(512);
        *(.big_const*)
        . = ALIGN(4);
    } >FLASH_EXT

    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } >FLASH_APP

    .text :
    {
        . = ALIGN(4);
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
    } >FLASH_APP

    _sidata = LOADADDR(.data);

    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data*)

        . = ALIGN(4);
        _edata = .;
    } >RAM AT> FLASH_APP

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } >RAM

    .heap :
    {
        . = ALIGN(4);
        . = . + _minimum_heap_size;
        . = ALIGN(4);
    } >RAM

    .stack :
    {
        . = ALIGN(4);
        . = . + _minimum_stack_size;
        . = ALIGN(4);
    } >RAM
}
