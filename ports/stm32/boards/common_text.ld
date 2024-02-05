/* This linker script fragment is intended to be included in SECTIONS. */

/* The program code and other data goes into FLASH */
.text :
{
    . = ALIGN(4);
    *(.text*)          /* .text* sections (code) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
/*  *(.glue_7)   */    /* glue arm to thumb code */
/*  *(.glue_7t)  */    /* glue thumb to arm code */

    . = ALIGN(4);
    _etext = .;        /* define a global symbol at end of code */
} >FLASH_COMMON
