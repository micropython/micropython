/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Damien P. George
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

#if MICROPY_PY_BUILTINS_FLOAT

#include <math.h>

#if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
// 1 sign bit, 8 exponent bits, and 23 mantissa bits.
// exponent values 0 and 255 are reserved, exponent can be 1 to 254.
// exponent is stored with a bias of 127.
// The min and max floats are on the order of 1x10^37 and 1x10^-37

#define FPTYPE float
#define FPCONST(x) x##F
#define FPROUND_TO_ONE 0.9999995F
#define FPDECEXP 32
#define FPMIN_BUF_SIZE 6 // +9e+99

#define FLT_SIGN_MASK   0x80000000
#define FLT_EXP_MASK    0x7F800000
#define FLT_MAN_MASK    0x007FFFFF

union floatbits {
    float f;
    uint32_t u;
};
static inline int fp_signbit(float x) {
    union floatbits fb = {x};
    return fb.u & FLT_SIGN_MASK;
}
#define fp_isnan(x) isnan(x)
#define fp_isinf(x) isinf(x)
static inline int fp_iszero(float x) {
    union floatbits fb = {x};
    return fb.u == 0;
}
static inline int fp_isless1(float x) {
    union floatbits fb = {x};
    return fb.u < 0x3f800000;
}

#elif MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE

#define FPTYPE double
#define FPCONST(x) x
#define FPROUND_TO_ONE 0.999999999995
#define FPDECEXP 256
#define FPMIN_BUF_SIZE 7 // +9e+199
#define fp_signbit(x) signbit(x)
#define fp_isnan(x) isnan(x)
#define fp_isinf(x) isinf(x)
#define fp_iszero(x) (x == 0)
#define fp_isless1(x) (x < 1.0)

#endif

#endif
