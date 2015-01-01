/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
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

#if defined(__xtensa__)

/*
    calling conventions:
    a0 = return address
    a1 = stack pointer
    a2 = first arg, return value
    a3-a7 = rest of args
*/

// the offset of nlr_top within mp_state_ctx_t
#define NLR_TOP_OFFSET (2 * 4)

#define NLR_TOP (mp_state_ctx + NLR_TOP_OFFSET)

    .file   "nlr.s"
    .text

    .literal_position
    .literal .LC0, NLR_TOP
    .align  4
    .global nlr_push
    .type   nlr_push, @function
nlr_push:
    // save regs
    s32i.n  a0, a2, 8
    s32i.n  a1, a2, 12
    s32i.n  a8, a2, 16
    s32i.n  a9, a2, 20
    s32i.n  a10, a2, 24
    s32i.n  a11, a2, 28
    s32i.n  a12, a2, 32
    s32i.n  a13, a2, 36
    s32i.n  a14, a2, 40
    s32i.n  a15, a2, 44

    l32r    a3, .LC0
    l32i.n  a4, a3, 0
    s32i.n  a2, a3, 0
    s32i.n  a4, a2, 0
    movi.n  a2, 0
    ret.n
    .size   nlr_push, .-nlr_push

    .literal_position
    .literal .LC1, NLR_TOP
    .align  4
    .global nlr_pop
    .type   nlr_pop, @function
nlr_pop:
    l32r    a2, .LC1
    l32i.n  a3, a2, 0
    l32i.n  a3, a3, 0
    s32i.n  a3, a2, 0
    ret.n
    .size   nlr_pop, .-nlr_pop

    .literal_position
    .literal .LC2, NLR_TOP
    .align    4
    .global    nlr_jump
    .type    nlr_jump, @function
nlr_jump:
    l32r    a3, .LC2
    l32i.n  a3, a3, 0   // a3 = nlr_top
    bnez.n  a3, .L4
    call0   nlr_jump_fail
.L4:
    s32i.n  a2, a3, 4   // nlr_top->ret_val = val

    // restore regs
    l32i.n  a0, a3, 8
    l32i.n  a1, a3, 12
    l32i.n  a8, a3, 16
    l32i.n  a9, a3, 20
    l32i.n  a10, a3, 24
    l32i.n  a11, a3, 28
    l32i.n  a12, a3, 32
    l32i.n  a13, a3, 36
    l32i.n  a14, a3, 40
    l32i.n  a15, a3, 44

    l32i.n  a3, a3, 0   // a3 = nlr_top->prev
    l32r    a2, .LC2
    s32i.n  a3, a2, 0   // nlr_top = a3
    movi.n  a2, 1       // return 1
    ret.n
    .size   nlr_jump, .-nlr_jump

#endif // defined(__xtensa__)
