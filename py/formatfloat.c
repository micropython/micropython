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
#include "py/misc.h"
#if MICROPY_FLOAT_IMPL != MICROPY_FLOAT_IMPL_NONE

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "py/formatfloat.h"
#include "py/parsenum.h"

/***********************************************************************

  Routine for converting a arbitrary floating
  point number into a string.

  The code in this function was inspired from Fred Bayer's pdouble.c.
  Since pdouble.c was released as Public Domain, I'm releasing this
  code as public domain as well.

  The original code can be found in https://github.com/dhylands/format-float

  Dave Hylands

***********************************************************************/

// This option might be provided in mpconfig.h in the future,
// but for now it is here just for testing.
// When enabled, with MICROPY_FLOAT_IMPL_DOUBLE
// - `repr` reversibility errors drop from 40% to 4.5%
// - conversion time increases by 20% in average
// When enabled, with MICROPY_FLOAT_IMPL_SIMPLE
// - `repr` reversibility errors drop from 10% to 5%
// - conversion time increases by 7% in average
#define MICROPY_FLOAT_HIGH_QUALITY_REPR 1

// Float formatting debug code is intended for use in ports/unix only,
// as it uses the libc float printing function as a reference.
#define DEBUG_FLOAT_FORMATTING 0

#if DEBUG_FLOAT_FORMATTING
#define DEBUG_PRINTF(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG_PRINTF(...)
#endif

#if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
// 1 sign bit, 8 exponent bits, and 23 mantissa bits.
// exponent values 0 and 255 are reserved, exponent can be 1 to 254.
// exponent is stored with a bias of 127.
// The min and max floats are on the order of 1x10^37 and 1x10^-37

#define FPCONST(x) x##F
#define FPUINT_FMT "%u"
#define ITER_MANTISSA_DIGITS 6
#define SAFE_MANTISSA_DIGITS 8
#define MAX_MANTISSA_DIGITS 9
#define MANTISSA_ROUND 10000u
#define FPMIN_BUF_SIZE 6 // +9e+99

#define FLT_SIGN_MASK   0x80000000

static inline int fp_signbit(float x) {
    mp_float_union_t fb = {x};
    return fb.i & FLT_SIGN_MASK;
}
#define fp_isnan(x) isnan(x)
#define fp_isinf(x) isinf(x)
static inline int fp_iszero(float x) {
    mp_float_union_t fb = {x};
    return fb.i == 0;
}
static inline int fp_isless1(float x) {
    mp_float_union_t fb = {x};
    return fb.i < 0x3f800000;
}

#elif MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE

#define FPCONST(x) x
#define FPUINT_FMT "%lu"
#define ITER_MANTISSA_DIGITS 16
#define SAFE_MANTISSA_DIGITS 16
#define MAX_MANTISSA_DIGITS 19
#define MANTISSA_ROUND 100000u
#define FPMIN_BUF_SIZE 7 // +9e+199
#define fp_signbit(x) signbit(x)
#define fp_isnan(x) isnan(x)
#define fp_isinf(x) isinf(x)
#define fp_iszero(x) (x == 0)
#define fp_isless1(x) (x < 1.0)

#endif // MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT/DOUBLE

static inline int fp_expval(mp_float_t x) {
    mp_float_union_t fb = {x};
    return (int)((fb.i >> MP_FLOAT_FRAC_BITS) & (~(0xFFFFFFFF << MP_FLOAT_EXP_BITS))) - MP_FLOAT_EXP_OFFSET;
}

// formatting flags
#define FMT_MODE_E 0x01  // user requested scientific notation (%e)
#define FMT_MODE_G 0x02  // user requested general format (%g)
#define FMT_MODE_F 0x04  // we must render using fixed-point notation (%f or %g)
#define FMT_E_CASE 0x20  // don't change this value (used for case conversion!)

// Helper to convert a decimal mantissa (provided as an mp_float_uint_t) to string
static int mp_format_mantissa(mp_float_uint_t mantissa, mp_float_uint_t mantissa_cap, char *buf, size_t buf_size,
    int num_digits, int trailing_zeroes, int dec, int e, int flags) {

    char *s = buf;
    int dot = 0;

    DEBUG_PRINTF("mantissa "FPUINT_FMT " (cap="FPUINT_FMT "):\n", mantissa, mantissa_cap);

    // If rounding created an extra digit, fix mantissa
    if (mantissa >= mantissa_cap) {
        if (flags & FMT_MODE_F) {
            num_digits++;
            dec++;
        } else {
            mantissa /= 10;
            e++;
        }
    }

    // Convert the mantissa to a string
    for (int digit = num_digits - 1; digit >= 0; digit--) {
        int digit_ofs = (digit > dec ? digit + 1 : digit);
        s[digit_ofs] = '0' + (int)(mantissa % 10);
        mantissa /= 10;
    }
    if (dec + 1 < num_digits) {
        dot = 1;
        s++;
        s[dec] = '.';
    }
    s += num_digits;
    #if DEBUG_FLOAT_FORMATTING
    *s = 0;
    DEBUG_PRINTF("  =      %s exp=%d num_digits=%d zeroes=%d\n", buf, e, num_digits, trailing_zeroes);
    #endif

    // verify that we did not overrun the input buffer so far
    assert((size_t)(s + 1 - buf) <= buf_size);

    if (dot && (flags & FMT_MODE_G) != 0) {
        // %g format requires to remove trailing zeroes...
        while (s[-1] == '0') {
            s--;
        }
        if (s[-1] == '.') {
            s--;
        }
    } else if (trailing_zeroes) {
        // ... while %f may require to add more of them
        dec -= num_digits - 1;
        while (trailing_zeroes--) {
            if (!dec--) {
                int max_more = (int)(buf + buf_size - s - 3);
                if (max_more < 0) {
                    break;
                }
                if (trailing_zeroes > max_more) {
                    trailing_zeroes = max_more;
                }
                *s++ = '.';
            }
            *s++ = '0';
        }
    }

    // Append the exponent if requested
    int add_exponent = (flags & FMT_MODE_E);
    if (!(flags & FMT_MODE_F)) {
        if (e != 0) {
            add_exponent = 1;
        }
    }
    if (add_exponent) {
        *s++ = 'E' | (flags & FMT_E_CASE);
        if (e >= 0) {
            *s++ = '+';
        } else {
            *s++ = '-';
            e = -e;
        }
        if (FPMIN_BUF_SIZE == 7 && e >= 100) {
            *s++ = '0' + (e / 100);
        }
        *s++ = '0' + ((e / 10) % 10);
        *s++ = '0' + (e % 10);
    }
    *s = '\0';
    DEBUG_PRINTF("  ===>   %s\n", buf);

    // verify that we did not overrun the input buffer
    assert((size_t)(s + 1 - buf) <= buf_size);

    return s - buf;
}

int mp_format_float(mp_float_t f, char *buf, size_t buf_size, char fmt, int prec, char sign) {

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
    int flags = (fmt & 0x20);  // setup FMT_E_CASE
    fmt |= 0x20; // Force fmt to be lowercase
    if (fmt == 'f') {
        flags |= FMT_MODE_F;
    } else if (fmt == 'g') {
        flags |= FMT_MODE_G;
        if (prec == 0) {
            prec = 1;
        }
    } else {
        flags |= FMT_MODE_E;
    }
    int e;
    int dec = 0;
    int num_digits = 0;
    int signed_e = 0;
    mp_float_t f_entry = f;  // Save original f for future checks

    // Approximate power of 10 exponent from binary exponent.
    // abs(e_guess) is lower bound on abs(power of 10 exponent).
    int e_guess = (int)(fp_expval(f) * FPCONST(0.3010299956639812));  // 1/log2(10).
    if (fp_iszero(f)) {
        e = 0;
        if (flags & FMT_MODE_F) {
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
        }
    } else if (fp_isless1(f)) {
        // Build negative exponent
        e = -e_guess;
        mp_float_t u_base = MICROPY_FLOAT_C_FUN(pow)(10, -e);
        while (u_base > f) {
            ++e;
            u_base = MICROPY_FLOAT_C_FUN(pow)(10, -e);
        }
        // Normalize out the inferred unit.  Use divide because
        // pow(10, e) * pow(10, -e) is slightly < 1 for some e in float32
        // (e.g. print("%.12f" % ((1e13) * (1e-13))))
        f /= u_base;

        // If the user specified 'g' format, and e is <= 4, then we'll switch
        // to the fixed format ('f')
        if (flags & FMT_MODE_G) {
            if (e <= 4) {
                flags |= FMT_MODE_F;
                prec += e - 1;
            }
        }

        if (flags & FMT_MODE_F) {
            dec = 0;

            // truncate precision to prevent buffer overflow
            if (prec + 2 > buf_remaining) {
                prec = buf_remaining - 2;
            }

            num_digits = prec;
            signed_e = 0;
            f = f_entry;
            ++num_digits;
        } else {
            dec = 0;

            if (prec > (buf_remaining - FPMIN_BUF_SIZE)) {
                prec = buf_remaining - FPMIN_BUF_SIZE;
                if (flags & FMT_MODE_G) {
                    prec++;
                }
            }
            signed_e = -e;
        }
    } else {
        // Build positive exponent.
        // We don't modify f at this point to avoid inaccuracies from
        // scaling it.  Instead, we find the product of powers of 10
        // that is not greater than it, and use that to start the
        // mantissa.
        e = e_guess;
        mp_float_t next_u = MICROPY_FLOAT_C_FUN(pow)(10, e + 1);
        while (f >= next_u) {
            ++e;
            next_u = MICROPY_FLOAT_C_FUN(pow)(10, e + 1);
        }

        // If the user specified fixed format (fmt == 'f') and e makes the
        // number too big to fit into the available buffer, then we'll
        // switch to the 'e' format.

        if (flags & FMT_MODE_F) {
            if (e >= buf_remaining) {
                flags ^= FMT_MODE_F;
                flags |= FMT_MODE_E;
            } else if ((e + prec + 2) > buf_remaining) {
                prec = buf_remaining - e - 2;
                if (prec < 0) {
                    // This means no decimal point, so we can add one back
                    // for the decimal.
                    prec++;
                }
            }
        }
        if (flags & FMT_MODE_E) {
            if (prec > (buf_remaining - FPMIN_BUF_SIZE)) {
                prec = buf_remaining - FPMIN_BUF_SIZE;
            }
        }
        if (flags & FMT_MODE_G) {
            // Truncate precision to prevent buffer overflow
            if (prec + (FPMIN_BUF_SIZE - 1) > buf_remaining) {
                prec = buf_remaining - (FPMIN_BUF_SIZE - 1);
            }
            // If the user specified 'g' format, and e is < prec, then we'll switch
            // to the fixed format.
            if (e < prec && e < SAFE_MANTISSA_DIGITS) {
                flags |= FMT_MODE_F;
                prec -= e + 1;
            }
        }

        if (flags & FMT_MODE_F) {
            dec = e;
            num_digits = prec + e + 1;
        }
        signed_e = e;
    }
    if (prec < 0) {
        // This can happen when the prec is trimmed to prevent buffer overflow
        prec = 0;
    }

    // At this point e contains the absolute value of the power of 10 exponent.
    // (dec + 1) == the number of digits before the decimal.

    // For e, prec is # digits after the decimal
    // For f, prec is # digits after the decimal
    // For g, prec is the max number of significant digits
    //
    // For e & g there will be a single digit before the decimal
    // for f there will be e digits before the decimal

    if (flags & FMT_MODE_E) {
        num_digits = prec + 1;
    } else if (!(flags & FMT_MODE_F)) {
        if (prec == 0) {
            prec = 1;
        }
        num_digits = prec;
    }

    // Digits beyond float precision are meaningless,
    // except for %f format which might need to compensate
    // for them using zero-padding.
    int trailing_zeroes = 0;
    if (num_digits > MAX_MANTISSA_DIGITS) {
        if (flags & FMT_MODE_F) {
            trailing_zeroes = num_digits - MAX_MANTISSA_DIGITS;
        }
        num_digits = MAX_MANTISSA_DIGITS;
    }
    mp_float_uint_t mantissa_cap = 10;
    for (int n = 1; n < num_digits; n++) {
        mantissa_cap *= 10;
    }

    // Build the decimal mantissa into an uint
    int decexp = num_digits - (signed_e < 0 ? 0 : signed_e);
    mp_float_t mantissa_flt = mp_decimal_exp(f, decexp - 1);
    mp_float_uint_t mantissa = (mp_float_uint_t)(mantissa_flt + FPCONST(0.5));
    DEBUG_PRINTF("input=%.19g num_digits=%d signed_e=%d prec=%d mantissa="FPUINT_FMT "\n", (double)f_entry, num_digits, signed_e, prec, mantissa);

    if (num_digits >= MAX_MANTISSA_DIGITS - 1) {
        // When using high-precision repr, the last two digits carry a high uncertainty
        // We have a chance to easily round the number within the uncertainty range, so use it
        unsigned epsilon = (unsigned)(mantissa % MANTISSA_ROUND);
        if (epsilon) {
            if (epsilon < 100u) {
                mantissa -= epsilon;
                DEBUG_PRINTF("-> round down mantissa to "FPUINT_FMT "\n", mantissa);
            } else if (epsilon > MANTISSA_ROUND - 100u) {
                mantissa += MANTISSA_ROUND - epsilon;
                DEBUG_PRINTF("-> round up mantissa to "FPUINT_FMT "\n", mantissa);
            }
        }
    }

    // Finally convert the decimal mantissa to a floating-point string, according to formatting rules
    buf_size -= s - buf;
    int reprlen = mp_format_mantissa(mantissa, mantissa_cap, s, buf_size, num_digits, trailing_zeroes, dec, signed_e, flags);

    #if MICROPY_FLOAT_HIGH_QUALITY_REPR
    // The initial decimal mantissa might not have been be completely accurate due
    // to the previous loating point operations. The best way to verify this is to
    // parse the resulting number and compare against the original
    mp_float_t check;
    mp_parse_float_internal(s, reprlen, &check);
    mp_float_t first_guess = check;

    if (check == f_entry) {
        // we have a perfect match
        DEBUG_PRINTF(FPUINT_FMT ": perfect match (direct)\n", mantissa);
    } else {
        // In order to get the best possible representation, we will perform a
        // dichotomic search for a reversible representation.
        // This will also provide optimal rounding on the fly.
        unsigned err_range = 1;
        if (num_digits > ITER_MANTISSA_DIGITS) {
            err_range <<= 3 * (num_digits - ITER_MANTISSA_DIGITS);
        }
        int maxruns = 3 + 3 * (MAX_MANTISSA_DIGITS - ITER_MANTISSA_DIGITS);
        while (check != f_entry && maxruns-- > 0) {
            if (check < f_entry) {
                mantissa += err_range;
            } else {
                while (mantissa < err_range) {
                    err_range >>= 1;
                }
                mantissa -= err_range;
            }
            reprlen = mp_format_mantissa(mantissa, mantissa_cap, s, buf_size, num_digits, trailing_zeroes, dec, signed_e, flags);
            mp_parse_float_internal(s, reprlen, &check);
            DEBUG_PRINTF("check=%.19g num_digits=%d signed_e=%d prec=%d mantissa="FPUINT_FMT "\n", check, num_digits, signed_e, prec, mantissa);
            if (check == f_entry) {
                // we have a perfect match
                DEBUG_PRINTF(FPUINT_FMT ": perfect match\n", mantissa);
                break;
            }
            // string repr is not perfect: continue a dichotomic improvement
            DEBUG_PRINTF(FPUINT_FMT ": %.19g, err_range=%d\n", mantissa, check, err_range);
            if (err_range > 1) {
                err_range >>= 1;
            } else {
                // We have tried all possible mantissa, without finding a reversible repr.
                // Keep the closest one, which is either the first one or the last one.
                if (MICROPY_FLOAT_C_FUN(fabs)(check - f_entry) <= MICROPY_FLOAT_C_FUN(fabs)(first_guess - f_entry)) {
                    // Last guess is the best one
                    DEBUG_PRINTF(FPUINT_FMT ": last guess was the best one\n", mantissa);
                    break;
                }
                // The first guess was the best.
                // Running one more time will get us back to the first repr.
                DEBUG_PRINTF(FPUINT_FMT ": first guess was better, one more time...\n", mantissa);
            }
        }
    }
    #endif

    return (s - buf) + reprlen;
}

#endif // MICROPY_FLOAT_IMPL != MICROPY_FLOAT_IMPL_NONE
