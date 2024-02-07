/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Angus Gratton
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

#ifndef MICROPY_INCLUDED_PY_GC_VALGRIND_H
#define MICROPY_INCLUDED_PY_GC_VALGRIND_H

// This header is intended for including into gc.c directly.
//
// Defining some helper macros here helps keep valgrind integration in gc.c
// as unobtrusive as possible.

#include "py/mpconfig.h"

#if MICROPY_DEBUG_VALGRIND

#include <valgrind/memcheck.h>

// MicroPython heap only knows size of an allocation in blocks,
// this function queries valgrind (if enabled) to tell us the size
// in bytes.
static size_t valgrind_get_alloc_sz(void *p, size_t num_blocks) {
    size_t max_bytes = num_blocks * BYTES_PER_BLOCK;
    VALGRIND_DISABLE_ERROR_REPORTING; // Checking reports an error otherwise
    uintptr_t first_invalid = VALGRIND_CHECK_MEM_IS_ADDRESSABLE(p, max_bytes);
    VALGRIND_ENABLE_ERROR_REPORTING;
    return first_invalid ? (first_invalid - (uintptr_t)p) : max_bytes;
}

// Note: Currently we tell valgrind that the memory is zeroed if MICROPY_GC_CONSERVATIVE_CLEAR
// is set. Running with this unset results in a lot of valgrind errors!
#define VALGRIND_MP_MALLOC(PTR, LEN_BYTES)  \
    VALGRIND_MALLOCLIKE_BLOCK((PTR), (LEN_BYTES), 0, MICROPY_GC_CONSERVATIVE_CLEAR);

// Tell valgrind the block at PTR was OLD_NUM_BLOCKS in length, now NEW_LEN_BYTES in length
#define VALGRIND_MP_RESIZE_BLOCK(PTR, OLD_NUM_BLOCKS, NEW_LEN_BYTES) \
    VALGRIND_RESIZEINPLACE_BLOCK((PTR), valgrind_get_alloc_sz((PTR), (OLD_NUM_BLOCKS)), NEW_LEN_BYTES, 0)

#define VALGRIND_MP_FREE(PTR) VALGRIND_FREELIKE_BLOCK((PTR), 0)

#else // MICROPY_DEBUG_VALGRIND

// No-op definitions
#define VALGRIND_MP_MALLOC(...)
#define VALGRIND_MP_RESIZE_BLOCK(...)
#define VALGRIND_MP_FREE(...)

#define VALGRIND_RESIZEINPLACE_BLOCK(...)
#define VALGRIND_MAKE_MEM_UNDEFINED(...)
#define VALGRIND_MAKE_MEM_NOACCESS(...)

#endif // MICROPY_DEBUG_VALGRIND
#endif // MICROPY_INCLUDED_PY_GC_VALGRIND_H
