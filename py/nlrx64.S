/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#if defined(__x86_64__) && !MICROPY_NLR_SETJMP

// We only need the functions here if we are on x86-64, and we are not
// using setjmp/longjmp.
//
// For reference, x86-64 callee save regs are:
//      rbx, rbp, rsp, r12, r13, r14, r15

// the offset of nlr_top within mp_state_ctx_t
#define NLR_TOP_OFFSET (2 * 8)

#if defined(__APPLE__) && defined(__MACH__)
#define NLR_TOP (_mp_state_ctx + NLR_TOP_OFFSET)
#define MP_THREAD_GET_STATE _mp_thread_get_state
#else
#define NLR_TOP (mp_state_ctx + NLR_TOP_OFFSET)
#define MP_THREAD_GET_STATE mp_thread_get_state
#endif

// offset of nlr_top within mp_state_thread_t structure
#define NLR_TOP_TH_OFF (0)

#if defined(_WIN32) || defined(__CYGWIN__)
#define NLR_OS_WINDOWS
#endif

    .file   "nlr.s"
    .text

#if !defined(NLR_OS_WINDOWS)

/******************************************************************************/
//
// Functions for *nix and OSX.
// OSX needs _ prefix for binding to C, and doesn't support some directives.
//
/******************************************************************************/

/**************************************/
// mp_uint_t nlr_push(rdi=nlr_buf_t *nlr)

#if !(defined(__APPLE__) && defined(__MACH__))
    .globl  nlr_push
    .type   nlr_push, @function
nlr_push:
#else
    .globl  _nlr_push
_nlr_push:
#endif
    movq    (%rsp), %rax            # load return %rip
    movq    %rax, 16(%rdi)          # store %rip into nlr_buf
    movq    %rbp, 24(%rdi)          # store %rbp into nlr_buf
    movq    %rsp, 32(%rdi)          # store %rsp into nlr_buf
    movq    %rbx, 40(%rdi)          # store %rbx into nlr_buf
    movq    %r12, 48(%rdi)          # store %r12 into nlr_buf
    movq    %r13, 56(%rdi)          # store %r13 into nlr_buf
    movq    %r14, 64(%rdi)          # store %r14 into nlr_buf
    movq    %r15, 72(%rdi)          # store %r15 into nlr_buf

#if !MICROPY_PY_THREAD
    movq    NLR_TOP(%rip), %rax     # get last nlr_buf
    movq    %rax, (%rdi)            # store it
    movq    %rdi, NLR_TOP(%rip)     # stor new nlr_buf (to make linked list)
#else
    movq    %rdi, %rbp              # since we make a call, must save rdi in rbp
    callq   MP_THREAD_GET_STATE     # get mp_state_thread ptr into rax
    movq    NLR_TOP_TH_OFF(%rax), %rsi # get thread.nlr_top (last nlr_buf)
    movq    %rsi, (%rbp)            # store it
    movq    %rbp, NLR_TOP_TH_OFF(%rax) # store new nlr_buf (to make linked list)
    movq    24(%rbp), %rbp          # restore rbp
#endif

    xorq    %rax, %rax              # return 0, normal return
    ret                             # return
#if !(defined(__APPLE__) && defined(__MACH__))
    .size   nlr_push, .-nlr_push
#endif

/**************************************/
// void nlr_pop()

#if !(defined(__APPLE__) && defined(__MACH__))
    .globl  nlr_pop
    .type   nlr_pop, @function
nlr_pop:
#else
    .globl  _nlr_pop
_nlr_pop:
#endif

#if !MICROPY_PY_THREAD
    movq    NLR_TOP(%rip), %rax     # get nlr_top into %rax
    movq    (%rax), %rax            # load prev nlr_buf
    movq    %rax, NLR_TOP(%rip)     # store prev nlr_buf (to unlink list)
#else
    callq   MP_THREAD_GET_STATE     # get mp_state_thread ptr into rax
    movq    NLR_TOP_TH_OFF(%rax), %rdi # get thread.nlr_top (last nlr_buf)
    movq    (%rdi), %rdi            # load prev nlr_buf
    movq    %rdi, NLR_TOP_TH_OFF(%rax) # store prev nlr_buf (to unlink list)
#endif

    ret                             # return
#if !(defined(__APPLE__) && defined(__MACH__))
    .size   nlr_pop, .-nlr_pop
#endif

/**************************************/
// void nlr_jump(rdi=mp_uint_t val)

#if !(defined(__APPLE__) && defined(__MACH__))
    .globl  nlr_jump
    .type   nlr_jump, @function
nlr_jump:
#else
    .globl  _nlr_jump
    _nlr_jump:
#endif

#if !MICROPY_PY_THREAD
    movq    %rdi, %rax              # put return value in %rax
    movq    NLR_TOP(%rip), %rdi     # get nlr_top into %rdi
    test    %rdi, %rdi              # check for nlr_top being NULL
    je      .fail                   # fail if nlr_top is NULL
    movq    %rax, 8(%rdi)           # store return value
    movq    (%rdi), %rax            # load prev nlr_buf
    movq    %rax, NLR_TOP(%rip)     # store prev nlr_buf (to unlink list)
#else
    movq    %rdi, %rbp              # put return value in rbp
    callq   MP_THREAD_GET_STATE     # get thread ptr in rax
    movq    %rax, %rsi              # put thread ptr in rsi
    movq    %rbp, %rax              # put return value to rax (for je .fail)
    movq    NLR_TOP_TH_OFF(%rsi), %rdi # get thread.nlr_top in rdi
    test    %rdi, %rdi              # check for nlr_top being NULL
    je      .fail                   # fail if nlr_top is NULL
    movq    %rax, 8(%rdi)           # store return value
    movq    (%rdi), %rax            # load prev nlr_buf
    movq    %rax, NLR_TOP_TH_OFF(%rsi) # store prev nlr_buf (to unlink list)
#endif

    movq    72(%rdi), %r15          # load saved %r15
    movq    64(%rdi), %r14          # load saved %r14
    movq    56(%rdi), %r13          # load saved %r13
    movq    48(%rdi), %r12          # load saved %r12
    movq    40(%rdi), %rbx          # load saved %rbx
    movq    32(%rdi), %rsp          # load saved %rsp
    movq    24(%rdi), %rbp          # load saved %rbp
    movq    16(%rdi), %rax          # load saved %rip
    movq    %rax, (%rsp)            # store saved %rip to stack
    xorq    %rax, %rax              # clear return register
    inc     %al                     # increase to make 1, non-local return
    ret                             # return
.fail:
    movq    %rax, %rdi              # put argument back in first-arg register
#if !(defined(__APPLE__) && defined(__MACH__))
    je      nlr_jump_fail           # transfer control to nlr_jump_fail
    .size   nlr_jump, .-nlr_jump
#else
    je      _nlr_jump_fail          # transfer control to nlr_jump_fail
#endif

#else // !defined(NLR_OS_WINDOWS)

/******************************************************************************/
//
// Functions for Windows
//
/******************************************************************************/

/**************************************/
// mp_uint_t nlr_push(rcx=nlr_buf_t *nlr)

    .globl  nlr_push
nlr_push:
    movq    (%rsp), %rax            # load return %rip
    movq    %rax, 16(%rcx)          # store %rip into nlr_buf
    movq    %rbp, 24(%rcx)          # store %rbp into nlr_buf
    movq    %rsp, 32(%rcx)          # store %rsp into nlr_buf
    movq    %rbx, 40(%rcx)          # store %rbx into nlr_buf
    movq    %r12, 48(%rcx)          # store %r12 into nlr_buf
    movq    %r13, 56(%rcx)          # store %r13 into nlr_buf
    movq    %r14, 64(%rcx)          # store %r14 into nlr_buf
    movq    %r15, 72(%rcx)          # store %r15 into
    movq    %rdi, 80(%rcx)          # store %rdr into
    movq    %rsi, 88(%rcx)          # store %rsi into
    movq    NLR_TOP(%rip), %rax     # get last nlr_buf
    movq    %rax, (%rcx)            # store it
    movq    %rcx, NLR_TOP(%rip)     # stor new nlr_buf (to make linked list)
    xorq    %rax, %rax              # return 0, normal return
    ret                             # return

/**************************************/
// void nlr_pop()

    .globl  nlr_pop
nlr_pop:
    movq    NLR_TOP(%rip), %rax     # get nlr_top into %rax
    movq    (%rax), %rax            # load prev nlr_buf
    movq    %rax, NLR_TOP(%rip)     # store prev nlr_buf (to unlink list)
    ret                             # return

/**************************************/
// void nlr_jump(rcx=mp_uint_t val)

    .globl  nlr_jump
nlr_jump:
    movq    %rcx, %rax              # put return value in %rax
    movq    NLR_TOP(%rip), %rcx     # get nlr_top into %rcx
    test    %rcx, %rcx              # check for nlr_top being NULL
    je      .fail                   # fail if nlr_top is NULL
    movq    %rax, 8(%rcx)           # store return value
    movq    (%rcx), %rax            # load prev nlr_buf
    movq    %rax, NLR_TOP(%rip)     # store prev nlr_buf (to unlink list)
    movq    72(%rcx), %r15          # load saved %r15
    movq    64(%rcx), %r14          # load saved %r14
    movq    56(%rcx), %r13          # load saved %r13
    movq    48(%rcx), %r12          # load saved %r12
    movq    40(%rcx), %rbx          # load saved %rbx
    movq    32(%rcx), %rsp          # load saved %rsp
    movq    24(%rcx), %rbp          # load saved %rbp
    movq    16(%rcx), %rax          # load saved %rip
    movq    80(%rcx), %rdi          # store %rdr into
    movq    88(%rcx), %rsi          # store %rsi into
    movq    %rax, (%rsp)            # store saved %rip to stack
    xorq    %rax, %rax              # clear return register
    inc     %al                     # increase to make 1, non-local return
    ret                             # return
.fail:
    movq    %rax, %rcx              # put argument back in first-arg register
    je      nlr_jump_fail           # transfer control to nlr_jump_fail

#endif // !defined(NLR_OS_WINDOWS)

#endif // defined(__x86_64__) && !MICROPY_NLR_SETJMP
#if defined(linux)
    .section    .note.GNU-stack,"",%progbits
#endif
