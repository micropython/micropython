/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Angus Gratton
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

/* This header provides some inline implementations of functions used by multiple objint*.c
   files.

   Implementations here should only be called by one of the objint*.c files. If
   called from more than one place in a single firmware, place into objint.c and
   define in objint.h
 */
#ifndef MICROPY_INCLUDED_PY_OBJINT_IMPL_H
#define MICROPY_INCLUDED_PY_OBJINT_IMPL_H

#include "py/binary.h"
#include "py/objint.h"
#include "py/runtime.h"

static void mp_obj_int_raise_to_bytes_overflow_error(size_t nbytes) {
    mp_raise_msg_varg(&mp_type_OverflowError, MP_ERROR_TEXT("value would overflow a %d byte buffer"), nbytes);
}

static void mp_obj_int_raise_unsigned_negative_overflow_error(void) {
    mp_raise_msg_varg(&mp_type_OverflowError, MP_ERROR_TEXT("can't convert negative int to unsigned"));
}

static void mp_obj_small_int_buffer_overflow_check(mp_int_t val, size_t nbytes, bool is_signed) {
    // Fast path for zero.
    if (val == 0) {
        return;
    }

    if (!is_signed && val < 0) {
        // Trying to store negative values in unsigned bytes
        mp_obj_int_raise_unsigned_negative_overflow_error();
    }

    if (nbytes >= sizeof(val)) {
        // All N bit small integers fit in an unsigned N bit integer.
        // This case prevents shifting too far below.
        return;
    }

    if (nbytes == 0) {
        // Can't fit a non-negative value in 0 bytes (prevents negative left shift, below)
        goto raise;
    }

    if (is_signed) {
        mp_int_t edge = ((mp_int_t)1 << (nbytes * 8 - 1));
        if (-edge <= val && val < edge) {
            return;
        }
        // Out of range, fall through to raise.
    } else {
        // Unsigned. We already know val >= 0.
        mp_int_t edge = ((mp_int_t)1 << (nbytes * 8));
        if (val < edge) {
            return;
        }
        // Fall through to raise.
    }

raise:
    mp_obj_int_raise_to_bytes_overflow_error(nbytes);
}

static inline void mp_obj_small_int_to_bytes(mp_int_t val, size_t buf_len, byte *buf, bool big_endian, bool is_signed, bool overflow_check) {
    if (overflow_check) {
        mp_obj_small_int_buffer_overflow_check(val, buf_len, is_signed);
    }
    mp_binary_set_int(buf_len, buf, sizeof(val), val, big_endian);

}

#endif // MICROPY_INCLUDED_PY_OBJINT_IMPL_H
