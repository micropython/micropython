/* This file is part of the MicroPython project, http://micropython.org/
 * The MIT License (MIT)
 * Copyright (c) 2018 Damien P. George
 */

#ifndef ISR_SECTION
#define ISR_SECTION RAM
#endif

#ifndef TEXT_SECTION
#define TEXT_SECTION RAM
#endif

#ifndef DATA_SECTION
#define DATA_SECTION TEXT_SECTION
#endif

#ifndef BSS_SECTION
#define BSS_SECTION RAM
#endif

#ifndef ESTACK
#define ESTACK ORIGIN(RAM) + LENGTH(RAM)
#endif

_estack = ESTACK;

SECTIONS
{
    .isr_vector : {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > ISR_SECTION

    .text : {
        . = ALIGN(4);
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        *(.ARM.extab*)
        *(.gnu.linkonce.armextab.*)
        . = ALIGN(4);
        __exidx_start = .;
        *(.ARM.exidx*)
        *(.gnu.linkonce.armexidx.*)
        __exidx_end = .;
        . = ALIGN(4);
        _etext = .;
        _sidata = _etext;
    } > TEXT_SECTION

    .data : AT ( _sidata )
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > DATA_SECTION

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > BSS_SECTION
}
