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

#include "py/mpstate.h"

#if MICROPY_NLR_POWERPC

#undef nlr_push

// Saving all ABI non-vol registers here

unsigned int nlr_push(nlr_buf_t *nlr) {

    __asm__ volatile(
    "li     4, 0x4eed ; "      // Store canary
    "std    4,  0x00(%0) ;"
    "std    0,  0x08(%0) ;"
    "std    1,  0x10(%0) ;"
    "std    2,  0x18(%0) ;"
    "std    14, 0x20(%0) ;"
    "std    15, 0x28(%0) ;"
    "std    16, 0x30(%0) ;"
    "std    17, 0x38(%0) ;"
    "std    18, 0x40(%0) ;"
    "std    19, 0x48(%0) ;"
    "std    20, 0x50(%0) ;"
    "std    21, 0x58(%0) ;"
    "std    22, 0x60(%0) ;"
    "std    23, 0x68(%0) ;"
    "std    24, 0x70(%0) ;"
    "std    25, 0x78(%0) ;"
    "std    26, 0x80(%0) ;"
    "std    27, 0x88(%0) ;"
    "std    28, 0x90(%0) ;"
    "std    29, 0x98(%0) ;"
    "std    30, 0xA0(%0) ;"
    "std    31, 0xA8(%0) ;"

    "mfcr   4 ; "
    "std    4, 0xB0(%0) ;"
    "mflr   4 ;"
    "std    4, 0xB8(%0) ;"
    "li     4, nlr_push_tail@l ;"
    "oris   4, 4, nlr_push_tail@h ;"
    "mtctr  4 ;"
    "mr    3, %1 ; "
    "bctr  ;"
    :
    : "r"(&nlr->regs), "r"(nlr)
    :
    );

    return 0;
}

NORETURN void nlr_jump(void *val) {
    MP_NLR_JUMP_HEAD(val, top)

    __asm__ volatile(
    "ld    3, 0x0(%0) ;"
    "cmpdi 3, 0x4eed ; " // Check canary
    "bne   . ; "
    "ld    0,  0x08(%0) ;"
    "ld    1,  0x10(%0) ;"
    "ld    2,  0x18(%0) ;"
    "ld    14, 0x20(%0) ;"
    "ld    15, 0x28(%0) ;"
    "ld    16, 0x30(%0) ;"
    "ld    17, 0x38(%0) ;"
    "ld    18, 0x40(%0) ;"
    "ld    19, 0x48(%0) ;"
    "ld    20, 0x50(%0) ;"
    "ld    21, 0x58(%0) ;"
    "ld    22, 0x60(%0) ;"
    "ld    23, 0x68(%0) ;"
    "ld    24, 0x70(%0) ;"
    "ld    25, 0x78(%0) ;"
    "ld    26, 0x80(%0) ;"
    "ld    27, 0x88(%0) ;"
    "ld    28, 0x90(%0) ;"
    "ld    29, 0x98(%0) ;"
    "ld    30, 0xA0(%0) ;"
    "ld    31, 0xA8(%0) ;"
    "ld    3,  0xB0(%0) ;"
    "mtcr  3 ;"
    "ld    3, 0xB8(%0) ;"
    "mtlr  3 ; "
    "li    3, 1;"
    "blr ;"
    :
    : "r"(&top->regs)
    :
    );

    MP_UNREACHABLE;
}

#endif // MICROPY_NLR_POWERPC
