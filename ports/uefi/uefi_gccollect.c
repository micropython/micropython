/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Nicko van Someren
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

// Conservative-GC root capture for the UEFI port.
//
// We dump the ABI's callee-saved general-purpose registers (which may hold the
// only reference to a live object up the call chain) into a buffer, then scan
// that buffer plus the C stack down to MP_STATE_THREAD(stack_top).
//
// The register dump is a top-level global-asm function rather than the shared
// gchelper_generic.c: clang ICEs on that file's `register ... asm("rbx")` named
// register variables for the x86_64-windows (MS ABI) target. Doing it explicitly
// also lets us capture the *Microsoft* callee-saved set on x64 (which includes
// RDI/RSI, unlike the System V set the generic helper assumes).
//
// XMM6–XMM15 (x64) are not scanned: a GC pointer living only in an XMM register
// would be missed. Scanning general-purpose registers only is the standard
// conservative-GC assumption and is fine in practice.

#include <stdint.h>
#include "py/mpstate.h"
#include "py/gc.h"

#if MICROPY_ENABLE_GC

#if defined(__x86_64__)
// Microsoft x64 callee-saved GP set: RBX RBP RDI RSI R12 R13 R14 R15.
#define MP_UEFI_NUM_REGS 8
// void mp_uefi_get_regs(void **regs);  arg (regs) in RCX.
__asm__ (
    ".text\n"
    ".globl mp_uefi_get_regs\n"
    "mp_uefi_get_regs:\n"
    "  movq %rbx,  0(%rcx)\n"
    "  movq %rbp,  8(%rcx)\n"
    "  movq %rdi, 16(%rcx)\n"
    "  movq %rsi, 24(%rcx)\n"
    "  movq %r12, 32(%rcx)\n"
    "  movq %r13, 40(%rcx)\n"
    "  movq %r14, 48(%rcx)\n"
    "  movq %r15, 56(%rcx)\n"
    "  ret\n"
    );
#elif defined(__aarch64__)
// AAPCS64 callee-saved GP set: x19–x28, plus FP (x29) and LR (x30).
#define MP_UEFI_NUM_REGS 12
// void mp_uefi_get_regs(void **regs);  arg (regs) in x0.
__asm__ (
    ".text\n"
    ".globl mp_uefi_get_regs\n"
    "mp_uefi_get_regs:\n"
    "  stp x19, x20, [x0, #0]\n"
    "  stp x21, x22, [x0, #16]\n"
    "  stp x23, x24, [x0, #32]\n"
    "  stp x25, x26, [x0, #48]\n"
    "  stp x27, x28, [x0, #64]\n"
    "  stp x29, x30, [x0, #80]\n"
    "  ret\n"
    );
#else
#error "uefi_gccollect: unsupported architecture"
#endif

void mp_uefi_get_regs(void **regs);

void gc_collect(void) {
    gc_collect_start();

    // Callee-saved registers.
    void *regs[MP_UEFI_NUM_REGS];
    mp_uefi_get_regs(regs);
    gc_collect_root(regs, MP_UEFI_NUM_REGS);

    // C stack, from here up to the recorded top.
    void *sp = (void *)&sp;
    gc_collect_root(sp, ((uintptr_t)MP_STATE_THREAD(stack_top) - (uintptr_t)sp) / sizeof(uintptr_t));

    gc_collect_end();
}

#endif // MICROPY_ENABLE_GC
