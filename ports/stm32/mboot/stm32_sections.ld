/*
    Linker script fragment for mboot on an STM32xxx MCU.
    This needs the following MEMORY sections to be defined: FLASH_BL, RAM.
*/

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 8K;

/* Define tho top end of the stack.  The stack is full descending so begins just
   above last byte of RAM.  Note that EABI requires the stack to be 8-byte
   aligned for a call. */
_estack = ORIGIN(RAM) + LENGTH(RAM);

ENTRY(Reset_Handler)

SECTIONS
{
    /* The startup code goes first into FLASH */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector)) /* Startup code */
        . = ALIGN(4);
    } >FLASH_BL

    /* The program code and other data goes into FLASH */
    .text :
    {
        . = ALIGN(4);
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
    } >FLASH_BL

    /* Secure Gateway stubs */
    .gnu.sgstubs :
    {
        . = ALIGN(4);
        *(.gnu.sgstubs*)
        . = ALIGN(4);
    } >FLASH_BL

    /* used by the startup to initialize data */
    _sidata = LOADADDR(.data);

    /* Initialized data section */
    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data*)

        . = ALIGN(4);
        _edata = .;
    } >RAM AT> FLASH_BL

    /* Final section of mboot flash reserved for mboot version */
    .mboot_version_text (ORIGIN(FLASH_BL) + LENGTH(FLASH_BL) - mboot_version_len) :
    {
        KEEP(*(.mboot_version));
    } >FLASH_BL

    /* Zeroed-out data section */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } >RAM

    /* Uninitialized data section */
    .nozero_bss (NOLOAD) :
    {
        . = ALIGN(4);
        *(.nozero_bss*)
        . = ALIGN(4);
    } >RAM

    /* this just checks there is enough RAM for the stack */
    .stack :
    {
        . = ALIGN(4);
        . = . + _minimum_stack_size;
        . = ALIGN(4);
    } >RAM

    .ARM.attributes 0 : { *(.ARM.attributes) }
}
