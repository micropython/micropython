/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Yonatan Goldschmidt
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

#include "py/mpstate.h" // needed for NLR defs

#if MICROPY_NLR_AARCH64

// AArch64 callee-saved registers are x19-x29.
// https://en.wikipedia.org/wiki/Calling_convention#ARM_(A64)

// Implemented purely as inline assembly; inside a function, we have to deal with undoing the prologue, restoring
// SP and LR. This way, we don't.
__asm(
    #if defined(__APPLE__) && defined(__MACH__)
    "_nlr_push:              \n"
    ".global _nlr_push       \n"
    #else
    "nlr_push:               \n"
    ".global nlr_push        \n"
    #endif
    "mov x9, sp              \n"
    "stp lr,  x9,  [x0,  #16]\n" // 16 == offsetof(nlr_buf_t, regs)
    "stp x19, x20, [x0,  #32]\n"
    "stp x21, x22, [x0,  #48]\n"
    "stp x23, x24, [x0,  #64]\n"
    "stp x25, x26, [x0,  #80]\n"
    "stp x27, x28, [x0,  #96]\n"
    "str x29,      [x0, #112]\n"
    #if defined(__APPLE__) && defined(__MACH__)
    "b _nlr_push_tail        \n" // do the rest in C
    #else
    "b nlr_push_tail         \n" // do the rest in C
    #endif
    );

NORETURN void nlr_jump(void *val) {
    MP_NLR_JUMP_HEAD(val, top)

    MP_STATIC_ASSERT(offsetof(nlr_buf_t, regs) == 16); // asm assumes it

    __asm volatile (
        "ldr x29,      [%0, #112]\n"
        "ldp x27, x28, [%0,  #96]\n"
        "ldp x25, x26, [%0,  #80]\n"
        "ldp x23, x24, [%0,  #64]\n"
        "ldp x21, x22, [%0,  #48]\n"
        "ldp x19, x20, [%0,  #32]\n"
        "ldp lr,  x9,  [%0,  #16]\n" // 16 == offsetof(nlr_buf_t, regs)
        "mov sp, x9              \n"
        "mov x0, #1              \n"  // non-local return
        "ret                     \n"
        :
        : "r" (top)
        :
        );

    MP_UNREACHABLE
}

#endif // MICROPY_NLR_AARCH64
