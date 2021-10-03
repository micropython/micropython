/* This linker script fragment is intended to be included in SECTIONS. */

/* For C++ exception handling */
.ARM :
{
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
} >FLASH_TEXT

/* Used by the start-up code to initialise data */
_sidata = LOADADDR(.data);

/* Initialised data section, start-up code will copy it from flash to RAM */
.data :
{
    . = ALIGN(4);
    _sdata = .;
    *(.data*)
    . = ALIGN(4);
    _edata = .;
} >RAM AT> FLASH_TEXT
