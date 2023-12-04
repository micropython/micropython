/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Damien P. George
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

#include "py/mpconfig.h"

#if MICROPY_PY_BUILTINS_FLOAT && MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT

// MicroPython is configured to use single precision float.

#define MICROPY_FLOAT_CONST(x) x##F

#if MICROPY_USE_INTERNAL_LIBM

// Use local libm, single precision.
#include "lib/libm/libm.h"
#define MICROPY_FLOAT_C_FUN(fun) mp_libm_##fun##f

#else

// Use system math library.
#include <math.h>
#define MICROPY_FLOAT_C_FUN(fun) fun##f

#endif

#elif MICROPY_PY_BUILTINS_FLOAT && MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE

// MicroPython is configured to use double precision float.

#define MICROPY_FLOAT_CONST(x) x

#if MICROPY_USE_INTERNAL_LIBM

// Local libm, double precision.
#include "lib/libm_dbl/libm.h"
#define MICROPY_FLOAT_C_FUN(fun) mp_libm_##fun

#else

// Use system math library.
#include <math.h>
#define MICROPY_FLOAT_C_FUN(fun) fun

#endif

#endif
