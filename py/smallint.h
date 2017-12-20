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
#ifndef MICROPY_INCLUDED_PY_SMALLINT_H
#define MICROPY_INCLUDED_PY_SMALLINT_H

#include "py/mpconfig.h"
#include "py/misc.h"

// Functions for small integer arithmetic

#ifndef MP_SMALL_INT_MIN

// In SMALL_INT, next-to-highest bits is used as sign, so both must match for value in range
#if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_A || MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_C

#define MP_SMALL_INT_MIN ((mp_int_t)(((mp_int_t)WORD_MSBIT_HIGH) >> 1))
#define MP_SMALL_INT_FITS(n) ((((n) ^ ((n) << 1)) & WORD_MSBIT_HIGH) == 0)
// Mask to truncate mp_int_t to positive value
#define MP_SMALL_INT_POSITIVE_MASK ~(WORD_MSBIT_HIGH | (WORD_MSBIT_HIGH >> 1))

#elif MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_B

#define MP_SMALL_INT_MIN ((mp_int_t)(((mp_int_t)WORD_MSBIT_HIGH) >> 2))
#define MP_SMALL_INT_FITS(n) ((((n) & MP_SMALL_INT_MIN) == 0) || (((n) & MP_SMALL_INT_MIN) == MP_SMALL_INT_MIN))
// Mask to truncate mp_int_t to positive value
#define MP_SMALL_INT_POSITIVE_MASK ~(WORD_MSBIT_HIGH | (WORD_MSBIT_HIGH >> 1) | (WORD_MSBIT_HIGH >> 2))

#elif MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_D

#define MP_SMALL_INT_MIN ((mp_int_t)(((mp_int_t)0xffff800000000000) >> 1))
#define MP_SMALL_INT_FITS(n) ((((n) ^ ((n) << 1)) & 0xffff800000000000) == 0)
// Mask to truncate mp_int_t to positive value
#define MP_SMALL_INT_POSITIVE_MASK ~(0xffff800000000000 | (0xffff800000000000 >> 1))

#endif

#endif

#define MP_SMALL_INT_MAX ((mp_int_t)(~(MP_SMALL_INT_MIN)))

bool mp_small_int_mul_overflow(mp_int_t x, mp_int_t y);
mp_int_t mp_small_int_modulo(mp_int_t dividend, mp_int_t divisor);
mp_int_t mp_small_int_floor_divide(mp_int_t num, mp_int_t denom);

#endif // MICROPY_INCLUDED_PY_SMALLINT_H
