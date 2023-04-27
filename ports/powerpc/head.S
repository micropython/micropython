/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019, Michael Neuling, IBM Corporation.
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

#define STACK_TOP 0x60000

#define FIXUP_ENDIAN                                           \
    tdi   0,0,0x48;   /* Reverse endian of b . + 8          */ \
    b     191f;       /* Skip trampoline if endian is good  */ \
    .long 0xa600607d; /* mfmsr r11                          */ \
    .long 0x01006b69; /* xori r11,r11,1                     */ \
    .long 0x05009f42; /* bcl 20,31,$+4                      */ \
    .long 0xa602487d; /* mflr r10                           */ \
    .long 0x14004a39; /* addi r10,r10,20                    */ \
    .long 0xa64b5a7d; /* mthsrr0 r10                        */ \
    .long 0xa64b7b7d; /* mthsrr1 r11                        */ \
    .long 0x2402004c; /* hrfid                              */ \
    191:

/* Load an immediate 64-bit value into a register */
#define LOAD_IMM64(r, e)                \
    lis     r,(e)@highest;              \
    ori     r,r,(e)@higher;             \
    rldicr  r,r, 32, 31;                \
    oris    r,r, (e)@h;                 \
    ori     r,r, (e)@l;

.section ".head","ax"

/*
 * Microwatt comes in at 0 as little endian so we do not need to worry up
 * FIXUP_ENDIAN.
 */
    . = 0
    .global _start
_start:
    b       boot_entry

/* QEMU comes in at 0x10. Put a value in argc/r3 to distinguish from
 * microwatt. */
    . = 0x10
    FIXUP_ENDIAN
    LOAD_IMM64(%r3, 1)
    b       boot_entry

    .global boot_entry
    boot_entry:
    /* Save R3 to non-volatile register */
    mr      %r14, %r3
    restart:
    /*
     * setup stack with a safety gap, since we might write to the
     * previous frame.
     */
    LOAD_IMM64(%r1, STACK_TOP - 0x100)
    LOAD_IMM64(%r12, main)
    mtctr   %r12
    bctrl

    /* On exit, restart */
    mr      %r3, %r14
    b       restart

#define EXCEPTION(nr)       \
    .= nr;                  \
b   .

    /* More exception stubs */
    EXCEPTION(0x300)
    EXCEPTION(0x380)
    EXCEPTION(0x400)
    EXCEPTION(0x480)
    EXCEPTION(0x500)
    EXCEPTION(0x600)
    EXCEPTION(0x700)
    EXCEPTION(0x800)
    EXCEPTION(0x900)
    EXCEPTION(0x980)
    EXCEPTION(0xa00)
    EXCEPTION(0xb00)
    EXCEPTION(0xc00)
    EXCEPTION(0xd00)
    EXCEPTION(0xe00)
    EXCEPTION(0xe20)
    EXCEPTION(0xe40)
    EXCEPTION(0xe60)
    EXCEPTION(0xe80)
    EXCEPTION(0xf00)
    EXCEPTION(0xf20)
    EXCEPTION(0xf40)
    EXCEPTION(0xf60)
    EXCEPTION(0xf80)
    EXCEPTION(0x1000)
    EXCEPTION(0x1100)
    EXCEPTION(0x1200)
    EXCEPTION(0x1300)
    EXCEPTION(0x1400)
    EXCEPTION(0x1500)
    EXCEPTION(0x1600)
