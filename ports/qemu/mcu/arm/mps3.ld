/* This file is part of the MicroPython project, http://micropython.org/
 * The MIT License (MIT)
 * Copyright (c) 2018 Damien P. George
 */

MEMORY
{
    ITCM : ORIGIN = 0x00000000, LENGTH = 512K
    RAM  : ORIGIN = 0x01000000, LENGTH = 2M
}

_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
    .isr_vector : {
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > ITCM

    .text : {
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        *(.ARM.exidx*)
        . = ALIGN(4);
        _etext = .;
        _sidata = _etext;
    } > RAM

    .data : AT ( _sidata )
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } >RAM

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } >RAM
}
