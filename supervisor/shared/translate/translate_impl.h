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

#ifndef NO_QSTR
#define QDEF(id, hash, len, str)
#define TRANSLATION(english_id, number) extern compressed_string_t translation##number;
#include "genhdr/qstrdefs.generated.h"
#undef TRANSLATION
#undef QDEF
#endif

#if CIRCUITPY_TRANSLATE_OBJECT == 0
static
#endif
inline
// gcc10 -flto has issues with this being always_inline for debug builds.
#if !CIRCUITPY_LTO || CIRCUITPY_DEBUG < 1
__attribute__((always_inline))
#endif
// Prevent instrumenting this because that disables the inlining we rely of for code size
// optimization.
__attribute__((no_instrument_function)) const compressed_string_t *translate(const char *original) {
    #ifndef NO_QSTR
    #define QDEF(id, hash, len, str)
    #define TRANSLATION(english_id, number) if (strcmp(original, english_id) == 0) { return &translation##number; } else
    #include "genhdr/qstrdefs.generated.h"
#undef TRANSLATION
#undef QDEF
    #endif
    return NULL;
}
