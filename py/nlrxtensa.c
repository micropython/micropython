/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-2017 Damien P. George
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

#if MICROPY_NLR_XTENSA

#undef nlr_push

// Xtensa calling conventions:
//  a0 = return address
//  a1 = stack pointer
//  a2 = first arg, return value
//  a3-a7 = rest of args

unsigned int nlr_push(nlr_buf_t *nlr) {

    __asm volatile (
        "s32i.n  a0, a2, 8          \n" // save regs...
        "s32i.n  a1, a2, 12         \n"
        "s32i.n  a8, a2, 16         \n"
        "s32i.n  a9, a2, 20         \n"
        "s32i.n  a10, a2, 24        \n"
        "s32i.n  a11, a2, 28        \n"
        "s32i.n  a12, a2, 32        \n"
        "s32i.n  a13, a2, 36        \n"
        "s32i.n  a14, a2, 40        \n"
        "s32i.n  a15, a2, 44        \n"
        "j      nlr_push_tail       \n" // do the rest in C
        );

    return 0; // needed to silence compiler warning
}

NORETURN void nlr_jump(void *val) {
    MP_NLR_JUMP_HEAD(val, top)

    __asm volatile (
        "mov.n   a2, %0             \n" // a2 points to nlr_buf
        "l32i.n  a0, a2, 8          \n" // restore regs...
        "l32i.n  a1, a2, 12         \n"
        "l32i.n  a8, a2, 16         \n"
        "l32i.n  a9, a2, 20         \n"
        "l32i.n  a10, a2, 24        \n"
        "l32i.n  a11, a2, 28        \n"
        "l32i.n  a12, a2, 32        \n"
        "l32i.n  a13, a2, 36        \n"
        "l32i.n  a14, a2, 40        \n"
        "l32i.n  a15, a2, 44        \n"
        "movi.n a2, 1               \n" // return 1, non-local return
        "ret.n                      \n" // return
        :                           // output operands
        : "r" (top)                 // input operands
        : "memory"                  // clobbered registers
        );

    MP_UNREACHABLE
}

#endif // MICROPY_NLR_XTENSA
