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

#if defined(__i386__) && !MICROPY_NLR_SETJMP

// We only need the functions here if we are on x86, and we are not
// using setjmp/longjmp.
//
// For reference, x86 callee save regs are:
//      ebx, esi, edi, ebp, esp, eip

// the offset of nlr_top within mp_state_ctx_t
#define NLR_TOP_OFFSET (2 * 4)

#if defined(_WIN32) || defined(__CYGWIN__)
#define NLR_OS_WINDOWS
#endif

#if defined(__APPLE__) && defined(__MACH__)
#define NLR_OS_MAC
#endif

#if defined(NLR_OS_WINDOWS) || defined(NLR_OS_MAC)
#define NLR_TOP (_mp_state_ctx + NLR_TOP_OFFSET)
#define MP_THREAD_GET_STATE _mp_thread_get_state
#else
#define NLR_TOP (mp_state_ctx + NLR_TOP_OFFSET)
#define MP_THREAD_GET_STATE mp_thread_get_state
#endif

// offset of nlr_top within mp_state_thread_t structure
#define NLR_TOP_TH_OFF (0)

    .file   "nlr.s"
    .text

/**************************************/
// mp_uint_t nlr_push(4(%esp)=nlr_buf_t *nlr)

#if defined(NLR_OS_WINDOWS)
    .globl  _nlr_push
    .def    _nlr_push; .scl 2; .type 32; .endef
_nlr_push:
#elif defined(NLR_OS_MAC)
    .globl  _nlr_push
_nlr_push:
#else
    .globl  nlr_push
    .type   nlr_push, @function
nlr_push:
#endif
    mov     4(%esp), %edx           # load nlr_buf
    mov     (%esp), %eax            # load return %ip
    mov     %eax, 8(%edx)           # store %ip into nlr_buf+8
    mov     %ebp, 12(%edx)          # store %bp into nlr_buf+12
    mov     %esp, 16(%edx)          # store %sp into nlr_buf+16
    mov     %ebx, 20(%edx)          # store %bx into nlr_buf+20
    mov     %edi, 24(%edx)          # store %di into nlr_buf
    mov     %esi, 28(%edx)          # store %si into nlr_buf

#if !MICROPY_PY_THREAD
    mov     NLR_TOP, %eax           # load nlr_top
    mov     %eax, (%edx)            # store it
    mov     %edx, NLR_TOP           # stor new nlr_buf (to make linked list)
#else
    // to check: stack is aligned to 16-byte boundary before this call
    call    MP_THREAD_GET_STATE     # get mp_state_thread ptr into eax
    mov     4(%esp), %edx           # load nlr_buf argument into edx (edx clobbered by call)
    mov     NLR_TOP_TH_OFF(%eax), %ecx # get thread.nlr_top (last nlr_buf)
    mov     %ecx, (%edx)            # store it
    mov     %edx, NLR_TOP_TH_OFF(%eax) # store new nlr_buf (to make linked list)
#endif

    xor     %eax, %eax              # return 0, normal return
    ret                             # return
#if !defined(NLR_OS_WINDOWS) && !defined(NLR_OS_MAC)
    .size   nlr_push, .-nlr_push
#endif

/**************************************/
// void nlr_pop()

#if defined(NLR_OS_WINDOWS)
    .globl  _nlr_pop
    .def    _nlr_pop; .scl 2; .type 32; .endef
_nlr_pop:
#elif defined(NLR_OS_MAC)
    .globl  _nlr_pop
_nlr_pop:
#else
    .globl  nlr_pop
    .type   nlr_pop, @function
nlr_pop:
#endif

#if !MICROPY_PY_THREAD
    mov     NLR_TOP, %eax           # load nlr_top
    mov     (%eax), %eax            # load prev nlr_buf
    mov     %eax, NLR_TOP           # store nlr_top (to unlink list)
#else
    call    MP_THREAD_GET_STATE     # get mp_state_thread ptr into eax
    mov     NLR_TOP_TH_OFF(%eax), %ecx # get thread.nlr_top (last nlr_buf)
    mov     (%ecx), %ecx            # load prev nlr_buf
    mov     %ecx, NLR_TOP_TH_OFF(%eax) # store prev nlr_buf (to unlink list)
#endif

    ret                             # return
#if !defined(NLR_OS_WINDOWS) && !defined(NLR_OS_MAC)
    .size   nlr_pop, .-nlr_pop
#endif

/**************************************/
// void nlr_jump(4(%esp)=mp_uint_t val)

#if defined(NLR_OS_WINDOWS)
    .globl  _nlr_jump
    .def    _nlr_jump; .scl 2; .type 32; .endef
_nlr_jump:
#elif defined(NLR_OS_MAC)
    .globl  _nlr_jump
_nlr_jump:
#else
    .globl  nlr_jump
    .type   nlr_jump, @function
nlr_jump:
#endif

#if !MICROPY_PY_THREAD
    mov     NLR_TOP, %edx           # load nlr_top
    test    %edx, %edx              # check for nlr_top being NULL
#if defined(NLR_OS_WINDOWS) || defined(NLR_OS_MAC)
    je      _nlr_jump_fail           # fail if nlr_top is NULL
#else
    je      nlr_jump_fail           # fail if nlr_top is NULL
#endif
    mov     4(%esp), %eax           # load return value
    mov     %eax, 4(%edx)           # store return value
    mov     (%edx), %eax            # load prev nlr_top
    mov     %eax, NLR_TOP           # store nlr_top (to unlink list)
#else
    call    MP_THREAD_GET_STATE     # get mp_state_thread ptr into eax
    mov     NLR_TOP_TH_OFF(%eax), %edx # get thread.nlr_top (last nlr_buf)
    test    %edx, %edx              # check for nlr_top being NULL
#if defined(NLR_OS_WINDOWS) || defined(NLR_OS_MAC)
    je      _nlr_jump_fail          # fail if nlr_top is NULL
#else
    je      nlr_jump_fail           # fail if nlr_top is NULL
#endif
    mov     4(%esp), %ecx           # load return value
    mov     %ecx, 4(%edx)           # store return value
    mov     (%edx), %ecx            # load prev nlr_top
    mov     %ecx, NLR_TOP_TH_OFF(%eax) # store nlr_top (to unlink list)
#endif

    mov     28(%edx), %esi          # load saved %si
    mov     24(%edx), %edi          # load saved %di
    mov     20(%edx), %ebx          # load saved %bx
    mov     16(%edx), %esp          # load saved %sp
    mov     12(%edx), %ebp          # load saved %bp
    mov     8(%edx), %eax           # load saved %ip
    mov     %eax, (%esp)            # store saved %ip to stack
    xor     %eax, %eax              # clear return register
    inc     %al                     # increase to make 1, non-local return
    ret                             # return
#if !defined(NLR_OS_WINDOWS) && !defined(NLR_OS_MAC)
    .size   nlr_jump, .-nlr_jump
#endif

#endif // defined(__i386__) && !MICROPY_NLR_SETJMP
#if defined(linux)
    .section    .note.GNU-stack,"",%progbits
#endif
