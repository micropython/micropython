/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#include <stdio.h>

#include "py/mpstate.h"
#include "py/gc.h"
#include "shared/runtime/gchelper.h"

#if MICROPY_ENABLE_GC

// Even if we have specific support for an architecture, it is
// possible to force use of setjmp-based implementation.
#if !MICROPY_GCREGS_SETJMP

// We capture here callee-save registers, i.e. ones which may contain
// interesting values held there by our callers. It doesn't make sense
// to capture caller-saved registers, because they, well, put on the
// stack already by the caller.
#if defined(__x86_64__)

static void gc_helper_get_regs(gc_helper_regs_t arr) {
    register long rbx asm ("rbx");
    register long rbp asm ("rbp");
    register long r12 asm ("r12");
    register long r13 asm ("r13");
    register long r14 asm ("r14");
    register long r15 asm ("r15");
    #ifdef __clang__
    // TODO:
    // This is dirty workaround for Clang. It tries to get around
    // uncompliant (wrt to GCC) behavior of handling register variables.
    // Application of this patch here is random, and done only to unbreak
    // MacOS build. Better, cross-arch ways to deal with Clang issues should
    // be found.
    asm ("" : "=r" (rbx));
    asm ("" : "=r" (rbp));
    asm ("" : "=r" (r12));
    asm ("" : "=r" (r13));
    asm ("" : "=r" (r14));
    asm ("" : "=r" (r15));
    #endif
    arr[0] = rbx;
    arr[1] = rbp;
    arr[2] = r12;
    arr[3] = r13;
    arr[4] = r14;
    arr[5] = r15;
}

#elif defined(__i386__)

static void gc_helper_get_regs(gc_helper_regs_t arr) {
    register long ebx asm ("ebx");
    register long esi asm ("esi");
    register long edi asm ("edi");
    register long ebp asm ("ebp");
    #ifdef __clang__
    // TODO:
    // This is dirty workaround for Clang. It tries to get around
    // uncompliant (wrt to GCC) behavior of handling register variables.
    // Application of this patch here is random, and done only to unbreak
    // MacOS build. Better, cross-arch ways to deal with Clang issues should
    // be found.
    asm ("" : "=r" (ebx));
    asm ("" : "=r" (esi));
    asm ("" : "=r" (edi));
    asm ("" : "=r" (ebp));
    #endif
    arr[0] = ebx;
    arr[1] = esi;
    arr[2] = edi;
    arr[3] = ebp;
}

#elif defined(__thumb2__) || defined(__thumb__) || defined(__arm__)

// Fallback implementation, prefer gchelper_thumb1.s or gchelper_thumb2.s

static void gc_helper_get_regs(gc_helper_regs_t arr) {
    #ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wuninitialized"
    #endif
    register long r4 asm ("r4");
    register long r5 asm ("r5");
    register long r6 asm ("r6");
    register long r7 asm ("r7");
    register long r8 asm ("r8");
    register long r9 asm ("r9");
    register long r10 asm ("r10");
    register long r11 asm ("r11");
    register long r12 asm ("r12");
    register long r13 asm ("r13");
    arr[0] = r4;
    arr[1] = r5;
    arr[2] = r6;
    arr[3] = r7;
    arr[4] = r8;
    arr[5] = r9;
    arr[6] = r10;
    arr[7] = r11;
    arr[8] = r12;
    arr[9] = r13;
    #ifdef __clang__
    #pragma clang diagnostic pop
    #endif
}

#elif defined(__aarch64__)

static void gc_helper_get_regs(gc_helper_regs_t arr) {
    const register long x19 asm ("x19");
    const register long x20 asm ("x20");
    const register long x21 asm ("x21");
    const register long x22 asm ("x22");
    const register long x23 asm ("x23");
    const register long x24 asm ("x24");
    const register long x25 asm ("x25");
    const register long x26 asm ("x26");
    const register long x27 asm ("x27");
    const register long x28 asm ("x28");
    const register long x29 asm ("x29");
    arr[0] = x19;
    arr[1] = x20;
    arr[2] = x21;
    arr[3] = x22;
    arr[4] = x23;
    arr[5] = x24;
    arr[6] = x25;
    arr[7] = x26;
    arr[8] = x27;
    arr[9] = x28;
    arr[10] = x29;
}

#elif defined(__riscv) && (__riscv_xlen <= 64)

// Fallback implementation for RV32I and RV64I, prefer gchelper_rv32i.s
// for RV32I targets or gchelper_rv64i.s for RV64I targets.

static void gc_helper_get_regs(gc_helper_regs_t arr) {
    register uintptr_t s0 asm ("x8");
    register uintptr_t s1 asm ("x9");
    register uintptr_t s2 asm ("x18");
    register uintptr_t s3 asm ("x19");
    register uintptr_t s4 asm ("x20");
    register uintptr_t s5 asm ("x21");
    register uintptr_t s6 asm ("x22");
    register uintptr_t s7 asm ("x23");
    register uintptr_t s8 asm ("x24");
    register uintptr_t s9 asm ("x25");
    register uintptr_t s10 asm ("x26");
    register uintptr_t s11 asm ("x27");
    arr[0] = s0;
    arr[1] = s1;
    arr[2] = s2;
    arr[3] = s3;
    arr[4] = s4;
    arr[5] = s5;
    arr[6] = s6;
    arr[7] = s7;
    arr[8] = s8;
    arr[9] = s9;
    arr[10] = s10;
    arr[11] = s11;
}

#else

#error "Architecture not supported for gc_helper_get_regs. Set MICROPY_GCREGS_SETJMP to use the fallback implementation."

#endif

#else // !MICROPY_GCREGS_SETJMP

// Even if we have specific support for an architecture, it is
// possible to force use of setjmp-based implementation.

static void gc_helper_get_regs(gc_helper_regs_t arr) {
    setjmp(arr);
}

#endif // MICROPY_GCREGS_SETJMP

// Explicitly mark this as noinline to make sure the regs variable
// is effectively at the top of the stack: otherwise, in builds where
// LTO is enabled and a lot of inlining takes place we risk a stack
// layout where regs is lower on the stack than pointers which have
// just been allocated but not yet marked, and get incorrectly sweeped.
MP_NOINLINE void gc_helper_collect_regs_and_stack(void) {
    gc_helper_regs_t regs;
    gc_helper_get_regs(regs);
    // GC stack (and regs because we captured them)
    void **regs_ptr = (void **)(void *)&regs;
    gc_collect_root(regs_ptr, ((uintptr_t)MP_STATE_THREAD(stack_top) - (uintptr_t)&regs) / sizeof(uintptr_t));
}

#endif // MICROPY_ENABLE_GC
