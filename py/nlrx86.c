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

#if MICROPY_NLR_X86

#undef nlr_push

// For reference, x86 callee save regs are:
//  ebx, esi, edi, ebp, esp, eip

#if MICROPY_NLR_OS_WINDOWS && defined(_MSC_VER)
unsigned int nlr_push_tail(nlr_buf_t *nlr);
#elif MICROPY_NLR_OS_WINDOWS
unsigned int nlr_push_tail(nlr_buf_t *nlr) asm ("nlr_push_tail");
#else
__attribute__((used)) unsigned int nlr_push_tail(nlr_buf_t *nlr);
#endif

#if !defined(__clang__) && defined(__GNUC__) && __GNUC__ >= 8
// Since gcc 8.0 the naked attribute is supported
#define USE_NAKED (1)
#define UNDO_PRELUDE (0)
#elif defined(__ZEPHYR__) || defined(__ANDROID__)
// Zephyr and Android use a different calling convention by default
#define USE_NAKED (0)
#define UNDO_PRELUDE (0)
#else
#define USE_NAKED (0)
#define UNDO_PRELUDE (1)
#endif

#ifndef _MSC_VER

#if USE_NAKED
__attribute__((naked))
#endif
unsigned int nlr_push(nlr_buf_t *nlr) {
    (void)nlr;

    __asm volatile (
        #if UNDO_PRELUDE
        "pop    %ebp                \n" // undo function's prelude
        #endif
        "mov    4(%esp), %edx       \n" // load nlr_buf
        "mov    (%esp), %eax        \n" // load return %eip
        "mov    %eax, 8(%edx)       \n" // store %eip into nlr_buf
        "mov    %ebp, 12(%edx)      \n" // store %ebp into nlr_buf
        "mov    %esp, 16(%edx)      \n" // store %esp into nlr_buf
        "mov    %ebx, 20(%edx)      \n" // store %ebx into nlr_buf
        "mov    %edi, 24(%edx)      \n" // store %edi into nlr_buf
        "mov    %esi, 28(%edx)      \n" // store %esi into nlr_buf
        "jmp    nlr_push_tail       \n" // do the rest in C
        );

    #if !USE_NAKED
    return 0; // needed to silence compiler warning
    #endif
}

NORETURN void nlr_jump(void *val) {
    MP_NLR_JUMP_HEAD(val, top)

    __asm volatile (
        "mov    %0, %%edx           \n" // %edx points to nlr_buf
        "mov    28(%%edx), %%esi    \n" // load saved %esi
        "mov    24(%%edx), %%edi    \n" // load saved %edi
        "mov    20(%%edx), %%ebx    \n" // load saved %ebx
        "mov    16(%%edx), %%esp    \n" // load saved %esp
        "mov    12(%%edx), %%ebp    \n" // load saved %ebp
        "mov    8(%%edx), %%eax     \n" // load saved %eip
        "mov    %%eax, (%%esp)      \n" // store saved %eip to stack
        "xor    %%eax, %%eax        \n" // clear return register
        "inc    %%al                \n" // increase to make 1, non-local return
        "ret                        \n" // return
        :                           // output operands
        : "r" (top)                 // input operands
        :                           // clobbered registers
        );

    MP_UNREACHABLE
}

#else

/* *FORMAT-OFF* */
#pragma warning(push)
#pragma warning(disable:4731)

unsigned int __declspec(naked) nlr_push(nlr_buf_t* nlr) {
    __asm {
        mov     ecx, [esp + 4]      ; load nlr
        mov     eax, [esp]          ; load return eip
        mov     [ecx +  8], eax     ; store eip into nlr_buf
        mov     [ecx + 12], ebp     ; store ebp into nlr_buf
        mov     [ecx + 16], esp     ; store esp into nlr_buf
        mov     [ecx + 20], ebx     ; store ebx into nlr_buf
        mov     [ecx + 24], edi     ; store edi into nlr_buf
        mov     [ecx + 28], esi     ; store esi into nlr_buf
        jmp     nlr_push_tail       ; do the rest in C
    };
}

void nlr_jump(nlr_buf_t* val) {
    MP_NLR_JUMP_HEAD(val, top)

    __asm {
        mov     ecx, top
        mov     esi, [ecx + 28]     ; load saved esi from nlr_buf
        mov     edi, [ecx + 24]     ; load saved edi from nlr_buf
        mov     ebx, [ecx + 20]     ; load saved ebx from nlr_buf
        mov     esp, [ecx + 16]     ; load saved esp from nlr_buf
        mov     ebp, [ecx + 12]     ; load saved ebp from nlr_buf
        mov     eax, [ecx +  8]     ; load saved eip from nlr_buf
        mov     [esp], eax          ; store return eip to stack
        mov     [esp + 4], ecx      ; store nlr to stack
        xor     eax, eax            ; clear return register
        inc     al                  ; increase to make 1, non - local return
        ret
    }
}

#pragma warning(pop)
/* *FORMAT-ON* */

#endif // _MSC_VER

#endif // MICROPY_NLR_X86
