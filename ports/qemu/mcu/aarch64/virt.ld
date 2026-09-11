/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 MicroPython Contributors
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

/* Output format is 64 bits little endian AArch64. */
OUTPUT_FORMAT("elf64-littleaarch64", "elf64-littleaarch64", "elf64-littleaarch64");

MEMORY
{
    RAM   (xrw) : ORIGIN = 0x40000000, LENGTH = 6M
    STACK (rw)  : ORIGIN = 0x405E0000, LENGTH = 128K
}

SECTIONS
{
    . = ORIGIN(RAM);

    .text : ALIGN (4K)
    {
        *(.start)
        *(.text)
        . = ALIGN (4K);
    }

    .rodata : ALIGN (4K)
    {
        *(.rodata)
    }

    .data : ALIGN (4K)
    {
        *(.data)
    }

    .bss : ALIGN(4K)
    {
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4K);
        _ebss = .;
    }

    /* Stack is at the top of RAM, grows downward. No need to reserve
     * a section for it - just define the symbols. */
    _sstack = ORIGIN(STACK);
    _estack = ORIGIN(STACK) + LENGTH(STACK);

    /DISCARD/ :
    {
        *(.comment)
        *(.note*)
    }
}
