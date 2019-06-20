/*
    Custom linker script for STM32F769DISC board with external memory-mapped QSPI flash

    Memory layout:

    FLASH_QSPI  .text_qspi

    FLASH_APP   .isr_vector
    FLASH_APP   .text
    FLASH_APP   .data

    RAM         .data
    RAM         .bss
    RAM         .heap
    RAM         .stack
*/

MEMORY
{
    FLASH_APP (rx)  : ORIGIN = 0x08020000, LENGTH = 2016K   /* sectors 1-11 3x32K 1*128K 7*256K */
    FLASH_QSPI (rx) : ORIGIN = 0x90000000, LENGTH = 64M     /* external QSPI flash in XIP mode */
    DTCM (xrw)      : ORIGIN = 0x20000000, LENGTH = 128K    /* Used for storage cache */
    RAM (xrw)       : ORIGIN = 0x20020000, LENGTH = 384K    /* SRAM1 = 368K, SRAM2 = 16K */
}

/* Produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 2K;
_minimum_heap_size = 16K;

/* Define the stack.  The stack is full descending so begins just above last byte
   of RAM.  Note that EABI requires the stack to be 8-byte aligned for a call. */
_estack = ORIGIN(RAM) + LENGTH(RAM);
_sstack = _estack - 32K; /* tunable */

/* RAM extents for the garbage collector */
_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
_heap_start = _ebss; /* heap starts just after statically allocated memory */
_heap_end = _sstack;

ENTRY(Reset_Handler)

SECTIONS
{
    /* Define the code that goes in QSPI flash */
    .text_qspi :
    {
        . = ALIGN(4);
        *extmod/*(.text* .rodata*)
        . = ALIGN(4);
    } >FLASH_QSPI

    /* The startup code goes first into main flash */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } >FLASH_APP

    /* The program code and other data goes into flash */
    .text :
    {
        . = ALIGN(4);
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
    } >FLASH_APP

    /* Used by the startup to initialize data */
    _sidata = LOADADDR(.data);

    /* The initialized data section */
    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } >RAM AT> FLASH_APP

    /* The uninitialized (zeroed) data section */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } >RAM

    /* Define the start of the heap, and make sure we have a minimum size */
    .heap :
    {
        . = ALIGN(4);
        . = . + _minimum_heap_size;
        . = ALIGN(4);
    } >RAM

    /* Just checks there is enough RAM for the stack */
    .stack :
    {
        . = ALIGN(4);
        . = . + _minimum_stack_size;
        . = ALIGN(4);
    } >RAM
}
