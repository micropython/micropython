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
#ifndef MICROPY_INCLUDED_PY_FORMATFLOAT_H
#define MICROPY_INCLUDED_PY_FORMATFLOAT_H

#include "py/mpconfig.h"

#if MICROPY_PY_BUILTINS_FLOAT
// When searching for the optimal representation for a float, `prec` can
// encode a list of values to be tried.
#define PRECLIST(a, b, c, d) (a + ((b) * 0x100) + ((c) * 0x10000) + ((d) * 0x1000000))
#define IS_PRECLIST(p) ((p) > 0x500)
#define PRECLIST_HEAD(p) ((p) % 0x100)
#define PRECLIST_QUEUE(p) ((p) / 0x100)

#if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
#define MP_FLOAT_REPR_PREC PRECLIST(6, 7, 8, 9)
#elif MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE
#define MP_FLOAT_REPR_PREC PRECLIST(16, 17, 18, 19)
#endif

int mp_format_float(mp_float_t f, char *buf, size_t bufSize, char fmt, int prec, char sign);
#endif // MICROPY_PY_BUILTINS_FLOAT

#endif // MICROPY_INCLUDED_PY_FORMATFLOAT_H
