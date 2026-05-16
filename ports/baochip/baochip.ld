/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 MicroPython contributors
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

/*
 * baochip.ld - Linker script for the baochip MicroPython port.
 *
 * Adapted from lib/dabao-sdk/bao1x.ld.  Keeps the same memory map,
 * section names, and entry-point address that boot1 expects, so the
 * SDK's crt0.S links unmodified.  Adds an explicit MicroPython heap
 * region between BSS end and the stack reservation, and bumps the
 * stack from 8 KiB to 16 KiB (8 KiB is too tight for the parser).
 *
 * Memory layout:
 *   RRAM:  0x60000000 - 0x603FFFFF (4 MiB; boot0+boot1+user code+ROM data)
 *   SRAM:  0x61000000 - 0x611FFFFF (2 MiB; .data, .bss, heap, stack)
 *   IFRAM: 0x50000000 - 0x5001FFFF (128 KiB; UDMA-only buffer region)
 *
 * User code starts at 0x60060000.  The first 768 bytes (0x300) are
 * the Ed25519ph signature block, populated post-link by
 * lib/dabao-sdk/tools/sign_and_uf2.py.  Actual code entry is
 * 0x60060300.
 */

ENTRY(_start)

MEMORY
{
    SIGBLOCK (r)  : ORIGIN = 0x60060000, LENGTH = 0x300
    RRAM (rx)     : ORIGIN = 0x60060300, LENGTH = 0x39A000 - 0x300
    SRAM (rwx)    : ORIGIN = 0x61000000, LENGTH = 0x200000
    IFRAM (rw)    : ORIGIN = 0x50000000, LENGTH = 0x20000
}

_stack_size = 0x4000;
_stack_top  = ORIGIN(SRAM) + LENGTH(SRAM);

SECTIONS
{
    .text : ALIGN(4)
    {
        *(.text.start)
        *(.text .text.*)
        *(.rodata .rodata.*)
        . = ALIGN(4);
    } > RRAM

    .data : ALIGN(4)
    {
        _data_start = .;
        *(.data .data.*)
        *(.sdata .sdata.*)
        . = ALIGN(4);
        _data_end = .;
    } > SRAM AT > RRAM

    _data_lma = LOADADDR(.data);

    .bss (NOLOAD) : ALIGN(4)
    {
        _bss_start = .;
        *(.bss .bss.*)
        *(.sbss .sbss.*)
        *(COMMON)
        . = ALIGN(4);
        _bss_end = .;
    } > SRAM

    /*
     * MicroPython GC heap occupies everything between BSS end and the
     * stack reservation at the top of SRAM.  Exposed via PROVIDE
     * symbols rather than a real section so the linker doesn't need to
     * allocate placeholder space; main.c calls
     * gc_init(&_heap_start, &_heap_end).  The stack pointer is set by
     * crt0 to _stack_top and grows downward, so the lower bound of the
     * stack is _stack_top - _stack_size; the heap must end there.
     */
    PROVIDE(_heap_start = _bss_end);
    PROVIDE(_heap_end = _stack_top - _stack_size);

    /*
     * UDMA buffer section.  Every buffer used by the UDMA engine must
     * land here (IFRAM at 0x50000000) -- placing a UDMA buffer in SRAM
     * silently fails because the engine's address decoder can't reach
     * SRAM.  Drivers should annotate with
     *     __attribute__((section(".dma_buffers")))
     * Currently empty but the section is declared so later drivers
     * need not touch this file.
     */
    .dma_buffers (NOLOAD) : ALIGN(4)
    {
        _dma_start = .;
        *(.dma_buffers .dma_buffers.*)
        . = ALIGN(4);
        _dma_end = .;
    } > IFRAM

    /* Global pointer for linker relaxation; placed in the middle of
     * the small-data region. */
    __global_pointer$ = _data_start + 0x800;

    /DISCARD/ :
    {
        *(.comment)
        *(.note.*)
        *(.eh_frame*)
    }
}

/*
 * Catch .bss/.data overrun before runtime: if .bss + .data grows past
 * the stack reservation, _heap_end is below _heap_start, gc_init runs
 * with start > end, and the chip silently misbehaves.
 */
ASSERT(_heap_end > _heap_start,
       "GC heap collapsed: .bss/.data grew past the stack reservation")
