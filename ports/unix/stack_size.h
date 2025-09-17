/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Angus Gratton
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
#ifndef MICROPY_INCLUDED_UNIX_STACK_SIZE_H
#define MICROPY_INCLUDED_UNIX_STACK_SIZE_H

#include "py/misc.h"

// Define scaling factors for the stack size (also applies to main thread)
#ifndef UNIX_STACK_MULTIPLIER

#if defined(__arm__) && !defined(__thumb2__)
// ARM (non-Thumb) architectures require more stack.
#define UNIX_STACK_MUL_ARM 2
#else
#define UNIX_STACK_MUL_ARM 1
#endif

#if MP_SANITIZER_BUILD
// Sanitizer features consume significant stack in some cases
// This multiplier can probably be removed when using GCC 12 or newer.
#define UNIX_STACK_MUL_SANITIZERS 4
#else
#define UNIX_STACK_MUL_SANITIZERS 1
#endif

// Double the stack size for 64-bit builds, plus additional scaling
#define UNIX_STACK_MULTIPLIER ((sizeof(void *) / 4) * UNIX_STACK_MUL_ARM * UNIX_STACK_MUL_SANITIZERS)

#endif // UNIX_STACK_MULTIPLIER

#endif // MICROPY_INCLUDED_UNIX_STACK_SIZE_H
