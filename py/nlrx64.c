/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2017 Damien P. George
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

#if MICROPY_NLR_X64

#undef nlr_push

// x86-64 callee-save registers are:
//  rbx, rbp, rsp, r12, r13, r14, r15

__attribute__((used)) unsigned int nlr_push_tail(nlr_buf_t *nlr);

#if !MICROPY_NLR_OS_WINDOWS
#if defined(__clang__) || (defined(__GNUC__) && __GNUC__ >= 8)
#define USE_NAKED 1
#else
// On older gcc the equivalent here is to force omit-frame-pointer
__attribute__((optimize("omit-frame-pointer")))
#endif
#endif

#if !defined(USE_NAKED)
#define USE_NAKED 0
#endif

#if USE_NAKED
// nlr_push prelude should never push frame pointer register ebp onto the stack
__attribute__((naked))
#endif
unsigned int nlr_push(nlr_buf_t *nlr) {
    #if !USE_NAKED
    (void)nlr;
    #endif

    #if MICROPY_NLR_OS_WINDOWS

    __asm volatile (
        "movq   (%rsp), %rax        \n" // load return %rip
        "movq   %rax, 16(%rcx)      \n" // store %rip into nlr_buf
        "movq   %rbp, 24(%rcx)      \n" // store %rbp into nlr_buf
        "movq   %rsp, 32(%rcx)      \n" // store %rsp into nlr_buf
        "movq   %rbx, 40(%rcx)      \n" // store %rbx into nlr_buf
        "movq   %r12, 48(%rcx)      \n" // store %r12 into nlr_buf
        "movq   %r13, 56(%rcx)      \n" // store %r13 into nlr_buf
        "movq   %r14, 64(%rcx)      \n" // store %r14 into nlr_buf
        "movq   %r15, 72(%rcx)      \n" // store %r15 into nlr_buf
        "movq   %rdi, 80(%rcx)      \n" // store %rdr into nlr_buf
        "movq   %rsi, 88(%rcx)      \n" // store %rsi into nlr_buf
        "jmp    nlr_push_tail       \n" // do the rest in C
        );

    #else

    __asm volatile (
        "movq   (%rsp), %rax        \n" // load return %rip
        "movq   %rax, 16(%rdi)      \n" // store %rip into nlr_buf
        "movq   %rbp, 24(%rdi)      \n" // store %rbp into nlr_buf
        "movq   %rsp, 32(%rdi)      \n" // store %rsp into nlr_buf
        "movq   %rbx, 40(%rdi)      \n" // store %rbx into nlr_buf
        "movq   %r12, 48(%rdi)      \n" // store %r12 into nlr_buf
        "movq   %r13, 56(%rdi)      \n" // store %r13 into nlr_buf
        "movq   %r14, 64(%rdi)      \n" // store %r14 into nlr_buf
        "movq   %r15, 72(%rdi)      \n" // store %r15 into nlr_buf
        #if defined(__APPLE__) && defined(__MACH__)
        "jmp    _nlr_push_tail      \n" // do the rest in C
        #else
        "jmp    nlr_push_tail       \n" // do the rest in C
        #endif
        );

    #endif

    #if !USE_NAKED
    return 0; // needed to silence compiler warning
    #endif
}

NORETURN void nlr_jump(void *val) {
    MP_NLR_JUMP_HEAD(val, top)

    __asm volatile (
        "movq   %0, %%rcx           \n" // %rcx points to nlr_buf
        #if MICROPY_NLR_OS_WINDOWS
        "movq   88(%%rcx), %%rsi    \n" // load saved %rsi
        "movq   80(%%rcx), %%rdi    \n" // load saved %rdi
        #endif
        "movq   72(%%rcx), %%r15    \n" // load saved %r15
        "movq   64(%%rcx), %%r14    \n" // load saved %r14
        "movq   56(%%rcx), %%r13    \n" // load saved %r13
        "movq   48(%%rcx), %%r12    \n" // load saved %r12
        "movq   40(%%rcx), %%rbx    \n" // load saved %rbx
        "movq   32(%%rcx), %%rsp    \n" // load saved %rsp
        "movq   24(%%rcx), %%rbp    \n" // load saved %rbp
        "movq   16(%%rcx), %%rax    \n" // load saved %rip
        "movq   %%rax, (%%rsp)      \n" // store saved %rip to stack
        "xorq   %%rax, %%rax        \n" // clear return register
        "inc    %%al                \n" // increase to make 1, non-local return
        "ret                        \n" // return
        :                           // output operands
        : "r" (top)                 // input operands
        : "memory"                  // clobbered registers
        );

    MP_UNREACHABLE
}

#endif // MICROPY_NLR_X64
