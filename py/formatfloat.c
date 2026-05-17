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

#if MICROPY_FLOAT_FORMAT_IMPL == MICROPY_FLOAT_FORMAT_IMPL_EXACT || MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE
#define MP_FFUINT_FMT "%lu"
#else
#define MP_FFUINT_FMT "%u"
#endif

static inline int fp_expval(mp_float_t x) {
    mp_float_union_t fb = { x };
    return (int)fb.p.exp - MP_FLOAT_EXP_OFFSET;
}

#if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE

static inline int fp_isless1(mp_float_t x) {
    return x < 1.0;
}

static inline int fp_iszero(mp_float_t x) {
    return x == 0.0;
}

#if MICROPY_FLOAT_FORMAT_IMPL != MICROPY_FLOAT_FORMAT_IMPL_APPROX
static inline int fp_equal(mp_float_t x, mp_float_t y) {
    return x == y;
}
#else
static inline mp_float_t fp_diff(mp_float_t x, mp_float_t y) {
    return x - y;
}
#endif

#elif MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT

// The functions below are roughly equivalent to the ones above,
// but they are optimized to reduce code footprint by skipping
// handling for special values such as nan, inf, +/-0.0
// for ports where FP support is done in software.
//
// They also take into account lost bits of REPR_C as needed.

static inline int fp_isless1(mp_float_t x) {
    mp_float_union_t fb = { x };
    return fb.i < 0x3f800000;
}

static inline int fp_iszero(mp_float_t x) {
    mp_float_union_t x_check = { x };
    return !x_check.i; // this is valid for REPR_C as well
}

#if MICROPY_FLOAT_FORMAT_IMPL != MICROPY_FLOAT_FORMAT_IMPL_APPROX
static inline int fp_equal(mp_float_t x, mp_float_t y) {
    mp_float_union_t x_check = { x };
    mp_float_union_t y_check = { y };
    #if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_C
    return (x_check.i & ~3) == (y_check.i & ~3);
    #else
    return x_check.i == y_check.i;
    #endif
}
#else
static inline mp_float_t fp_diff(mp_float_t x, mp_float_t y) {
    #if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_C
    mp_float_union_t x_check = { x };
    mp_float_union_t y_check = { y };
    x_check.i &= ~3;
    y_check.i &= ~3;
    return x_check.f - y_check.f;
    #else
    return x - y;
    #endif
}
#endif

#endif

#if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
#define FPMIN_BUF_SIZE 6 // +9e+99
#define MAX_MANTISSA_DIGITS  (9)
#define SAFE_MANTISSA_DIGITS (6)
#elif MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE
#define FPMIN_BUF_SIZE 7 // +9e+199
#define MAX_MANTISSA_DIGITS  (19)
#define SAFE_MANTISSA_DIGITS (16)
#endif

// Internal formatting flags
#define FMT_MODE_E 0x01  // render using scientific notation (%e)
#define FMT_MODE_G 0x02  // render using general format (%g)
#define FMT_MODE_F 0x04  // render using using expanded fixed-point format (%f)
#define FMT_E_CASE 0x20  // don't change this value (used for case conversion!)

static char *mp_prepend_zeros(char *s, int cnt) {
    *s++ = '0';
    *s++ = '.';
    while (cnt > 0) {
        *s++ = '0';
        cnt--;
    }
    return s;
}

// Helper to convert a decimal mantissa (provided as an mp_large_float_uint_t) to string
static int mp_format_mantissa(mp_large_float_uint_t mantissa, mp_large_float_uint_t mantissa_cap, char *buf, char *s,
    int num_digits, int max_exp_zeros, int trailing_zeros, int dec, int e, int fmt_flags) {

    DEBUG_PRINTF("mantissa=" MP_FFUINT_FMT " exp=%d (cap=" MP_FFUINT_FMT "):\n", mantissa, e, mantissa_cap);

    if (mantissa) {
        // If rounding/searching created an extra digit or removed too many, fix mantissa first
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
    }

    // When 'g' format is used, replace small exponents by explicit zeros
    if ((fmt_flags & FMT_MODE_G) && e != 0) {
        if (e >= 0) {
            // If 0 < e < max_exp_zeros, expand positive exponent into trailing zeros
            if (e < max_exp_zeros) {
                dec += e;
                if (dec >= num_digits) {
                    trailing_zeros = dec - (num_digits - 1);
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
                s = mp_prepend_zeros(s, cnt - e - 1);
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
    DEBUG_PRINTF("  =      %s exp=%d num_digits=%d zeros=%d dec=%d\n", buf, e, num_digits, trailing_zeros, dec);
    #endif

    // Append or remove trailing zeros, as required by format
    if (trailing_zeros) {
        dec -= num_digits - 1;
        while (trailing_zeros--) {
            if (!dec--) {
                *s++ = '.';
                dot = 1;
            }
            *s++ = '0';
        }
    }
    if (fmt_flags & FMT_MODE_G) {
        // 'g' format requires to remove trailing zeros after decimal point
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
    if (signbit(f_entry) && !isnan(f_entry)) {
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
        if (isinf(f)) {
            *s++ = 'I' ^ uc;
            *s++ = 'N' ^ uc;
            *s++ = 'F' ^ uc;
            goto ret;
        } else if (isnan(f)) {
            *s++ = 'N' ^ uc;
            *s++ = 'A' ^ uc;
            *s++ = 'N' ^ uc;
        ret:
            *s = '\0';
            return s - buf_entry;
        }
    }

    // Decode format character
    int fmt_flags = (unsigned char)uc;  // setup FMT_E_CASE, clear all other bits
    char lofmt = (char)(fmt | 0x20);  // fmt in lowercase
    if (lofmt == 'f') {
        fmt_flags |= FMT_MODE_F;
    } else if (lofmt == 'g') {
        fmt_flags |= FMT_MODE_G;
    } else {
        fmt_flags |= FMT_MODE_E;
    }

    // When precision is unspecified, default to 6
    if (prec < 0) {
        prec = 6;
    }
    // Use high precision for `repr`, but switch to exponent mode
    // after 16 digits in any case to match CPython behaviour
    int max_exp_zeros = (prec < (int)buf_size - 3 ? prec : (int)buf_size - 3);
    if (prec == MP_FLOAT_REPR_PREC) {
        prec = MAX_MANTISSA_DIGITS;
        max_exp_zeros = 16;
    }

    // Precompute the exact decimal exponent of f, such that
    // abs(e) is lower bound on abs(power of 10 exponent).
    int e = 0;
    if (!fp_iszero(f)) {
        // Approximate power of 10 exponent from binary exponent.
        e = (int)(fp_expval(f) * MICROPY_FLOAT_CONST(0.3010299956639812));  // 1/log2(10).
        int positive_exp = !fp_isless1(f);
        mp_float_t u_base = (mp_float_t)mp_decimal_exp((mp_large_float_t)1.0, e + positive_exp);
        while ((f >= u_base) == positive_exp) {
            e += (positive_exp ? 1 : -1);
            u_base = (mp_float_t)mp_decimal_exp((mp_large_float_t)1.0, e + positive_exp);
        }
    }

    // For 'e' format, prec is # digits after the decimal
    // For 'f' format, prec is # digits after the decimal
    // For 'g' format, prec is the max number of significant digits
    //
    // For 'e' & 'g' format, there will be a single digit before the decimal
    // For 'f' format, zeros must be expanded instead of using an exponent.
    // Make sure there is enough room in the buffer for them, or switch to format 'g'.
    if ((fmt_flags & FMT_MODE_F) && e > 0) {
        int req_size = e + prec + 2;
        if (req_size > (int)buf_size) {
            fmt_flags ^= FMT_MODE_F;
            fmt_flags |= FMT_MODE_G;
            prec++;
        }
    }

    // To work independently of the format, we precompute:
    // - the max number of significant digits to produce
    // - the number of leading zeros to prepend (mode f only)
    // - the number of trailing zeros to append
    int max_digits = prec;
    int lead_zeros = 0;
    int trail_zeros = 0;
    if (fmt_flags & FMT_MODE_F) {
        if (max_digits > (int)buf_size - 3) {
            // cannot satisfy requested number of decimals given buf_size, sorry
            max_digits = (int)buf_size - 3;
        }
        if (e < 0) {
            if (max_digits > 2 && e < -2) {
                // Insert explicit leading zeros
                lead_zeros = (-e < max_digits ? -e : max_digits) - 2;
                max_digits -= lead_zeros;
            } else {
                max_digits++;
            }
        } else {
            max_digits += e + 1;
        }
    } else {
        if (!(fmt_flags & FMT_MODE_G) || max_digits == 0) {
            max_digits++;
        }
    }
    if (max_digits > MAX_MANTISSA_DIGITS) {
        // use trailing zeros to avoid overflowing the mantissa
        trail_zeros = max_digits - MAX_MANTISSA_DIGITS;
        max_digits = MAX_MANTISSA_DIGITS;
    }
    int overhead = (fmt_flags & FMT_MODE_F ? 3 : FPMIN_BUF_SIZE + 1);
    if (trail_zeros > (int)buf_size - max_digits - overhead) {
        // cannot satisfy requested number of decimals given buf_size, sorry
        trail_zeros = (int)buf_size - max_digits - overhead;
    }

    // When the caller asks for more precision than available for sure,
    // Look for a shorter (rounded) representation first, and only dig
    // into more digits if there is no short representation.
    int num_digits = (SAFE_MANTISSA_DIGITS < max_digits ? SAFE_MANTISSA_DIGITS : max_digits);
try_again:
    ;

    char *s = buf;
    int extra_zeros = trail_zeros + (max_digits - num_digits);
    int decexp;
    int dec = 0;

    if (fp_iszero(f)) {
        // no need for scaling 0.0
        decexp = 0;
    } else if (fmt_flags & FMT_MODE_F) {
        decexp = num_digits - 1;
        if (e < 0) {
            // Negative exponent: we keep a single leading zero in the mantissa,
            // as using more would waste precious digits needed for accuracy.
            if (lead_zeros > 0) {
                // We are using leading zeros
                s = mp_prepend_zeros(s, lead_zeros);
                decexp += lead_zeros + 1;
                dec = 255; // no decimal dot
            } else {
                // Small negative exponent, work directly on the mantissa
                dec = 0;
            }
        } else {
            // Positive exponent: we will add trailing zeros separately
            decexp -= e;
            dec = e;
        }
    } else {
        decexp = num_digits - e - 1;
    }
    DEBUG_PRINTF("input=%.19g e=%d fmt=%c max_d=%d num_d=%d decexp=%d dec=%d l0=%d r0=%d\n",
        (double)f, e, lofmt, max_digits, num_digits, decexp, dec, lead_zeros, extra_zeros);

    // At this point,
    // - buf points to beginning of output buffer for the unsigned representation
    // - num_digits == the number of mantissa digits to add
    // - (dec + 1) == the number of digits to print before adding a decimal point
    // - decexp == the power of 10 exponent to apply to f to get the decimal mantissa
    // - e == the power of 10 exponent to append ('e' or 'g' format)
    mp_large_float_uint_t mantissa_cap = 10;
    for (int n = 1; n < num_digits; n++) {
        mantissa_cap *= 10;
    }

    // Build the decimal mantissa into a large uint
    mp_large_float_uint_t mantissa = 1;
    if (sizeof(mp_large_float_t) == sizeof(mp_float_t) && num_digits > SAFE_MANTISSA_DIGITS && decexp > 1) {
        // if we don't have large floats, use integer multiply to produce the last digits
        if (num_digits > SAFE_MANTISSA_DIGITS + 1 && decexp > 2) {
            mantissa = 100;
            decexp -= 2;
        } else {
            mantissa = 10;
            decexp -= 1;
        }
    }
    mp_large_float_t mantissa_f = mp_decimal_exp((mp_large_float_t)f, decexp);
    mantissa *= (mp_large_float_uint_t)(mantissa_f + (mp_large_float_t)0.5);
    DEBUG_PRINTF("input=%.19g fmt=%c num_digits=%d dec=%d mantissa=" MP_FFUINT_FMT " r0=%d\n", (double)f, lofmt, num_digits, dec, mantissa, extra_zeros);

    // Finally convert the decimal mantissa to a floating-point string, according to formatting rules
    int reprlen = mp_format_mantissa(mantissa, mantissa_cap, buf, s, num_digits, max_exp_zeros, extra_zeros, dec, e, fmt_flags);
    assert(reprlen + 1 <= (int)buf_size);

    #if MICROPY_FLOAT_FORMAT_IMPL != MICROPY_FLOAT_FORMAT_IMPL_APPROX

    if (num_digits < max_digits) {
        // The initial precision might not be sufficient for an exact representation
        // for all numbers. If the result is not exact, restart using next precision.
        // parse the resulting number and compare against the original
        mp_float_t check;
        DEBUG_PRINTF("input=%.19g, compare to float('%s')\n", (double)f, buf);
        mp_parse_float_internal(buf, reprlen, &check);
        if (!fp_equal(check, f)) {
            num_digits++;
            DEBUG_PRINTF("Not perfect, retry using more digits (%d)\n", num_digits);
            goto try_again;
        }
    }

    #else

    // The initial decimal mantissa might not have been be completely accurate due
    // to the previous loating point operations. The best way to verify this is to
    // parse the resulting number and compare against the original
    mp_float_t check;
    DEBUG_PRINTF("input=%.19g, compare to float('%s')\n", (double)f, buf);
    mp_parse_float_internal(buf, reprlen, &check);
    mp_float_t diff = fp_diff(check, f);
    mp_float_t best_diff = diff;
    mp_large_float_uint_t best_mantissa = mantissa;

    if (fp_iszero(diff)) {
        // we have a perfect match
        DEBUG_PRINTF(MP_FFUINT_FMT ": perfect match (direct)\n", mantissa);
    } else {
        // In order to get the best possible representation, we will perform a
        // dichotomic search for a reversible representation.
        // This will also provide optimal rounding on the fly.
        unsigned err_range = 1;
        if (num_digits > SAFE_MANTISSA_DIGITS) {
            err_range <<= 3 * (num_digits - SAFE_MANTISSA_DIGITS);
        }
        int maxruns = 3 + 3 * (MAX_MANTISSA_DIGITS - SAFE_MANTISSA_DIGITS);
        while (maxruns-- > 0) {
            // update mantissa according to dichotomic search
            if (signbit(diff)) {
                mantissa += err_range;
            } else {
                // mantissa is expected to always have more significant digits than err_range
                assert(mantissa >= err_range);
                mantissa -= err_range;
            }
            // retry conversion
            reprlen = mp_format_mantissa(mantissa, mantissa_cap, buf, s, num_digits, max_exp_zeros, extra_zeros, dec, e, fmt_flags);
            assert(reprlen + 1 <= (int)buf_size);
            DEBUG_PRINTF("input=%.19g, compare to float('%s')\n", (double)f, buf);
            mp_parse_float_internal(buf, reprlen, &check);
            DEBUG_PRINTF("check=%.19g num_digits=%d e=%d mantissa=" MP_FFUINT_FMT "\n", (double)check, num_digits, e, mantissa);
            diff = fp_diff(check, f);
            if (fp_iszero(diff)) {
                // we have a perfect match
                DEBUG_PRINTF(MP_FFUINT_FMT ": perfect match\n", mantissa);
                break;
            }
            // keep track of our best estimate
            mp_float_t delta = MICROPY_FLOAT_C_FUN(fabs)(diff) - MICROPY_FLOAT_C_FUN(fabs)(best_diff);
            if (signbit(delta) || (fp_iszero(delta) && !(mantissa % 10u))) {
                best_diff = diff;
                best_mantissa = mantissa;
            }
            // string repr is not perfect: continue a dichotomic improvement
            DEBUG_PRINTF(MP_FFUINT_FMT ": %.19g, err_range=%d\n", mantissa, (double)check, err_range);
            if (err_range > 1) {
                err_range >>= 1;
            } else {
                // We have tried all possible mantissa, without finding a reversible repr.
                // Check if we have an alternate precision to try.
                if (num_digits < max_digits) {
                    num_digits++;
                    DEBUG_PRINTF("Failed to find a perfect match, try with more digits (%d)\n", num_digits);
                    goto try_again;
                }
                // Otherwise, keep the closest one, which is either the first one or the last one.
                if (mantissa == best_mantissa) {
                    // Last guess is the best one
                    DEBUG_PRINTF(MP_FFUINT_FMT ": last guess was the best one\n", mantissa);
                } else {
                    // We had a better guess earlier
                    DEBUG_PRINTF(MP_FFUINT_FMT ": use best guess\n", mantissa);
                    reprlen = mp_format_mantissa(best_mantissa, mantissa_cap, buf, s, num_digits, max_exp_zeros, extra_zeros, dec, e, fmt_flags);
                }
                break;
            }
        }
    }
    #endif

    return buf + reprlen - buf_entry;
}

#endif // MICROPY_FLOAT_IMPL != MICROPY_FLOAT_IMPL_NONE
