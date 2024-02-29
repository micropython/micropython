/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Daniel Campora
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

__stack_size__    = 2K;       /* interrupts are handled within this stack */
__min_heap_size__ = 8K;

MEMORY
{
    SRAMB (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00004000
    SRAM  (rwx) : ORIGIN = 0x20004000, LENGTH = 0x0003C000
}

ENTRY(ResetISR)

SECTIONS
{
    /* place the FreeRTOS heap (the MicroPython stack will live here) */
    .rtos_heap (NOLOAD) :
    {
        . = ALIGN(8);
        *(.rtos_heap*)
        . = ALIGN(8);
    } > SRAMB

    .text :
    {
        _text = .;
        KEEP(*(.intvecs))
        *(.text*)
        *(.rodata*)
        *(.ARM.extab* .gnu.linkonce.armextab.*)
        . = ALIGN(8);
    } > SRAM

    .ARM :
    {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
        _etext = .;
    } > SRAM

    .data :
    {
        . = ALIGN(8);
        _data = .;
        *(.data*)
        . = ALIGN(8);
        _edata = .;
    } > SRAM

    .bss :
    {
        . = ALIGN(8);
        _bss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(8);
        _ebss = .;
    } > SRAM

    /* place here functions that are only called during boot up, */
    /* that way, we can reuse this area for the MicroPython heap */
    .boot :
    {
        . = ALIGN(8);
        _boot = .;
        *(.boot*)
        . = ALIGN(8);
        _eboot = .;
    } > SRAM

    /* allocate the MicroPython heap */
    .heap :
    {
        . = ALIGN(8);
        _heap = .;
        . = . + __min_heap_size__;
        . = . + (ORIGIN(SRAM) + LENGTH(SRAM) - __stack_size__ - ABSOLUTE(.));
        . = ALIGN(8);
        _eheap = .;
    } > SRAM

    /* allocate the main stack */
    .stack ORIGIN(SRAM) + LENGTH(SRAM) - __stack_size__ :
    {
        . = ALIGN(8);
        _stack = .;
        . = . + __stack_size__;
        . = ALIGN(8);
        _estack = .;
    } > SRAM
}
