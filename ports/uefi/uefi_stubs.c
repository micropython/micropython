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

// Compiler-support stubs the -windows target expects.
//
// clang emits a call to __chkstk in the prologue of functions with large stack
// frames (it probes stack guard pages on Windows). UEFI runs flat with all memory
// present and no guard pages, so the probe is unnecessary: a bare return that
// preserves all registers (including the size operand the caller then applies to
// the stack pointer) is correct.

#if defined(__x86_64__)
__attribute__((naked)) void __chkstk(void) {
    __asm__ __volatile__ ("ret");
}
// MSVC marker the CRT normally defines once floating point is used; with
// -nodefaultlib we provide it. (Only the x86-64 codegen references it.)
int _fltused = 0x9875;
#elif defined(__aarch64__)
__attribute__((naked)) void __chkstk(void) {
    __asm__ __volatile__ ("ret");
}
#endif
