/* Teensyduino Core Library
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2013 PJRC.COM, LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * 2. If the Software is incorporated into a build system that allows
 * selection among a list of target devices, then similar target
 * devices manufactured by PJRC.COM must be included in the list of
 * target devices and selectable in the same manner.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

MEMORY
{
        FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 256K
        RAM  (rwx) : ORIGIN = 0x1FFF8000, LENGTH = 64K
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 2K;
_minimum_heap_size = 16K;

/* INCLUDE common.ld */

/* Teensyduino Core Library
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2013 PJRC.COM, LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * 2. If the Software is incorporated into a build system that allows
 * selection among a list of target devices, then similar target
 * devices manufactured by PJRC.COM must be included in the list of
 * target devices and selectable in the same manner.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */



SECTIONS
{
    .text : {
        . = 0;
        KEEP(*(.vectors))
        *(.startup*)
        /* TODO: does linker detect startup overflow onto flashconfig? */
        . = 0x400;
        KEEP(*(.flashconfig*))
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        KEEP(*(.init))
        . = ALIGN(4);
        __preinit_array_start = .;
        KEEP (*(.preinit_array))
        __preinit_array_end = .;
        __init_array_start = .;
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array))
        __init_array_end = .;
    } > FLASH = 0xFF

    .ARM.exidx : {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > FLASH
    _etext = .;

    .usbdescriptortable (NOLOAD) : {
        /* . = ORIGIN(RAM); */
        . = ALIGN(512);
        *(.usbdescriptortable*)
    } > RAM

    .dmabuffers (NOLOAD) : {
        . = ALIGN(4);
        *(.dmabuffers*)
    } > RAM

    .usbbuffers (NOLOAD) : {
        . = ALIGN(4);
        *(.usbbuffers*)
    } > RAM

    /* used by the startup to initialize data */
    _sidata = LOADADDR(.data);

    .data : AT (_etext) {
        . = ALIGN(4);
        _sdata = .;
        _ram_start = .;
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > RAM

        /*
     * _staticfs is the place in flash where the static filesystem which
     * is concatenated to the .hex file will wind up.
         */
    _staticfs = LOADADDR(.data) + SIZEOF(.data);

    .noinit (NOLOAD) : {
        *(.noinit*)
    } > RAM

    .bss : {
        . = ALIGN(4);
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
        __bss_end = .;
    } > RAM

    /* this is to define the start of the heap, and make sure we have a minimum size */
    .heap :
        {
        . = ALIGN(4);
        _heap_start = .;    /* define a global symbol at heap start */
        . = . + _minimum_heap_size;
    } >RAM

    /* this just checks there is enough RAM for the stack */
    .stack :
        {
        . = ALIGN(4);
        . = . + _minimum_stack_size;
        . = ALIGN(4);
    } >RAM

    _estack = ORIGIN(RAM) + LENGTH(RAM);
    _ram_end = ORIGIN(RAM) + LENGTH(RAM);
    _heap_end = ORIGIN(RAM) + 0xe000;
}




