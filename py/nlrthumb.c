/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2016 Damien P. George
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
#include "py/nlr.h"

#if (!defined(MICROPY_NLR_SETJMP) || !MICROPY_NLR_SETJMP) && (defined(__thumb2__) || defined(__thumb__) || defined(__arm__))

#undef nlr_push

// We only need the functions here if we are on arm/thumb, and we are not
// using setjmp/longjmp.
//
// For reference, arm/thumb callee save regs are:
//      r4-r11, r13=sp

__attribute__((naked)) unsigned int nlr_push(nlr_buf_t *nlr) {

    __asm volatile (
    "str    r4, [r0, #12]       \n" // store r4 into nlr_buf
    "str    r5, [r0, #16]       \n" // store r5 into nlr_buf
    "str    r6, [r0, #20]       \n" // store r6 into nlr_buf
    "str    r7, [r0, #24]       \n" // store r7 into nlr_buf

#if defined(__ARM_ARCH_6M__)
    "mov    r1, r8              \n"
    "str    r1, [r0, #28]       \n" // store r8 into nlr_buf
    "mov    r1, r9              \n"
    "str    r1, [r0, #32]       \n" // store r9 into nlr_buf
    "mov    r1, r10             \n"
    "str    r1, [r0, #36]       \n" // store r10 into nlr_buf
    "mov    r1, r11             \n"
    "str    r1, [r0, #40]       \n" // store r11 into nlr_buf
    "mov    r1, r13             \n"
    "str    r1, [r0, #44]       \n" // store r13=sp into nlr_buf
    "mov    r1, lr              \n"
    "str    r1, [r0, #8]        \n" // store lr into nlr_buf
#else
    "str    r8, [r0, #28]       \n" // store r8 into nlr_buf
    "str    r9, [r0, #32]       \n" // store r9 into nlr_buf
    "str    r10, [r0, #36]      \n" // store r10 into nlr_buf
    "str    r11, [r0, #40]      \n" // store r11 into nlr_buf
    "str    r13, [r0, #44]      \n" // store r13=sp into nlr_buf
    "str    lr, [r0, #8]        \n" // store lr into nlr_buf
#endif

#if defined(__ARM_ARCH_6M__)
    "ldr    r1, nlr_push_tail_var \n"
    "bx     r1                  \n" // do the rest in C
    ".align 2                   \n"
    "nlr_push_tail_var: .word nlr_push_tail \n"
#else
    "b      nlr_push_tail       \n" // do the rest in C
#endif
    );

    return 0; // needed to silence compiler warning
}

__attribute__((used)) unsigned int nlr_push_tail(nlr_buf_t *nlr) {
    nlr_buf_t **top = &MP_STATE_THREAD(nlr_top);
    nlr->prev = *top;
    *top = nlr;
    return 0; // normal return
}

void nlr_pop(void) {
    nlr_buf_t **top = &MP_STATE_THREAD(nlr_top);
    *top = (*top)->prev;
}

NORETURN __attribute__((naked)) void nlr_jump(void *val) {
    nlr_buf_t **top_ptr = &MP_STATE_THREAD(nlr_top);
    nlr_buf_t *top = *top_ptr;
    if (top == NULL) {
        nlr_jump_fail(val);
    }

    top->ret_val = val;
    *top_ptr = top->prev;

    __asm volatile (
    "mov    r0, %0              \n" // r0 points to nlr_buf
    "ldr    r4, [r0, #12]       \n" // load r4 from nlr_buf
    "ldr    r5, [r0, #16]       \n" // load r5 from nlr_buf
    "ldr    r6, [r0, #20]       \n" // load r6 from nlr_buf
    "ldr    r7, [r0, #24]       \n" // load r7 from nlr_buf

#if defined(__ARM_ARCH_6M__)
    "ldr    r1, [r0, #28]       \n" // load r8 from nlr_buf
    "mov    r8, r1              \n"
    "ldr    r1, [r0, #32]       \n" // load r9 from nlr_buf
    "mov    r9, r1              \n"
    "ldr    r1, [r0, #36]       \n" // load r10 from nlr_buf
    "mov    r10, r1             \n"
    "ldr    r1, [r0, #40]       \n" // load r11 from nlr_buf
    "mov    r11, r1             \n"
    "ldr    r1, [r0, #44]       \n" // load r13=sp from nlr_buf
    "mov    r13, r1             \n"
    "ldr    r1, [r0, #8]        \n" // load lr from nlr_buf
    "mov    lr, r1              \n"
#else
    "ldr    r8, [r0, #28]       \n" // load r8 from nlr_buf
    "ldr    r9, [r0, #32]       \n" // load r9 from nlr_buf
    "ldr    r10, [r0, #36]      \n" // load r10 from nlr_buf
    "ldr    r11, [r0, #40]      \n" // load r11 from nlr_buf
    "ldr    r13, [r0, #44]      \n" // load r13=sp from nlr_buf
    "ldr    lr, [r0, #8]        \n" // load lr from nlr_buf
#endif
    "movs   r0, #1              \n" // return 1, non-local return
    "bx     lr                  \n" // return
    :                               // output operands
    : "r"(top)                      // input operands
    :                               // clobbered registers
    );

    for (;;); // needed to silence compiler warning
}

#endif // (!defined(MICROPY_NLR_SETJMP) || !MICROPY_NLR_SETJMP) && (defined(__thumb2__) || defined(__thumb__) || defined(__arm__))
