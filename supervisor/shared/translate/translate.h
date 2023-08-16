/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "supervisor/shared/translate/compressed_string.h"

// Map MicroPython's error messages to our translations.
#if !defined(MICROPY_ENABLE_DYNRUNTIME) || !MICROPY_ENABLE_DYNRUNTIME
#define MP_ERROR_TEXT(x) translate(x)
#endif

// translate() is a giant function with many strcmp calls. The assumption is
// that the build process will optimize this away and replace it with the
// appropriate compressed data for each call site.

#if CIRCUITPY_TRANSLATE_OBJECT == 0
// Without LTO, we need to include a copy of this function in each compilation
// unit so that the compile stage can do the optimization. Otherwise the linker
// will leave this as a giant function and have each call site call into it.
#include "supervisor/shared/translate/translate_impl.h"
#else
// In link time optimized (LTO) builds, we can compile this once into a .o and
// at link time the calls will be optimized.
const compressed_string_t *translate(const char *c);
#endif
