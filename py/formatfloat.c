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

#include "py/mpconfig.h"
#if MICROPY_FLOAT_IMPL != MICROPY_FLOAT_IMPL_NONE

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "py/formatfloat.h"

/***********************************************************************

  Routine for converting a arbitrary floating
  point number into a string.

  The code in this funcion was inspired from Fred Bayer's pdouble.c.
  Since pdouble.c was released as Public Domain, I'm releasing this
  code as public domain as well.

  The original code can be found in https://github.com/dhylands/format-float

  Dave Hylands

***********************************************************************/

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
static inline int fp_signbit(float x) { union floatbits fb = {x}; return fb.u & FLT_SIGN_MASK; }
#define fp_isnan(x) isnan(x)
#define fp_isinf(x) isinf(x)
static inline int fp_iszero(float x) { union floatbits fb = {x}; return fb.u == 0; }
static inline int fp_isless1(float x) { union floatbits fb = {x}; return fb.u < 0x3f800000; }

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

static const FPTYPE g_pos_pow[] = {
    #if FPDECEXP > 32
    1e256, 1e128, 1e64,
    #endif
    1e32, 1e16, 1e8, 1e4, 1e2, 1e1
};
static const FPTYPE g_neg_pow[] = {
    #if FPDECEXP > 32
    1e-256, 1e-128, 1e-64,
    #endif
    1e-32, 1e-16, 1e-8, 1e-4, 1e-2, 1e-1
};

int mp_format_float(FPTYPE f, char *buf, size_t buf_size, char fmt, int prec, char sign) {

    char *s = buf;

    if (buf_size <= FPMIN_BUF_SIZE) {
        // FPMIN_BUF_SIZE is the minimum size needed to store any FP number.
        // If the buffer does not have enough room for this (plus null terminator)
        // then don't try to format the float.

        if (buf_size >= 2) {
            *s++ = '?';
        }
        if (buf_size >= 1) {
            *s = '\0';
        }
        return buf_size >= 2;
    }
    if (fp_signbit(f) && !fp_isnan(f)) {
        *s++ = '-';
        f = -f;
    } else {
        if (sign) {
            *s++ = sign;
        }
    }

    // buf_remaining contains bytes available for digits and exponent.
    // It is buf_size minus room for the sign and null byte.
    int buf_remaining = buf_size - 1 - (s - buf);

    {
        char uc = fmt & 0x20;
        if (fp_isinf(f)) {
            *s++ = 'I' ^ uc;
            *s++ = 'N' ^ uc;
            *s++ = 'F' ^ uc;
            goto ret;
        } else if (fp_isnan(f)) {
            *s++ = 'N' ^ uc;
            *s++ = 'A' ^ uc;
            *s++ = 'N' ^ uc;
        ret:
            *s = '\0';
            return s - buf;
        }
    }

    if (prec < 0) {
        prec = 6;
    }
    char e_char = 'E' | (fmt & 0x20);   // e_char will match case of fmt
    fmt |= 0x20; // Force fmt to be lowercase
    char org_fmt = fmt;
    if (fmt == 'g' && prec == 0) {
        prec = 1;
    }
    int e, e1;
    int dec = 0;
    char e_sign = '\0';
    int num_digits = 0;
    const FPTYPE *pos_pow = g_pos_pow;
    const FPTYPE *neg_pow = g_neg_pow;

    if (fp_iszero(f)) {
        e = 0;
        if (fmt == 'f') {
            // Truncate precision to prevent buffer overflow
            if (prec + 2 > buf_remaining) {
                prec = buf_remaining - 2;
            }
            num_digits = prec + 1;
        } else {
            // Truncate precision to prevent buffer overflow
            if (prec + 6 > buf_remaining) {
                prec = buf_remaining - 6;
            }
            if (fmt == 'e') {
                e_sign = '+';
            }
        }
    } else if (fp_isless1(f)) {
        // We need to figure out what an integer digit will be used
        // in case 'f' is used (or we revert other format to it below).
        // As we just tested number to be <1, this is obviously 0,
        // but we can round it up to 1 below.
        char first_dig = '0';
        if (f >= FPROUND_TO_ONE) {
            first_dig = '1';
        }

        // Build negative exponent
        for (e = 0, e1 = FPDECEXP; e1; e1 >>= 1, pos_pow++, neg_pow++) {
            if (*neg_pow > f) {
                e += e1;
                f *= *pos_pow;
            }
        }
        char e_sign_char = '-';
        if (fp_isless1(f) && f >= FPROUND_TO_ONE) {
            f = FPCONST(1.0);
            if (e == 0) {
                e_sign_char = '+';
            }
        } else if (fp_isless1(f)) {
            e++;
            f *= FPCONST(10.0);
        }

        // If the user specified 'g' format, and e is <= 4, then we'll switch
        // to the fixed format ('f')

        if (fmt == 'f' || (fmt == 'g' && e <= 4)) {
            fmt = 'f';
            dec = -1;
            *s++ = first_dig;

            if (org_fmt == 'g') {
                prec += (e - 1);
            }

            // truncate precision to prevent buffer overflow
            if (prec + 2 > buf_remaining) {
                prec = buf_remaining - 2;
            }

            num_digits = prec;
            if (num_digits) {
                *s++ = '.';
                while (--e && num_digits) {
                    *s++ = '0';
                    num_digits--;
                }
            }
        } else {
            // For e & g formats, we'll be printing the exponent, so set the
            // sign.
            e_sign = e_sign_char;
            dec = 0;

            if (prec > (buf_remaining - FPMIN_BUF_SIZE)) {
                prec = buf_remaining - FPMIN_BUF_SIZE;
                if (fmt == 'g') {
                    prec++;
                }
            }
        }
    } else {
        // Build positive exponent
        for (e = 0, e1 = FPDECEXP; e1; e1 >>= 1, pos_pow++, neg_pow++) {
            if (*pos_pow <= f) {
                e += e1;
                f *= *neg_pow;
            }
        }

        // It can be that f was right on the edge of an entry in pos_pow needs to be reduced
        if ((int)f >= 10) {
            e += 1;
            f *= FPCONST(0.1);
        }

        // If the user specified fixed format (fmt == 'f') and e makes the
        // number too big to fit into the available buffer, then we'll
        // switch to the 'e' format.

        if (fmt == 'f') {
            if (e >= buf_remaining) {
                fmt = 'e';
            } else if ((e + prec + 2) > buf_remaining) {
                prec = buf_remaining - e - 2;
                if (prec < 0) {
                    // This means no decimal point, so we can add one back
                    // for the decimal.
                    prec++;
                }
            }
        }
        if (fmt == 'e' && prec > (buf_remaining - FPMIN_BUF_SIZE)) {
            prec = buf_remaining - FPMIN_BUF_SIZE;
        }
        if (fmt == 'g'){
            // Truncate precision to prevent buffer overflow
            if (prec + (FPMIN_BUF_SIZE - 1) > buf_remaining) {
                prec = buf_remaining - (FPMIN_BUF_SIZE - 1);
            }
        }
        // If the user specified 'g' format, and e is < prec, then we'll switch
        // to the fixed format.

        if (fmt == 'g' && e < prec) {
            fmt = 'f';
            prec -= (e + 1);
        }
        if (fmt == 'f') {
            dec = e;
            num_digits = prec + e + 1;
        } else {
            e_sign = '+';
        }
    }
    if (prec < 0) {
        // This can happen when the prec is trimmed to prevent buffer overflow
        prec = 0;
    }

    // We now have num.f as a floating point number between >= 1 and < 10
    // (or equal to zero), and e contains the absolute value of the power of
    // 10 exponent. and (dec + 1) == the number of dgits before the decimal.

    // For e, prec is # digits after the decimal
    // For f, prec is # digits after the decimal
    // For g, prec is the max number of significant digits
    //
    // For e & g there will be a single digit before the decimal
    // for f there will be e digits before the decimal

    if (fmt == 'e') {
        num_digits = prec + 1;
    } else if (fmt == 'g') {
        if (prec == 0) {
            prec = 1;
        }
        num_digits = prec;
    }

    // Print the digits of the mantissa
    for (int i = 0; i < num_digits; ++i, --dec) {
        int32_t d = (int32_t)f;
        *s++ = '0' + d;
        if (dec == 0 && prec > 0) {
            *s++ = '.';
        }
        f -= (FPTYPE)d;
        f *= FPCONST(10.0);
    }

    // Round
    // If we print non-exponential format (i.e. 'f'), but a digit we're going
    // to round by (e) is too far away, then there's nothing to round.
    if ((org_fmt != 'f' || e <= num_digits) && f >= FPCONST(5.0)) {
        char *rs = s;
        rs--;
        while (1) {
            if (*rs == '.') {
                rs--;
                continue;
            }
            if (*rs < '0' || *rs > '9') {
                // + or -
                rs++; // So we sit on the digit to the right of the sign
                break;
            }
            if (*rs < '9') {
                (*rs)++;
                break;
            }
            *rs = '0';
            if (rs == buf) {
                break;
            }
            rs--;
        }
        if (*rs == '0') {
            // We need to insert a 1
            if (rs[1] == '.' && fmt != 'f') {
                // We're going to round 9.99 to 10.00
                // Move the decimal point
                rs[0] = '.';
                rs[1] = '0';
                if (e_sign == '-') {
                    e--;
                    if (e == 0) {
                        e_sign = '+';
                    }
                } else {
                    e++;
                }
            } else {
                // Need at extra digit at the end to make room for the leading '1'
                s++;
            }
            char *ss = s;
            while (ss > rs) {
                *ss = ss[-1];
                ss--;
            }
            *rs = '1';
        }
    }

    // verify that we did not overrun the input buffer so far
    assert((size_t)(s + 1 - buf) <= buf_size);

    if (org_fmt == 'g' && prec > 0) {
        // Remove trailing zeros and a trailing decimal point
        while (s[-1] == '0') {
            s--;
        }
        if (s[-1] == '.') {
            s--;
        }
    }
    // Append the exponent
    if (e_sign) {
        *s++ = e_char;
        *s++ = e_sign;
        if (FPMIN_BUF_SIZE == 7 && e >= 100) {
            *s++ = '0' + (e / 100);
        }
        *s++ = '0' + ((e / 10) % 10);
        *s++ = '0' + (e % 10);
    }
    *s = '\0';

    // verify that we did not overrun the input buffer
    assert((size_t)(s + 1 - buf) <= buf_size);

    return s - buf;
}

#endif // MICROPY_FLOAT_IMPL != MICROPY_FLOAT_IMPL_NONE
