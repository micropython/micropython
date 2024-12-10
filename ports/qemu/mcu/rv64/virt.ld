/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Alessandro Gatti
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

/* Output format is 64 bits little endian. */
OUTPUT_FORMAT("elf64-littleriscv", "elf64-littleriscv", "elf64-littleriscv");

/*
 * Memory layout:
 *
 * 0x0000_0000_8000_0000: .text
 *             .........: .rodata
 * 0x0000_0000_8040_0000: .data
 *             .........: _global_pointer
 *             .........: .bss
 * 0x0000_0000_8060_0000: .stack
 * 0x0000_0000_8060_0000: _sstack
 * 0x0000_0000_8061_FFFF: _estack
 */
MEMORY
{
    ROM   (xr)  : ORIGIN = 0x0000000080000000,        LENGTH = 4M
    RAM   (xrw) : ORIGIN = ORIGIN(ROM) + LENGTH(ROM), LENGTH = 2M
    STACK (rw)  : ORIGIN = ORIGIN(RAM) + LENGTH(RAM), LENGTH = 128K
}

SECTIONS
{
    /* Code + Read-Only data segment */

    .text : ALIGN (4K)
    {
        *(.start)
        *(.text)
        . = ALIGN (4K);
        *(.rodata)
        _sirodata = .;
    } > ROM

    .rodata : AT (_sirodata) ALIGN (4K)
    {
        *(.rodata)
    } > ROM

    /* Data + BSS segment */

    .data : ALIGN (4K)
    {
        *(.data)
        _sibss = .;
    } > RAM

    .bss : AT (_sibss) ALIGN (4K)
    {
        /* Mark global pointer address. */
        _global_pointer = .;

        /* Mark BSS start. */
        . = . + 4;
        _sbss = .;
        *(.bss)
        /* Mark BSS end. */
        _ebss = .;
    } > RAM

    /* Isolated stack segment. */

    .stack : ALIGN(4K)
    {
        /* Mark stack start. */
        _sstack = .;
        . = LENGTH(STACK);
        /* Mark stack end. */
        _estack = .;
    } > STACK
}
