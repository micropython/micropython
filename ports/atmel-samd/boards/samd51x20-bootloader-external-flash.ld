/*
    GNU linker script for SAMD51x20 (1MB flash, 256K RAM)
*/

/* Specify the memory areas */
MEMORY
{
    /* Leave 16KiB for the bootloader. */
    FLASH (rx) : ORIGIN = 0x00000000 + 16K, LENGTH = 1M - 16K - 8K
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 256K
}

/* top end of the stack */
/* stack must be double-word (8 byte) aligned */
_estack = ORIGIN(RAM) + LENGTH(RAM) - 8;
_bootloader_dbl_tap = ORIGIN(RAM) + LENGTH(RAM) - 4;

/* define output sections */
SECTIONS
{
    /* The program code and other data goes into FLASH */
    .text :
    {
        . = ALIGN(4);
        _sfixed = .;
        KEEP(*(.vectors)) /* isr vector table */
        *(.text)           /* .text sections (code) */
        *(.text*)          /* .text* sections (code) */
        *(.rodata)         /* .rodata sections (constants, strings, etc.) */
        *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */

        . = ALIGN(4);
    } >FLASH

    .ARM.exidx :
    {
        *(.ARM.exidx*)
        *(.gnu.linkonce.armexidx.*)
        _etext = .;        /* define a global symbol at end of code */
        _sidata = .;       /* start of .data section */
    } > FLASH

    /* This is the initialized data section
    The program executes knowing that the data is in the RAM
    but the loader puts the initial values in the FLASH (inidata).
    It is one task of the startup to copy the initial values from FLASH to RAM. */
    .data : AT ( _sidata )
    {
        . = ALIGN(4);
        _srelocate = .;        /* create a global symbol at data start; used by startup code in order to initialise the .data section in RAM */
        *(.ramfunc)
        *(.ramfunc*)
        *(.data)           /* .data sections */
        *(.data*)          /* .data* sections */

        . = ALIGN(4);
        _erelocate = .;        /* define a global symbol at data end; used by startup code in order to initialise the .data section in RAM */
    } >RAM

    /* Uninitialized data section */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        _szero = .;         /* define a global symbol at bss start; used by startup code */
        *(.bss)
        *(.bss*)
        *(COMMON)

        . = ALIGN(4);
        _ezero = .;         /* define a global symbol at bss end; used by startup code */
        _ebss = .;
    } >RAM

    /* this just checks there is enough RAM for a minimal stack */
    .stack :
    {
        . = ALIGN(4);
        . = . + 10K; /* Reserve a minimum of 10K for the stack. nvm will temporarily store 8k on the stack when writing. */
        . = ALIGN(4);
    } >RAM

    .ARM.attributes 0 : { *(.ARM.attributes) }
}
