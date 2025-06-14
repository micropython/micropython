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

  The code in this function was inspired from Dave Hylands's previous
  version, which was itself inspired from Fred Bayer's pdouble.c.

  The original code can be found in https://github.com/dhylands/format-float

***********************************************************************/

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

// Internal formatting flags
#define FMT_MODE_E 0x01  // render using scientific notation (%e)
#define FMT_MODE_G 0x02  // render using general format (%g)
#define FMT_MODE_F 0x04  // render using using expanded fixed-point format (%f)
#define FMT_E_CASE 0x20  // don't change this value (used for case conversion!)

static char *mp_prepend_zeroes(char *s, int cnt) {
    *s++ = '0';
    *s++ = '.';
    while (cnt > 0) {
        *s++ = '0';
        cnt--;
    }
    return s;
}

// Helper to convert a decimal mantissa (provided as an mp_float_uint_t) to string
static int mp_format_mantissa(mp_float_uint_t mantissa, mp_float_uint_t mantissa_cap, char *buf, char *s,
    int num_digits, int trailing_zeroes, int dec, int e, int fmt_flags) {

    DEBUG_PRINTF("mantissa=" FPUINT_FMT " exp=%d (cap=" FPUINT_FMT "):\n", mantissa, e, mantissa_cap);

    // If rounding/searching created an extra digit, fix mantissa first
    if (mantissa >= mantissa_cap) {
        if (fmt_flags & FMT_MODE_F) {
            assert(e >= 0);
            num_digits++;
            dec++;
        } else {
            mantissa /= 10;
            e++;
        }
    }

    // When 'g' format is used, replace small exponents by explicit zeroes
    if ((fmt_flags & FMT_MODE_G) && e != 0) {
        if (e >= 0) {
            // If 0 < e < prec, expand positive exponent into trailing zeroes
            if (e < num_digits) {
                dec += e;
                if (dec >= num_digits - 1) {
                    trailing_zeroes = dec - (num_digits - 1);
                }
                e = 0;
            }
        } else {
            // If -4 <= e < 0, expand negative exponent without losing significant digits
            if (e >= -4) {
                int cnt = 0;
                while (e < 0 && !(mantissa % 10)) {
                    mantissa /= 10;
                    cnt++;
                    e++;
                }
                num_digits -= cnt;
                s = mp_prepend_zeroes(s, cnt - e - 1);
                dec = 255;
                e = 0;
            }
        }
    }

    // Convert the integer mantissa to string
    for (int digit = num_digits - 1; digit >= 0; digit--) {
        int digit_ofs = (digit > dec ? digit + 1 : digit);
        s[digit_ofs] = '0' + (int)(mantissa % 10);
        mantissa /= 10;
    }
    int dot = (dec >= 255);
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

    // Append or remove trailing zeroes, as required by format
    if (trailing_zeroes) {
        dec -= num_digits - 1;
        while (trailing_zeroes--) {
            if (!dec--) {
                *s++ = '.';
            }
            *s++ = '0';
        }
    }
    if (fmt_flags & FMT_MODE_G) {
        // 'g' format requires to remove trailing zeroes after decimal point
        if (dot) {
            while (s[-1] == '0') {
                s--;
            }
            if (s[-1] == '.') {
                s--;
            }
        }
    }

    // Append the exponent if needed
    if (((e != 0) || (fmt_flags & FMT_MODE_E)) && !(fmt_flags & FMT_MODE_F)) {
        *s++ = 'E' | (fmt_flags & FMT_E_CASE);
        if (e >= 0) {
            *s++ = '+';
        } else {
            *s++ = '-';
            e = -e;
        }
        if (e >= 100) {
            *s++ = '0' + (e / 100);
        }
        *s++ = '0' + ((e / 10) % 10);
        *s++ = '0' + (e % 10);
    }
    *s = '\0';
    DEBUG_PRINTF("  ===>   %s\n", buf);

    return s - buf;
}

// minimal value expected for buf_size, to avoid checking everywhere for overflow
#define MIN_BUF_SIZE (MAX_MANTISSA_DIGITS + 10)

int mp_format_float(mp_float_t f_entry, char *buf_entry, size_t buf_size, char fmt, int prec, char sign) {

    assert(buf_size >= MIN_BUF_SIZE);

    // Handle sign
    mp_float_t f = f_entry;
    char *buf = buf_entry;
    if (fp_signbit(f_entry) && !fp_isnan(f_entry)) {
        f = -f;
        sign = '-';
    }
    if (sign) {
        *buf++ = sign;
        buf_size--;
    }

    // Handle inf/nan
    char uc = fmt & 0x20;
    {
        char *s = buf;
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
            return s - buf_entry;
        }
    }

    // Decode format character
    int flags = (unsigned char)uc;  // setup FMT_E_CASE, clear all other bits
    char lofmt = (char)(fmt | 0x20);  // fmt in lowercase
    if (lofmt == 'f') {
        flags |= FMT_MODE_F;
    } else if (lofmt == 'g') {
        flags |= FMT_MODE_G;
    } else {
        flags |= FMT_MODE_E;
    }

    // Precompute the exact decimal exponent of f
    int f_e = 0;
    if (!fp_iszero(f)) {
        // Approximate power of 10 exponent from binary exponent.
        // abs(f_e) is lower bound on abs(power of 10 exponent).
        f_e = (int)(fp_expval(f) * FPCONST(0.3010299956639812));  // 1/log2(10).
        if (fp_isless1(f)) {
            // Negative exponent
            mp_float_t u_base = MICROPY_FLOAT_C_FUN(pow)(10, f_e);
            while (u_base > f) {
                --f_e;
                u_base = MICROPY_FLOAT_C_FUN(pow)(10, f_e);
            }
        } else {
            // Positive exponent
            mp_float_t next_u = MICROPY_FLOAT_C_FUN(pow)(10, f_e + 1);
            while (f >= next_u) {
                ++f_e;
                next_u = MICROPY_FLOAT_C_FUN(pow)(10, f_e + 1);
            }
        }
    }

    #if MICROPY_FLOAT_HIGH_QUALITY_REPR
    // When looking for an exact representation with regard to the parse function,
    // the caller can provide a list of precisions to be tried.
    // This is the restart point when changing precision setting.
    int prec_next;
try_again:
    if (IS_PRECLIST(prec)) {
        prec_next = PRECLIST_QUEUE(prec);
        prec = PRECLIST_HEAD(prec);
    } else {
        prec_next = 0;
    }
    #endif
    if (prec < 0) {
        prec = 6;
    }

    // For 'e' format, prec is # digits after the decimal
    // For 'f' format, prec is # digits after the decimal
    // For 'g' format, prec is the max number of significant digits
    //
    // For 'e' & 'g' format, there will be a single digit before the decimal
    // for 'f' format, there will be e digits before the decimal

    char *s = buf;
    int fmt_flags = flags;
    int max_digits = ((flags & FMT_MODE_F) ? buf_size - 2 : buf_size - FPMIN_BUF_SIZE - 1);
    int num_digits = ((flags & FMT_MODE_G) && prec > 0 ? prec : prec + 1);
    if (num_digits > max_digits) {
        num_digits = max_digits;
    }
    int e = f_e;
    int decexp = num_digits - e - 1;
    int dec = 0;
    int trailing_zeroes = 0;

    if (fp_iszero(f)) {
        // no need for scaling 0.0
        decexp = 0;
    } else if (flags & FMT_MODE_F) {
        // Format 'f' requires to expand zeroes instead of using an exponent
        if (fp_isless1(f)) {
            // Negative exponent
            // We should keep a single leading zero in the mantissa, as
            // using more would waste precious digits needed for accuracy.
            num_digits--;
            decexp = num_digits;
            if (num_digits >= 2 && e <= -2) {
                // Insert explicit leading zeroes, and fix exponent accordingly
                int cnt = (-e < decexp ? -e : decexp) - 2;
                num_digits -= cnt;
                s = mp_prepend_zeroes(s, cnt);
                dec = 255; // don't add a decimal dot
            } else {
                // Precision is zero or one, work directly on the mantissa
                num_digits++;
                dec = 0;
            }
        } else {
            // Positive exponent
            // Make sur that the buffer is big enough for that all zeroes
            if (num_digits + e + 2 < (int)buf_size) {
                // Prepare to add trailing zeroes
                num_digits += e;
                decexp += e;
                dec = e;
                if (num_digits > MAX_MANTISSA_DIGITS) {
                    trailing_zeroes = num_digits - MAX_MANTISSA_DIGITS;
                }
            } else {
                // Unable to use 'f' format given buf_size, fallback to 'g' format
                fmt_flags ^= FMT_MODE_F;
                fmt_flags |= FMT_MODE_G;
            }
        }
    }

    // Make sure we will never overflow the integer mantissa
    if (num_digits > MAX_MANTISSA_DIGITS) {
        // Discard digits beyond mantissa size
        decexp -= num_digits - MAX_MANTISSA_DIGITS;
        num_digits = MAX_MANTISSA_DIGITS;
    }

    // At this point,
    // - buf points to beginning of output buffer for the unsigned representation
    // - num_digits == the number of mantissa digits to add
    // - (dec + 1) == the number of digits to print before adding a decimal point
    // - decexp == the power of 10 exponent to apply to f to get the decimal mantissa
    // - e == the power of 10 exponent to append ('e' or 'g' format)
    mp_float_uint_t mantissa_cap = 10;
    for (int n = 1; n < num_digits; n++) {
        mantissa_cap *= 10;
    }

    // Build the decimal mantissa into an uint
    mp_float_t mantissa_flt = mp_decimal_exp(f, decexp);
    mp_float_uint_t mantissa = (mp_float_uint_t)(mantissa_flt + FPCONST(0.5));
    DEBUG_PRINTF("input=%.19g fmt=%c num_digits=%d decexp=%d e=%d mantissa=" FPUINT_FMT "\n", (double)f, lofmt, num_digits, decexp, e, mantissa);

    if (num_digits >= MAX_MANTISSA_DIGITS - 1) {
        // When using high-precision repr, the last two digits carry a high uncertainty
        // We have a chance to easily round the number within the uncertainty range, so use it
        unsigned epsilon = (unsigned)(mantissa % MANTISSA_ROUND);
        if (epsilon) {
            if (epsilon < 100u) {
                mantissa -= epsilon;
                DEBUG_PRINTF("-> round down mantissa to " FPUINT_FMT "\n", mantissa);
            } else if (epsilon > MANTISSA_ROUND - 100u) {
                mantissa += MANTISSA_ROUND - epsilon;
                DEBUG_PRINTF("-> round up mantissa to " FPUINT_FMT "\n", mantissa);
            }
        }
    }

    // Finally convert the decimal mantissa to a floating-point string, according to formatting rules
    int reprlen = mp_format_mantissa(mantissa, mantissa_cap, buf, s, num_digits, trailing_zeroes, dec, e, fmt_flags);
    assert(reprlen + 1 <= (int)buf_size);

    #if MICROPY_FLOAT_HIGH_QUALITY_REPR
    // The initial decimal mantissa might not have been be completely accurate due
    // to the previous loating point operations. The best way to verify this is to
    // parse the resulting number and compare against the original
    mp_float_t check;
    DEBUG_PRINTF("input=%.19g, compare to float('%s')\n", (double)f, buf);
    mp_parse_float_internal(buf, reprlen, &check);
    mp_float_t first_guess = check;

    if (check == f) {
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
        while (check != f && maxruns-- > 0) {
            // update mantissa according to dichotomic search
            if (check < f) {
                mantissa += err_range;
            } else {
                // mantissa is expected to always have more significant digits than err_range
                assert(mantissa >= err_range);
                mantissa -= err_range;
            }
            // retry conversion
            reprlen = mp_format_mantissa(mantissa, mantissa_cap, buf, s, num_digits, trailing_zeroes, dec, e, fmt_flags);
            assert(reprlen + 1 <= (int)buf_size);
            DEBUG_PRINTF("input=%.19g, compare to float('%s')\n", (double)f, buf);
            mp_parse_float_internal(buf, reprlen, &check);
            DEBUG_PRINTF("check=%.19g num_digits=%d e=%d mantissa=" FPUINT_FMT "\n", (double)check, num_digits, e, mantissa);
            if (check == f) {
                // we have a perfect match
                DEBUG_PRINTF(FPUINT_FMT ": perfect match\n", mantissa);
                break;
            }
            // string repr is not perfect: continue a dichotomic improvement
            DEBUG_PRINTF(FPUINT_FMT ": %.19g, err_range=%d\n", mantissa, (double)check, err_range);
            if (err_range > 1) {
                err_range >>= 1;
            } else {
                // We have tried all possible mantissa, without finding a reversible repr.
                // Check if we have an alternate precision to try.
                if (prec_next) {
                    DEBUG_PRINTF("Failed to find a perfect match, try again (prec=0x%x)\n", prec);
                    prec = prec_next;
                    goto try_again;
                }
                // Otherwise, keep the closest one, which is either the first one or the last one.
                if (MICROPY_FLOAT_C_FUN(fabs)(check - f) <= MICROPY_FLOAT_C_FUN(fabs)(first_guess - f)) {
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

    return buf + reprlen - buf_entry;
}

#endif // MICROPY_FLOAT_IMPL != MICROPY_FLOAT_IMPL_NONE
