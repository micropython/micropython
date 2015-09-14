/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include <stdlib.h>
#include <stdint.h>
#include "py/formatfloat.h"

#if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
/***********************************************************************

  Routine for converting a single-precision floating
  point number into a string.

  The code in this funcion was inspired from Fred Bayer's pdouble.c.
  Since pdouble.c was released as Public Domain, I'm releasing this
  code as public domain as well.

  The original code can be found in https://github.com/dhylands/format-float

  Dave Hylands

***********************************************************************/

// 1 sign bit, 8 exponent bits, and 23 mantissa bits.
// exponent values 0 and 255 are reserved, exponent can be 1 to 254.
// exponent is stored with a bias of 127.
// The min and max floats are on the order of 1x10^37 and 1x10^-37

#define FLT_SIGN_MASK   0x80000000
#define FLT_EXP_MASK    0x7F800000
#define FLT_MAN_MASK    0x007FFFFF

static const float g_pos_pow[] = {
    1e32, 1e16, 1e8, 1e4, 1e2, 1e1
};
static const float g_neg_pow[] = {
    1e-32, 1e-16, 1e-8, 1e-4, 1e-2, 1e-1
};

int mp_format_float(float f, char *buf, size_t buf_size, char fmt, int prec, char sign) {

    char *s = buf;
    int buf_remaining = buf_size - 1;

    union {
        float f;
        uint32_t u;
    } num = {f};

    if (buf_size < 7) {
        // Smallest exp notion is -9e+99 which is 6 chars plus terminating
        // null.

        if (buf_size >= 2) {
            *s++ = '?';
        }
        if (buf_size >= 1) {
            *s++ = '\0';
        }
        return buf_size >= 2;
    }
    if (num.u & FLT_SIGN_MASK) {
        *s++ = '-';
        num.u &= ~FLT_SIGN_MASK;
    } else {
        if (sign) {
            *s++ = sign;
        }
    }
    buf_remaining -= (s - buf); // Adjust for sign

    if ((num.u & FLT_EXP_MASK) == FLT_EXP_MASK) {
        char uc = fmt & 0x20;
        if ((num.u & FLT_MAN_MASK) == 0) {
            *s++ = 'I' ^ uc;
            *s++ = 'N' ^ uc;
            *s++ = 'F' ^ uc;
        } else {
            *s++ = 'N' ^ uc;
            *s++ = 'A' ^ uc;
            *s++ = 'N' ^ uc;
        }
        *s = '\0';
        return s - buf;
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
    const float *pos_pow = g_pos_pow;
    const float *neg_pow = g_neg_pow;

    if (num.u == 0) {
        e = 0;
        if (fmt == 'e') {
            e_sign = '+';
        } else if (fmt == 'f') {
            num_digits = prec + 1;
        }
    } else if (num.u < 0x3f800000) { // f < 1.0
        // Build negative exponent
        for (e = 0, e1 = 32; e1; e1 >>= 1, pos_pow++, neg_pow++) {
            if (*neg_pow > num.f) {
                e += e1;
                num.f *= *pos_pow;
            }
        }
        char first_dig = '0';
        char e_sign_char = '-';
        if (num.f < 1.0F && num.f >= 0.9999995F) {
            num.f = 1.0F;
            if (e > 1) {
                // numbers less than 1.0 start with 0.xxx
                first_dig = '1'; 
            }
            if (e == 0) {
                e_sign_char = '+';
            }
        } else {
            e++; 
            num.f *= 10.0F;
        }

        // If the user specified 'g' format, and e is <= 4, then we'll switch
        // to the fixed format ('f')

        if (fmt == 'f' || (fmt == 'g' && e <= 4)) {
            fmt = 'f';
            dec = -1;
            *s++ = first_dig;

            if (prec + e + 1 > buf_remaining) {
                prec = buf_remaining - e - 1;
            }

            if (org_fmt == 'g') {
                prec += (e - 1);
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

            if (prec > (buf_remaining - 6)) {
                prec = buf_remaining - 6;
                if (fmt == 'g') {
                    prec++;
                }
            }
        }
    } else {
        // Build positive exponent
        for (e = 0, e1 = 32; e1; e1 >>= 1, pos_pow++, neg_pow++) {
            if (*pos_pow <= num.f) {
                e += e1;
                num.f *= *neg_pow;
            }
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
        if (fmt == 'e' && prec > (buf_remaining - 6)) {
            prec = buf_remaining - 6;
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
        int32_t d = num.f;
        *s++ = '0' + d;
        if (dec == 0 && prec > 0) {
            *s++ = '.';
        }
        num.f -= (float)d;
        num.f *= 10.0F;
    }

    // Round
    if (num.f >= 5.0F) {
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
                } else {
                    e++; 
                }
            }
            s++;
            char *ss = s; 
            while (ss > rs) {
                *ss = ss[-1];
                ss--;
            }
            *rs = '1';
        }
        if (num.u < 0x3f800000 && fmt == 'f') {
            // We rounded up to 1.0
            prec--;
        }
    }

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
        *s++ = '0' + (e / 10);
        *s++ = '0' + (e % 10);
    }
    *s = '\0';

    return s - buf;
}

#elif MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE

#include <errno.h>
#include <stdio.h>

#ifdef _MSC_VER
// For msvc we need to address some quirks in the snprintf implementation:
// - there is no standard snprintf, it is named _snprintf instead
// - 'F' format isn't handled so use 'f'
// - nan and inf are printed as 1.#QNAN and 1.#INF
#include <math.h>
#include <string.h>

STATIC int copy_with_sign(char *dest, size_t bufSize, const char *value, char sign) {
    if (bufSize == 0) {
        return 0;
    }
    size_t numSignChars = 0;
    if (sign) {
        *dest = sign;
        ++numSignChars;
    }
    // check total length including terminator
    size_t length = strlen(value) + 1 + numSignChars;
    if (length > bufSize) {
        length = bufSize;
    }
    // length without terminator
    --length;
    if (length > numSignChars) {
        memcpy(dest + numSignChars, value, length - numSignChars);
    }
    dest[length] = 0;
    return length;
}

#define snprintf _snprintf
#endif

int mp_format_float(double value, char *buf, size_t bufSize, char fmt, int prec, char sign) {
    if (!buf) {
        errno = EINVAL;
        return -1;
    }
#ifdef _MSC_VER
    if (isnan(value)) {
        return copy_with_sign(buf, bufSize, "nan", sign);
    } else if (isinf(value)) {
        return copy_with_sign(buf, bufSize, "inf", value > 0.0 ? sign : '-');
    } else {
        if (fmt == 'F') {
            fmt = 'f';
        }
#endif
        char fmt_buf[6];
        char *fmt_s = fmt_buf;

        *fmt_s++ = '%';
        if (sign) {
            *fmt_s++ = sign;
        }
        *fmt_s++ = '.';
        *fmt_s++ = '*';
        *fmt_s++ = fmt;
        *fmt_s = '\0';

        return snprintf(buf, bufSize, fmt_buf, prec, value);
#ifdef _MSC_VER
    }
#endif
}

#endif
