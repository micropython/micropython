/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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
#ifndef MICROPY_INCLUDED_LIB_UTILS_GCHELPER_H
#define MICROPY_INCLUDED_LIB_UTILS_GCHELPER_H

#include <stdint.h>

#if MICROPY_GCREGS_SETJMP
#include <setjmp.h>
typedef jmp_buf gc_helper_regs_t;
#else

#if defined(__x86_64__)
typedef uintptr_t gc_helper_regs_t[6];
#elif defined(__i386__)
typedef uintptr_t gc_helper_regs_t[4];
#elif defined(__thumb2__) || defined(__thumb__) || defined(__arm__)
typedef uintptr_t gc_helper_regs_t[10];
#endif

#endif

void gc_helper_collect_regs_and_stack(void);

#endif // MICROPY_INCLUDED_LIB_UTILS_GCHELPER_H
