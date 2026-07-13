// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Nicko van Someren

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
