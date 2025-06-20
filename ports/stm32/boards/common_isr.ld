/* This linker script fragment is intended to be included in SECTIONS. */

/* The startup code goes first into FLASH */
.isr_vector :
{
    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(4);
} >FLASH_COMMON
