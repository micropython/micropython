/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
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

#include <string.h>
#include <assert.h>

#include "py/mpz.h"

#if MICROPY_LONGINT_IMPL == MICROPY_LONGINT_IMPL_MPZ

#define DIG_SIZE (MPZ_DIG_SIZE)
#define DIG_MASK ((MPZ_LONG_1 << DIG_SIZE) - 1)
#define DIG_MSB  (MPZ_LONG_1 << (DIG_SIZE - 1))
#define DIG_BASE (MPZ_LONG_1 << DIG_SIZE)

/*
 mpz is an arbitrary precision integer type with a public API.

 mpn functions act on non-negative integers represented by an array of generalised
 digits (eg a word per digit).  You also need to specify separately the length of the
 array.  There is no public API for mpn.  Rather, the functions are used by mpz to
 implement its features.

 Integer values are stored little endian (first digit is first in memory).

 Definition of normalise: ?
*/

STATIC size_t mpn_remove_trailing_zeros(mpz_dig_t *oidig, mpz_dig_t *idig) {
    for (--idig; idig >= oidig && *idig == 0; --idig) {
    }
    return idig + 1 - oidig;
}

/* compares i with j
   returns sign(i - j)
   assumes i, j are normalised
*/
STATIC int mpn_cmp(const mpz_dig_t *idig, size_t ilen, const mpz_dig_t *jdig, size_t jlen) {
    if (ilen < jlen) {
        return -1;
    }
    if (ilen > jlen) {
        return 1;
    }

    for (idig += ilen, jdig += ilen; ilen > 0; --ilen) {
        mpz_dbl_dig_signed_t cmp = (mpz_dbl_dig_t)*(--idig) - (mpz_dbl_dig_t)*(--jdig);
        if (cmp < 0) {
            return -1;
        }
        if (cmp > 0) {
            return 1;
        }
    }

    return 0;
}

/* computes i = j << n
   returns number of digits in i
   assumes enough memory in i; assumes normalised j; assumes n > 0
   can have i, j pointing to same memory
*/
STATIC size_t mpn_shl(mpz_dig_t *idig, mpz_dig_t *jdig, size_t jlen, mp_uint_t n) {
    mp_uint_t n_whole = (n + DIG_SIZE - 1) / DIG_SIZE;
    mp_uint_t n_part = n % DIG_SIZE;
    if (n_part == 0) {
        n_part = DIG_SIZE;
    }

    // start from the high end of the digit arrays
    idig += jlen + n_whole - 1;
    jdig += jlen - 1;

    // shift the digits
    mpz_dbl_dig_t d = 0;
    for (size_t i = jlen; i > 0; i--, idig--, jdig--) {
        d |= *jdig;
        *idig = (d >> (DIG_SIZE - n_part)) & DIG_MASK;
        d <<= DIG_SIZE;
    }

    // store remaining bits
    *idig = (d >> (DIG_SIZE - n_part)) & DIG_MASK;
    idig -= n_whole - 1;
    memset(idig, 0, (n_whole - 1) * sizeof(mpz_dig_t));

    // work out length of result
    jlen += n_whole;
    while (jlen != 0 && idig[jlen - 1] == 0) {
        jlen--;
    }

    // return length of result
    return jlen;
}

/* computes i = j >> n
   returns number of digits in i
   assumes enough memory in i; assumes normalised j; assumes n > 0
   can have i, j pointing to same memory
*/
STATIC size_t mpn_shr(mpz_dig_t *idig, mpz_dig_t *jdig, size_t jlen, mp_uint_t n) {
    mp_uint_t n_whole = n / DIG_SIZE;
    mp_uint_t n_part = n % DIG_SIZE;

    if (n_whole >= jlen) {
        return 0;
    }

    jdig += n_whole;
    jlen -= n_whole;

    for (size_t i = jlen; i > 0; i--, idig++, jdig++) {
        mpz_dbl_dig_t d = *jdig;
        if (i > 1) {
            d |= (mpz_dbl_dig_t)jdig[1] << DIG_SIZE;
        }
        d >>= n_part;
        *idig = d & DIG_MASK;
    }

    if (idig[-1] == 0) {
        jlen--;
    }

    return jlen;
}

/* computes i = j + k
   returns number of digits in i
   assumes enough memory in i; assumes normalised j, k; assumes jlen >= klen
   can have i, j, k pointing to same memory
*/
STATIC size_t mpn_add(mpz_dig_t *idig, const mpz_dig_t *jdig, size_t jlen, const mpz_dig_t *kdig, size_t klen) {
    mpz_dig_t *oidig = idig;
    mpz_dbl_dig_t carry = 0;

    jlen -= klen;

    for (; klen > 0; --klen, ++idig, ++jdig, ++kdig) {
        carry += (mpz_dbl_dig_t)*jdig + (mpz_dbl_dig_t)*kdig;
        *idig = carry & DIG_MASK;
        carry >>= DIG_SIZE;
    }

    for (; jlen > 0; --jlen, ++idig, ++jdig) {
        carry += *jdig;
        *idig = carry & DIG_MASK;
        carry >>= DIG_SIZE;
    }

    if (carry != 0) {
        *idig++ = carry;
    }

    return idig - oidig;
}

/* computes i = j - k
   returns number of digits in i
   assumes enough memory in i; assumes normalised j, k; assumes j >= k
   can have i, j, k pointing to same memory
*/
STATIC size_t mpn_sub(mpz_dig_t *idig, const mpz_dig_t *jdig, size_t jlen, const mpz_dig_t *kdig, size_t klen) {
    mpz_dig_t *oidig = idig;
    mpz_dbl_dig_signed_t borrow = 0;

    jlen -= klen;

    for (; klen > 0; --klen, ++idig, ++jdig, ++kdig) {
        borrow += (mpz_dbl_dig_t)*jdig - (mpz_dbl_dig_t)*kdig;
        *idig = borrow & DIG_MASK;
        borrow >>= DIG_SIZE;
    }

    for (; jlen > 0; --jlen, ++idig, ++jdig) {
        borrow += *jdig;
        *idig = borrow & DIG_MASK;
        borrow >>= DIG_SIZE;
    }

    return mpn_remove_trailing_zeros(oidig, idig);
}

#if MICROPY_OPT_MPZ_BITWISE

/* computes i = j & k
   returns number of digits in i
   assumes enough memory in i; assumes normalised j, k; assumes jlen >= klen (jlen argument not needed)
   can have i, j, k pointing to same memory
*/
STATIC size_t mpn_and(mpz_dig_t *idig, const mpz_dig_t *jdig, const mpz_dig_t *kdig, size_t klen) {
    mpz_dig_t *oidig = idig;

    for (; klen > 0; --klen, ++idig, ++jdig, ++kdig) {
        *idig = *jdig & *kdig;
    }

    return mpn_remove_trailing_zeros(oidig, idig);
}

#endif

/*  i = -((-j) & (-k))                = ~((~j + 1) & (~k + 1)) + 1
    i =  (j & (-k)) =  (j & (~k + 1)) =  (  j      & (~k + 1))
    i =  ((-j) & k) =  ((~j + 1) & k) =  ((~j + 1) &   k     )
   computes general form:
   i = (im ^ (((j ^ jm) + jc) & ((k ^ km) + kc))) + ic  where Xm = Xc == 0 ? 0 : DIG_MASK
   returns number of digits in i
   assumes enough memory in i; assumes normalised j, k; assumes length j >= length k
   can have i, j, k pointing to same memory
*/
STATIC size_t mpn_and_neg(mpz_dig_t *idig, const mpz_dig_t *jdig, size_t jlen, const mpz_dig_t *kdig, size_t klen,
    mpz_dbl_dig_t carryi, mpz_dbl_dig_t carryj, mpz_dbl_dig_t carryk) {
    mpz_dig_t *oidig = idig;
    mpz_dig_t imask = (0 == carryi) ? 0 : DIG_MASK;
    mpz_dig_t jmask = (0 == carryj) ? 0 : DIG_MASK;
    mpz_dig_t kmask = (0 == carryk) ? 0 : DIG_MASK;

    for (; jlen > 0; ++idig, ++jdig) {
        carryj += *jdig ^ jmask;
        carryk += (--klen <= --jlen) ? (*kdig++ ^ kmask) : kmask;
        carryi += ((carryj & carryk) ^ imask) & DIG_MASK;
        *idig = carryi & DIG_MASK;
        carryk >>= DIG_SIZE;
        carryj >>= DIG_SIZE;
        carryi >>= DIG_SIZE;
    }

    if (0 != carryi) {
        *idig++ = carryi;
    }

    return mpn_remove_trailing_zeros(oidig, idig);
}

#if MICROPY_OPT_MPZ_BITWISE

/* computes i = j | k
   returns number of digits in i
   assumes enough memory in i; assumes normalised j, k; assumes jlen >= klen
   can have i, j, k pointing to same memory
*/
STATIC size_t mpn_or(mpz_dig_t *idig, const mpz_dig_t *jdig, size_t jlen, const mpz_dig_t *kdig, size_t klen) {
    mpz_dig_t *oidig = idig;

    jlen -= klen;

    for (; klen > 0; --klen, ++idig, ++jdig, ++kdig) {
        *idig = *jdig | *kdig;
    }

    for (; jlen > 0; --jlen, ++idig, ++jdig) {
        *idig = *jdig;
    }

    return idig - oidig;
}

#endif

/*  i = -((-j) | (-k))                = ~((~j + 1) | (~k + 1)) + 1
    i = -(j | (-k)) = -(j | (~k + 1)) = ~(  j      | (~k + 1)) + 1
    i = -((-j) | k) = -((~j + 1) | k) = ~((~j + 1) |   k     ) + 1
   computes general form:
   i = ~(((j ^ jm) + jc) | ((k ^ km) + kc)) + 1  where Xm = Xc == 0 ? 0 : DIG_MASK
   returns number of digits in i
   assumes enough memory in i; assumes normalised j, k; assumes length j >= length k
   can have i, j, k pointing to same memory
*/

#if MICROPY_OPT_MPZ_BITWISE

STATIC size_t mpn_or_neg(mpz_dig_t *idig, const mpz_dig_t *jdig, size_t jlen, const mpz_dig_t *kdig, size_t klen,
    mpz_dbl_dig_t carryj, mpz_dbl_dig_t carryk) {
    mpz_dig_t *oidig = idig;
    mpz_dbl_dig_t carryi = 1;
    mpz_dig_t jmask = (0 == carryj) ? 0 : DIG_MASK;
    mpz_dig_t kmask = (0 == carryk) ? 0 : DIG_MASK;

    for (; jlen > 0; ++idig, ++jdig) {
        carryj += *jdig ^ jmask;
        carryk += (--klen <= --jlen) ? (*kdig++ ^ kmask) : kmask;
        carryi += ((carryj | carryk) ^ DIG_MASK) & DIG_MASK;
        *idig = carryi & DIG_MASK;
        carryk >>= DIG_SIZE;
        carryj >>= DIG_SIZE;
        carryi >>= DIG_SIZE;
    }

    // At least one of j,k must be negative so the above for-loop runs at least
    // once.  For carryi to be non-zero here it must be equal to 1 at the end of
    // each iteration of the loop.  So the accumulation of carryi must overflow
    // each time, ie carryi += 0xff..ff.  So carryj|carryk must be 0 in the
    // DIG_MASK bits on each iteration.  But considering all cases of signs of
    // j,k one sees that this is not possible.
    assert(carryi == 0);

    return mpn_remove_trailing_zeros(oidig, idig);
}

#else

STATIC size_t mpn_or_neg(mpz_dig_t *idig, const mpz_dig_t *jdig, size_t jlen, const mpz_dig_t *kdig, size_t klen,
    mpz_dbl_dig_t carryi, mpz_dbl_dig_t carryj, mpz_dbl_dig_t carryk) {
    mpz_dig_t *oidig = idig;
    mpz_dig_t imask = (0 == carryi) ? 0 : DIG_MASK;
    mpz_dig_t jmask = (0 == carryj) ? 0 : DIG_MASK;
    mpz_dig_t kmask = (0 == carryk) ? 0 : DIG_MASK;

    for (; jlen > 0; ++idig, ++jdig) {
        carryj += *jdig ^ jmask;
        carryk += (--klen <= --jlen) ? (*kdig++ ^ kmask) : kmask;
        carryi += ((carryj | carryk) ^ imask) & DIG_MASK;
        *idig = carryi & DIG_MASK;
        carryk >>= DIG_SIZE;
        carryj >>= DIG_SIZE;
        carryi >>= DIG_SIZE;
    }

    // See comment in above mpn_or_neg for why carryi must be 0.
    assert(carryi == 0);

    return mpn_remove_trailing_zeros(oidig, idig);
}

#endif

#if MICROPY_OPT_MPZ_BITWISE

/* computes i = j ^ k
   returns number of digits in i
   assumes enough memory in i; assumes normalised j, k; assumes jlen >= klen
   can have i, j, k pointing to same memory
*/
STATIC size_t mpn_xor(mpz_dig_t *idig, const mpz_dig_t *jdig, size_t jlen, const mpz_dig_t *kdig, size_t klen) {
    mpz_dig_t *oidig = idig;

    jlen -= klen;

    for (; klen > 0; --klen, ++idig, ++jdig, ++kdig) {
        *idig = *jdig ^ *kdig;
    }

    for (; jlen > 0; --jlen, ++idig, ++jdig) {
        *idig = *jdig;
    }

    return mpn_remove_trailing_zeros(oidig, idig);
}

#endif

/*  i = (-j) ^ (-k) = ~(j - 1) ^ ~(k - 1)                   = (j - 1) ^ (k - 1)
    i = -(j ^ (-k)) = -(j ^ ~(k - 1)) = ~(j ^ ~(k - 1)) + 1 = (j ^ (k - 1)) + 1
    i = -((-j) ^ k) = -(~(j - 1) ^ k) = ~(~(j - 1) ^ k) + 1 = ((j - 1) ^ k) + 1
   computes general form:
   i = ((j - 1 + jc) ^ (k - 1 + kc)) + ic
   returns number of digits in i
   assumes enough memory in i; assumes normalised j, k; assumes length j >= length k
   can have i, j, k pointing to same memory
*/
STATIC size_t mpn_xor_neg(mpz_dig_t *idig, const mpz_dig_t *jdig, size_t jlen, const mpz_dig_t *kdig, size_t klen,
    mpz_dbl_dig_t carryi, mpz_dbl_dig_t carryj, mpz_dbl_dig_t carryk) {
    mpz_dig_t *oidig = idig;

    for (; jlen > 0; ++idig, ++jdig) {
        carryj += *jdig + DIG_MASK;
        carryk += (--klen <= --jlen) ? (*kdig++ + DIG_MASK) : DIG_MASK;
        carryi += (carryj ^ carryk) & DIG_MASK;
        *idig = carryi & DIG_MASK;
        carryk >>= DIG_SIZE;
        carryj >>= DIG_SIZE;
        carryi >>= DIG_SIZE;
    }

    if (0 != carryi) {
        *idig++ = carryi;
    }

    return mpn_remove_trailing_zeros(oidig, idig);
}

/* computes i = i * d1 + d2
   returns number of digits in i
   assumes enough memory in i; assumes normalised i; assumes dmul != 0
*/
STATIC size_t mpn_mul_dig_add_dig(mpz_dig_t *idig, size_t ilen, mpz_dig_t dmul, mpz_dig_t dadd) {
    mpz_dig_t *oidig = idig;
    mpz_dbl_dig_t carry = dadd;

    for (; ilen > 0; --ilen, ++idig) {
        carry += (mpz_dbl_dig_t)*idig * (mpz_dbl_dig_t)dmul; // will never overflow so long as DIG_SIZE <= 8*sizeof(mpz_dbl_dig_t)/2
        *idig = carry & DIG_MASK;
        carry >>= DIG_SIZE;
    }

    if (carry != 0) {
        *idig++ = carry;
    }

    return idig - oidig;
}

/* computes i = j * k
   returns number of digits in i
   assumes enough memory in i; assumes i is zeroed; assumes normalised j, k
   can have j, k point to same memory
*/
STATIC size_t mpn_mul(mpz_dig_t *idig, mpz_dig_t *jdig, size_t jlen, mpz_dig_t *kdig, size_t klen) {
    mpz_dig_t *oidig = idig;
    size_t ilen = 0;

    for (; klen > 0; --klen, ++idig, ++kdig) {
        mpz_dig_t *id = idig;
        mpz_dbl_dig_t carry = 0;

        size_t jl = jlen;
        for (mpz_dig_t *jd = jdig; jl > 0; --jl, ++jd, ++id) {
            carry += (mpz_dbl_dig_t)*id + (mpz_dbl_dig_t)*jd * (mpz_dbl_dig_t)*kdig; // will never overflow so long as DIG_SIZE <= 8*sizeof(mpz_dbl_dig_t)/2
            *id = carry & DIG_MASK;
            carry >>= DIG_SIZE;
        }

        if (carry != 0) {
            *id++ = carry;
        }

        ilen = id - oidig;
        // check to prevent usb starvation
        #ifdef RUN_BACKGROUND_TASKS
        RUN_BACKGROUND_TASKS;
        #endif
    }

    return ilen;
}

/* natural_div - quo * den + new_num = old_num (ie num is replaced with rem)
   assumes den != 0
   assumes num_dig has enough memory to be extended by 1 digit
   assumes quo_dig has enough memory (as many digits as num)
   assumes quo_dig is filled with zeros
*/
STATIC void mpn_div(mpz_dig_t *num_dig, size_t *num_len, const mpz_dig_t *den_dig, size_t den_len, mpz_dig_t *quo_dig, size_t *quo_len) {
    mpz_dig_t *orig_num_dig = num_dig;
    mpz_dig_t *orig_quo_dig = quo_dig;
    mpz_dig_t norm_shift = 0;
    mpz_dbl_dig_t lead_den_digit;

    // handle simple cases
    {
        int cmp = mpn_cmp(num_dig, *num_len, den_dig, den_len);
        if (cmp == 0) {
            *num_len = 0;
            quo_dig[0] = 1;
            *quo_len = 1;
            return;
        } else if (cmp < 0) {
            // numerator remains the same
            *quo_len = 0;
            return;
        }
    }

    // We need to normalise the denominator (leading bit of leading digit is 1)
    // so that the division routine works.  Since the denominator memory is
    // read-only we do the normalisation on the fly, each time a digit of the
    // denominator is needed.  We need to know is how many bits to shift by.

    // count number of leading zeros in leading digit of denominator
    {
        mpz_dig_t d = den_dig[den_len - 1];
        while ((d & DIG_MSB) == 0) {
            d <<= 1;
            ++norm_shift;
        }
    }

    // now need to shift numerator by same amount as denominator
    // first, increase length of numerator in case we need more room to shift
    num_dig[*num_len] = 0;
    ++(*num_len);
    for (mpz_dig_t *num = num_dig, carry = 0; num < num_dig + *num_len; ++num) {
        mpz_dig_t n = *num;
        *num = ((n << norm_shift) | carry) & DIG_MASK;
        carry = (mpz_dbl_dig_t)n >> (DIG_SIZE - norm_shift);
    }

    // cache the leading digit of the denominator
    lead_den_digit = (mpz_dbl_dig_t)den_dig[den_len - 1] << norm_shift;
    if (den_len >= 2) {
        lead_den_digit |= (mpz_dbl_dig_t)den_dig[den_len - 2] >> (DIG_SIZE - norm_shift);
    }

    // point num_dig to last digit in numerator
    num_dig += *num_len - 1;

    // calculate number of digits in quotient
    *quo_len = *num_len - den_len;

    // point to last digit to store for quotient
    quo_dig += *quo_len - 1;

    // keep going while we have enough digits to divide
    while (*num_len > den_len) {
        mpz_dbl_dig_t quo = ((mpz_dbl_dig_t)*num_dig << DIG_SIZE) | num_dig[-1];

        // get approximate quotient
        quo /= lead_den_digit;

        // Multiply quo by den and subtract from num to get remainder.
        // Must be careful with overflow of the borrow variable.  Both
        // borrow and low_digs are signed values and need signed right-shift,
        // but x is unsigned and may take a full-range value.
        const mpz_dig_t *d = den_dig;
        mpz_dbl_dig_t d_norm = 0;
        mpz_dbl_dig_signed_t borrow = 0;
        for (mpz_dig_t *n = num_dig - den_len; n < num_dig; ++n, ++d) {
            // Get the next digit in (den).
            d_norm = ((mpz_dbl_dig_t)*d << norm_shift) | (d_norm >> DIG_SIZE);
            // Multiply the next digit in (quo * den).
            mpz_dbl_dig_t x = (mpz_dbl_dig_t)quo * (d_norm & DIG_MASK);
            // Compute the low DIG_MASK bits of the next digit in (num - quo * den)
            mpz_dbl_dig_signed_t low_digs = (borrow & DIG_MASK) + *n - (x & DIG_MASK);
            // Store the digit result for (num).
            *n = low_digs & DIG_MASK;
            // Compute the borrow, shifted right before summing to avoid overflow.
            borrow = (borrow >> DIG_SIZE) - (x >> DIG_SIZE) + (low_digs >> DIG_SIZE);
        }

        // At this point we have either:
        //
        //   1. quo was the correct value and the most-sig-digit of num is exactly
        //      cancelled by borrow (borrow + *num_dig == 0).  In this case there is
        //      nothing more to do.
        //
        //   2. quo was too large, we subtracted too many den from num, and the
        //      most-sig-digit of num is less than needed (borrow + *num_dig < 0).
        //      In this case we must reduce quo and add back den to num until the
        //      carry from this operation cancels out the borrow.
        //
        borrow += *num_dig;
        for (; borrow != 0; --quo) {
            d = den_dig;
            d_norm = 0;
            mpz_dbl_dig_t carry = 0;
            for (mpz_dig_t *n = num_dig - den_len; n < num_dig; ++n, ++d) {
                d_norm = ((mpz_dbl_dig_t)*d << norm_shift) | (d_norm >> DIG_SIZE);
                carry += (mpz_dbl_dig_t)*n + (d_norm & DIG_MASK);
                *n = carry & DIG_MASK;
                carry >>= DIG_SIZE;
            }
            borrow += carry;
        }

        // store this digit of the quotient
        *quo_dig = quo & DIG_MASK;
        --quo_dig;

        // move down to next digit of numerator
        --num_dig;
        --(*num_len);
    }

    // unnormalise numerator (remainder now)
    for (mpz_dig_t *num = orig_num_dig + *num_len - 1, carry = 0; num >= orig_num_dig; --num) {
        mpz_dig_t n = *num;
        *num = ((n >> norm_shift) | carry) & DIG_MASK;
        carry = (mpz_dbl_dig_t)n << (DIG_SIZE - norm_shift);
    }

    // strip trailing zeros

    while (*quo_len > 0 && orig_quo_dig[*quo_len - 1] == 0) {
        --(*quo_len);
    }

    while (*num_len > 0 && orig_num_dig[*num_len - 1] == 0) {
        --(*num_len);
    }
}

#define MIN_ALLOC (2)

void mpz_init_zero(mpz_t *z) {
    z->neg = 0;
    z->fixed_dig = 0;
    z->alloc = 0;
    z->len = 0;
    z->dig = NULL;
}

void mpz_init_from_int(mpz_t *z, mp_int_t val) {
    mpz_init_zero(z);
    mpz_set_from_int(z, val);
}

void mpz_init_fixed_from_int(mpz_t *z, mpz_dig_t *dig, size_t alloc, mp_int_t val) {
    z->neg = 0;
    z->fixed_dig = 1;
    z->alloc = alloc;
    z->len = 0;
    z->dig = dig;
    mpz_set_from_int(z, val);
}

void mpz_deinit(mpz_t *z) {
    if (z != NULL && !z->fixed_dig) {
        m_del(mpz_dig_t, z->dig, z->alloc);
    }
}

#if 0
these functions are unused

mpz_t *mpz_zero(void) {
    mpz_t *z = m_new_obj(mpz_t);
    mpz_init_zero(z);
    return z;
}

mpz_t *mpz_from_int(mp_int_t val) {
    mpz_t *z = mpz_zero();
    mpz_set_from_int(z, val);
    return z;
}

mpz_t *mpz_from_ll(long long val, bool is_signed) {
    mpz_t *z = mpz_zero();
    mpz_set_from_ll(z, val, is_signed);
    return z;
}

#if MICROPY_PY_BUILTINS_FLOAT
mpz_t *mpz_from_float(mp_float_t val) {
    mpz_t *z = mpz_zero();
    mpz_set_from_float(z, val);
    return z;
}
#endif

mpz_t *mpz_from_str(const char *str, size_t len, bool neg, unsigned int base) {
    mpz_t *z = mpz_zero();
    mpz_set_from_str(z, str, len, neg, base);
    return z;
}
#endif

STATIC void mpz_free(mpz_t *z) {
    if (z != NULL) {
        m_del(mpz_dig_t, z->dig, z->alloc);
        m_del_obj(mpz_t, z);
    }
}

STATIC void mpz_need_dig(mpz_t *z, size_t need) {
    if (need < MIN_ALLOC) {
        need = MIN_ALLOC;
    }

    if (z->dig == NULL || z->alloc < need) {
        // if z has fixed digit buffer there's not much we can do as the caller will
        // be expecting a buffer with at least "need" bytes (but it shouldn't happen)
        assert(!z->fixed_dig);
        z->dig = m_renew(mpz_dig_t, z->dig, z->alloc, need);
        z->alloc = need;
    }
}

STATIC mpz_t *mpz_clone(const mpz_t *src) {
    assert(src->alloc != 0);
    mpz_t *z = m_new_obj(mpz_t);
    z->neg = src->neg;
    z->fixed_dig = 0;
    z->alloc = src->alloc;
    z->len = src->len;
    z->dig = m_new(mpz_dig_t, z->alloc);
    memcpy(z->dig, src->dig, src->alloc * sizeof(mpz_dig_t));
    return z;
}

/* sets dest = src
   can have dest, src the same
*/
void mpz_set(mpz_t *dest, const mpz_t *src) {
    mpz_need_dig(dest, src->len);
    dest->neg = src->neg;
    dest->len = src->len;
    memcpy(dest->dig, src->dig, src->len * sizeof(mpz_dig_t));
}

void mpz_set_from_int(mpz_t *z, mp_int_t val) {
    if (val == 0) {
        z->len = 0;
        return;
    }

    mpz_need_dig(z, MPZ_NUM_DIG_FOR_INT);

    mp_uint_t uval;
    if (val < 0) {
        z->neg = 1;
        uval = -val;
    } else {
        z->neg = 0;
        uval = val;
    }

    z->len = 0;
    while (uval > 0) {
        z->dig[z->len++] = uval & DIG_MASK;
        uval >>= DIG_SIZE;
    }
}

void mpz_set_from_ll(mpz_t *z, long long val, bool is_signed) {
    mpz_need_dig(z, MPZ_NUM_DIG_FOR_LL);

    unsigned long long uval;
    if (is_signed && val < 0) {
        z->neg = 1;
        uval = -val;
    } else {
        z->neg = 0;
        uval = val;
    }

    z->len = 0;
    while (uval > 0) {
        z->dig[z->len++] = uval & DIG_MASK;
        uval >>= DIG_SIZE;
    }
}

#if MICROPY_PY_BUILTINS_FLOAT
void mpz_set_from_float(mpz_t *z, mp_float_t src) {
    mp_float_union_t u = {src};
    z->neg = u.p.sgn;
    if (u.p.exp == 0) {
        // value == 0 || value < 1
        mpz_set_from_int(z, 0);
    } else if (u.p.exp == ((1 << MP_FLOAT_EXP_BITS) - 1)) {
        // u.p.frc == 0 indicates inf, else NaN
        // should be handled by caller
        mpz_set_from_int(z, 0);
    } else {
        const int adj_exp = (int)u.p.exp - MP_FLOAT_EXP_BIAS;
        if (adj_exp < 0) {
            // value < 1 , truncates to 0
            mpz_set_from_int(z, 0);
        } else if (adj_exp == 0) {
            // 1 <= value < 2 , so truncates to 1
            mpz_set_from_int(z, 1);
        } else {
            // 2 <= value
            const int dig_cnt = (adj_exp + 1 + (DIG_SIZE - 1)) / DIG_SIZE;
            const unsigned int rem = adj_exp % DIG_SIZE;
            int dig_ind, shft;
            mp_float_uint_t frc = u.p.frc | ((mp_float_uint_t)1 << MP_FLOAT_FRAC_BITS);

            if (adj_exp < MP_FLOAT_FRAC_BITS) {
                shft = 0;
                dig_ind = 0;
                frc >>= MP_FLOAT_FRAC_BITS - adj_exp;
            } else {
                shft = (rem - MP_FLOAT_FRAC_BITS) % DIG_SIZE;
                dig_ind = (adj_exp - MP_FLOAT_FRAC_BITS) / DIG_SIZE;
            }
            mpz_need_dig(z, dig_cnt);
            z->len = dig_cnt;
            if (dig_ind != 0) {
                memset(z->dig, 0, dig_ind * sizeof(mpz_dig_t));
            }
            if (shft != 0) {
                z->dig[dig_ind++] = (frc << shft) & DIG_MASK;
                frc >>= DIG_SIZE - shft;
            }
            #if DIG_SIZE < (MP_FLOAT_FRAC_BITS + 1)
            while (dig_ind != dig_cnt) {
                z->dig[dig_ind++] = frc & DIG_MASK;
                frc >>= DIG_SIZE;
            }
            #else
            if (dig_ind != dig_cnt) {
                z->dig[dig_ind] = frc;
            }
            #endif
        }
    }
}
#endif

// returns number of bytes from str that were processed
size_t mpz_set_from_str(mpz_t *z, const char *str, size_t len, bool neg, unsigned int base) {
    assert(base <= 36);

    const char *cur = str;
    const char *top = str + len;

    mpz_need_dig(z, len * 8 / DIG_SIZE + 1);

    if (neg) {
        z->neg = 1;
    } else {
        z->neg = 0;
    }

    z->len = 0;
    for (; cur < top; ++cur) { // XXX UTF8 next char
        // mp_uint_t v = char_to_numeric(cur#); // XXX UTF8 get char
        mp_uint_t v = *cur;
        if ('0' <= v && v <= '9') {
            v -= '0';
        } else if ('A' <= v && v <= 'Z') {
            v -= 'A' - 10;
        } else if ('a' <= v && v <= 'z') {
            v -= 'a' - 10;
        } else {
            break;
        }
        if (v >= base) {
            break;
        }
        z->len = mpn_mul_dig_add_dig(z->dig, z->len, base, v);
    }

    return cur - str;
}

void mpz_set_from_bytes(mpz_t *z, bool big_endian, size_t len, const byte *buf) {
    int delta = 1;
    if (big_endian) {
        buf += len - 1;
        delta = -1;
    }

    mpz_need_dig(z, (len * 8 + DIG_SIZE - 1) / DIG_SIZE);

    mpz_dig_t d = 0;
    int num_bits = 0;
    z->neg = 0;
    z->len = 0;
    while (len) {
        while (len && num_bits < DIG_SIZE) {
            d |= *buf << num_bits;
            num_bits += 8;
            buf += delta;
            len--;
        }
        z->dig[z->len++] = d & DIG_MASK;
        // Need this #if because it's C undefined behavior to do: uint32_t >> 32
        #if DIG_SIZE != 8 && DIG_SIZE != 16 && DIG_SIZE != 32
        d >>= DIG_SIZE;
        #else
        d = 0;
        #endif
        num_bits -= DIG_SIZE;
    }

    z->len = mpn_remove_trailing_zeros(z->dig, z->dig + z->len);
}

#if 0
these functions are unused

bool mpz_is_pos(const mpz_t *z) {
    return z->len > 0 && z->neg == 0;
}

bool mpz_is_odd(const mpz_t *z) {
    return z->len > 0 && (z->dig[0] & 1) != 0;
}

bool mpz_is_even(const mpz_t *z) {
    return z->len == 0 || (z->dig[0] & 1) == 0;
}
#endif

int mpz_cmp(const mpz_t *z1, const mpz_t *z2) {
    // to catch comparison of -0 with +0
    if (z1->len == 0 && z2->len == 0) {
        return 0;
    }
    int cmp = (int)z2->neg - (int)z1->neg;
    if (cmp != 0) {
        return cmp;
    }
    cmp = mpn_cmp(z1->dig, z1->len, z2->dig, z2->len);
    if (z1->neg != 0) {
        cmp = -cmp;
    }
    return cmp;
}

#if 0
// obsolete
// compares mpz with an integer that fits within DIG_SIZE bits
mp_int_t mpz_cmp_sml_int(const mpz_t *z, mp_int_t sml_int) {
    mp_int_t cmp;
    if (z->neg == 0) {
        if (sml_int < 0) {
            return 1;
        }
        if (sml_int == 0) {
            if (z->len == 0) {
                return 0;
            }
            return 1;
        }
        if (z->len == 0) {
            return -1;
        }
        assert(sml_int < (1 << DIG_SIZE));
        if (z->len != 1) {
            return 1;
        }
        cmp = z->dig[0] - sml_int;
    } else {
        if (sml_int > 0) {
            return -1;
        }
        if (sml_int == 0) {
            if (z->len == 0) {
                return 0;
            }
            return -1;
        }
        if (z->len == 0) {
            return 1;
        }
        assert(sml_int > -(1 << DIG_SIZE));
        if (z->len != 1) {
            return -1;
        }
        cmp = -z->dig[0] - sml_int;
    }
    if (cmp < 0) {
        return -1;
    }
    if (cmp > 0) {
        return 1;
    }
    return 0;
}
#endif

#if 0
these functions are unused

/* returns abs(z)
*/
mpz_t *mpz_abs(const mpz_t *z) {
    // TODO: handle case of z->alloc=0
    mpz_t *z2 = mpz_clone(z);
    z2->neg = 0;
    return z2;
}

/* returns -z
*/
mpz_t *mpz_neg(const mpz_t *z) {
    // TODO: handle case of z->alloc=0
    mpz_t *z2 = mpz_clone(z);
    z2->neg = 1 - z2->neg;
    return z2;
}

/* returns lhs + rhs
   can have lhs, rhs the same
*/
mpz_t *mpz_add(const mpz_t *lhs, const mpz_t *rhs) {
    mpz_t *z = mpz_zero();
    mpz_add_inpl(z, lhs, rhs);
    return z;
}

/* returns lhs - rhs
   can have lhs, rhs the same
*/
mpz_t *mpz_sub(const mpz_t *lhs, const mpz_t *rhs) {
    mpz_t *z = mpz_zero();
    mpz_sub_inpl(z, lhs, rhs);
    return z;
}

/* returns lhs * rhs
   can have lhs, rhs the same
*/
mpz_t *mpz_mul(const mpz_t *lhs, const mpz_t *rhs) {
    mpz_t *z = mpz_zero();
    mpz_mul_inpl(z, lhs, rhs);
    return z;
}

/* returns lhs ** rhs
   can have lhs, rhs the same
*/
mpz_t *mpz_pow(const mpz_t *lhs, const mpz_t *rhs) {
    mpz_t *z = mpz_zero();
    mpz_pow_inpl(z, lhs, rhs);
    return z;
}

/* computes new integers in quo and rem such that:
       quo * rhs + rem = lhs
       0 <= rem < rhs
   can have lhs, rhs the same
*/
void mpz_divmod(const mpz_t *lhs, const mpz_t *rhs, mpz_t **quo, mpz_t **rem) {
    *quo = mpz_zero();
    *rem = mpz_zero();
    mpz_divmod_inpl(*quo, *rem, lhs, rhs);
}
#endif

/* computes dest = abs(z)
   can have dest, z the same
*/
void mpz_abs_inpl(mpz_t *dest, const mpz_t *z) {
    if (dest != z) {
        mpz_set(dest, z);
    }
    dest->neg = 0;
}

/* computes dest = -z
   can have dest, z the same
*/
void mpz_neg_inpl(mpz_t *dest, const mpz_t *z) {
    if (dest != z) {
        mpz_set(dest, z);
    }
    dest->neg = 1 - dest->neg;
}

/* computes dest = ~z (= -z - 1)
   can have dest, z the same
*/
void mpz_not_inpl(mpz_t *dest, const mpz_t *z) {
    if (dest != z) {
        mpz_set(dest, z);
    }
    if (dest->len == 0) {
        mpz_need_dig(dest, 1);
        dest->dig[0] = 1;
        dest->len = 1;
        dest->neg = 1;
    } else if (dest->neg) {
        dest->neg = 0;
        mpz_dig_t k = 1;
        dest->len = mpn_sub(dest->dig, dest->dig, dest->len, &k, 1);
    } else {
        mpz_need_dig(dest, dest->len + 1);
        mpz_dig_t k = 1;
        dest->len = mpn_add(dest->dig, dest->dig, dest->len, &k, 1);
        dest->neg = 1;
    }
}

/* computes dest = lhs << rhs
   can have dest, lhs the same
*/
void mpz_shl_inpl(mpz_t *dest, const mpz_t *lhs, mp_uint_t rhs) {
    if (lhs->len == 0 || rhs == 0) {
        mpz_set(dest, lhs);
    } else {
        mpz_need_dig(dest, lhs->len + (rhs + DIG_SIZE - 1) / DIG_SIZE);
        dest->len = mpn_shl(dest->dig, lhs->dig, lhs->len, rhs);
        dest->neg = lhs->neg;
    }
}

/* computes dest = lhs >> rhs
   can have dest, lhs the same
*/
void mpz_shr_inpl(mpz_t *dest, const mpz_t *lhs, mp_uint_t rhs) {
    if (lhs->len == 0 || rhs == 0) {
        mpz_set(dest, lhs);
    } else {
        mpz_need_dig(dest, lhs->len);
        dest->len = mpn_shr(dest->dig, lhs->dig, lhs->len, rhs);
        dest->neg = lhs->neg;
        if (dest->neg) {
            // arithmetic shift right, rounding to negative infinity
            mp_uint_t n_whole = rhs / DIG_SIZE;
            mp_uint_t n_part = rhs % DIG_SIZE;
            mpz_dig_t round_up = 0;
            for (size_t i = 0; i < lhs->len && i < n_whole; i++) {
                if (lhs->dig[i] != 0) {
                    round_up = 1;
                    break;
                }
            }
            if (n_whole < lhs->len && (lhs->dig[n_whole] & ((1 << n_part) - 1)) != 0) {
                round_up = 1;
            }
            if (round_up) {
                if (dest->len == 0) {
                    // dest == 0, so need to add 1 by hand (answer will be -1)
                    dest->dig[0] = 1;
                    dest->len = 1;
                } else {
                    // dest > 0, so can use mpn_add to add 1
                    dest->len = mpn_add(dest->dig, dest->dig, dest->len, &round_up, 1);
                }
            }
        }
    }
}

/* computes dest = lhs + rhs
   can have dest, lhs, rhs the same
*/
void mpz_add_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs) {
    if (mpn_cmp(lhs->dig, lhs->len, rhs->dig, rhs->len) < 0) {
        const mpz_t *temp = lhs;
        lhs = rhs;
        rhs = temp;
    }

    if (lhs->neg == rhs->neg) {
        mpz_need_dig(dest, lhs->len + 1);
        dest->len = mpn_add(dest->dig, lhs->dig, lhs->len, rhs->dig, rhs->len);
    } else {
        mpz_need_dig(dest, lhs->len);
        dest->len = mpn_sub(dest->dig, lhs->dig, lhs->len, rhs->dig, rhs->len);
    }

    dest->neg = lhs->neg;
}

/* computes dest = lhs - rhs
   can have dest, lhs, rhs the same
*/
void mpz_sub_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs) {
    bool neg = false;

    if (mpn_cmp(lhs->dig, lhs->len, rhs->dig, rhs->len) < 0) {
        const mpz_t *temp = lhs;
        lhs = rhs;
        rhs = temp;
        neg = true;
    }

    if (lhs->neg != rhs->neg) {
        mpz_need_dig(dest, lhs->len + 1);
        dest->len = mpn_add(dest->dig, lhs->dig, lhs->len, rhs->dig, rhs->len);
    } else {
        mpz_need_dig(dest, lhs->len);
        dest->len = mpn_sub(dest->dig, lhs->dig, lhs->len, rhs->dig, rhs->len);
    }

    if (neg) {
        dest->neg = 1 - lhs->neg;
    } else {
        dest->neg = lhs->neg;
    }
}

/* computes dest = lhs & rhs
   can have dest, lhs, rhs the same
*/
void mpz_and_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs) {
    // make sure lhs has the most digits
    if (lhs->len < rhs->len) {
        const mpz_t *temp = lhs;
        lhs = rhs;
        rhs = temp;
    }

    #if MICROPY_OPT_MPZ_BITWISE

    if ((0 == lhs->neg) && (0 == rhs->neg)) {
        mpz_need_dig(dest, lhs->len);
        dest->len = mpn_and(dest->dig, lhs->dig, rhs->dig, rhs->len);
        dest->neg = 0;
    } else {
        mpz_need_dig(dest, lhs->len + 1);
        dest->len = mpn_and_neg(dest->dig, lhs->dig, lhs->len, rhs->dig, rhs->len,
            lhs->neg == rhs->neg, 0 != lhs->neg, 0 != rhs->neg);
        dest->neg = lhs->neg & rhs->neg;
    }

    #else

    mpz_need_dig(dest, lhs->len + (lhs->neg || rhs->neg));
    dest->len = mpn_and_neg(dest->dig, lhs->dig, lhs->len, rhs->dig, rhs->len,
        (lhs->neg == rhs->neg) ? lhs->neg : 0, lhs->neg, rhs->neg);
    dest->neg = lhs->neg & rhs->neg;

    #endif
}

/* computes dest = lhs | rhs
   can have dest, lhs, rhs the same
*/
void mpz_or_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs) {
    // make sure lhs has the most digits
    if (lhs->len < rhs->len) {
        const mpz_t *temp = lhs;
        lhs = rhs;
        rhs = temp;
    }

    #if MICROPY_OPT_MPZ_BITWISE

    if ((0 == lhs->neg) && (0 == rhs->neg)) {
        mpz_need_dig(dest, lhs->len);
        dest->len = mpn_or(dest->dig, lhs->dig, lhs->len, rhs->dig, rhs->len);
        dest->neg = 0;
    } else {
        mpz_need_dig(dest, lhs->len + 1);
        dest->len = mpn_or_neg(dest->dig, lhs->dig, lhs->len, rhs->dig, rhs->len,
            0 != lhs->neg, 0 != rhs->neg);
        dest->neg = 1;
    }

    #else

    mpz_need_dig(dest, lhs->len + (lhs->neg || rhs->neg));
    dest->len = mpn_or_neg(dest->dig, lhs->dig, lhs->len, rhs->dig, rhs->len,
        (lhs->neg || rhs->neg), lhs->neg, rhs->neg);
    dest->neg = lhs->neg | rhs->neg;

    #endif
}

/* computes dest = lhs ^ rhs
   can have dest, lhs, rhs the same
*/
void mpz_xor_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs) {
    // make sure lhs has the most digits
    if (lhs->len < rhs->len) {
        const mpz_t *temp = lhs;
        lhs = rhs;
        rhs = temp;
    }

    #if MICROPY_OPT_MPZ_BITWISE

    if (lhs->neg == rhs->neg) {
        mpz_need_dig(dest, lhs->len);
        if (lhs->neg == 0) {
            dest->len = mpn_xor(dest->dig, lhs->dig, lhs->len, rhs->dig, rhs->len);
        } else {
            dest->len = mpn_xor_neg(dest->dig, lhs->dig, lhs->len, rhs->dig, rhs->len, 0, 0, 0);
        }
        dest->neg = 0;
    } else {
        mpz_need_dig(dest, lhs->len + 1);
        dest->len = mpn_xor_neg(dest->dig, lhs->dig, lhs->len, rhs->dig, rhs->len, 1,
            0 == lhs->neg, 0 == rhs->neg);
        dest->neg = 1;
    }

    #else

    mpz_need_dig(dest, lhs->len + (lhs->neg || rhs->neg));
    dest->len = mpn_xor_neg(dest->dig, lhs->dig, lhs->len, rhs->dig, rhs->len,
        (lhs->neg != rhs->neg), 0 == lhs->neg, 0 == rhs->neg);
    dest->neg = lhs->neg ^ rhs->neg;

    #endif
}

/* computes dest = lhs * rhs
   can have dest, lhs, rhs the same
*/
void mpz_mul_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs) {
    if (lhs->len == 0 || rhs->len == 0) {
        mpz_set_from_int(dest, 0);
        return;
    }

    mpz_t *temp = NULL;
    if (lhs == dest) {
        lhs = temp = mpz_clone(lhs);
        if (rhs == dest) {
            rhs = lhs;
        }
    } else if (rhs == dest) {
        rhs = temp = mpz_clone(rhs);
    }

    mpz_need_dig(dest, lhs->len + rhs->len); // min mem l+r-1, max mem l+r
    memset(dest->dig, 0, dest->alloc * sizeof(mpz_dig_t));
    dest->len = mpn_mul(dest->dig, lhs->dig, lhs->len, rhs->dig, rhs->len);

    if (lhs->neg == rhs->neg) {
        dest->neg = 0;
    } else {
        dest->neg = 1;
    }

    mpz_free(temp);
}

/* computes dest = lhs ** rhs
   can have dest, lhs, rhs the same
*/
void mpz_pow_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs) {
    if (lhs->len == 0 || rhs->neg != 0) {
        mpz_set_from_int(dest, 0);
        return;
    }

    if (rhs->len == 0) {
        mpz_set_from_int(dest, 1);
        return;
    }

    mpz_t *x = mpz_clone(lhs);
    mpz_t *n = mpz_clone(rhs);

    mpz_set_from_int(dest, 1);

    while (n->len > 0) {
        if ((n->dig[0] & 1) != 0) {
            mpz_mul_inpl(dest, dest, x);
        }
        n->len = mpn_shr(n->dig, n->dig, n->len, 1);
        if (n->len == 0) {
            break;
        }
        mpz_mul_inpl(x, x, x);
    }

    mpz_free(x);
    mpz_free(n);
}

/* computes dest = (lhs ** rhs) % mod
   can have dest, lhs, rhs the same; mod can't be the same as dest
*/
void mpz_pow3_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs, const mpz_t *mod) {
    if (lhs->len == 0 || rhs->neg != 0 || (mod->len == 1 && mod->dig[0] == 1)) {
        mpz_set_from_int(dest, 0);
        return;
    }

    mpz_set_from_int(dest, 1);

    if (rhs->len == 0) {
        return;
    }

    mpz_t *x = mpz_clone(lhs);
    mpz_t *n = mpz_clone(rhs);
    mpz_t quo;
    mpz_init_zero(&quo);

    while (n->len > 0) {
        if ((n->dig[0] & 1) != 0) {
            mpz_mul_inpl(dest, dest, x);
            mpz_divmod_inpl(&quo, dest, dest, mod);
        }
        n->len = mpn_shr(n->dig, n->dig, n->len, 1);
        if (n->len == 0) {
            break;
        }
        mpz_mul_inpl(x, x, x);
        mpz_divmod_inpl(&quo, x, x, mod);
    }

    mpz_deinit(&quo);
    mpz_free(x);
    mpz_free(n);
}

#if 0
these functions are unused

/* computes gcd(z1, z2)
   based on Knuth's modified gcd algorithm (I think?)
   gcd(z1, z2) >= 0
   gcd(0, 0) = 0
   gcd(z, 0) = abs(z)
*/
mpz_t *mpz_gcd(const mpz_t *z1, const mpz_t *z2) {
    if (z1->len == 0) {
        // TODO: handle case of z2->alloc=0
        mpz_t *a = mpz_clone(z2);
        a->neg = 0;
        return a;
    } else if (z2->len == 0) {
        mpz_t *a = mpz_clone(z1);
        a->neg = 0;
        return a;
    }

    mpz_t *a = mpz_clone(z1);
    mpz_t *b = mpz_clone(z2);
    mpz_t c;
    mpz_init_zero(&c);
    a->neg = 0;
    b->neg = 0;

    for (;;) {
        if (mpz_cmp(a, b) < 0) {
            if (a->len == 0) {
                mpz_free(a);
                mpz_deinit(&c);
                return b;
            }
            mpz_t *t = a;
            a = b;
            b = t;
        }
        if (!(b->len >= 2 || (b->len == 1 && b->dig[0] > 1))) { // compute b > 0; could be mpz_cmp_small_int(b, 1) > 0
            break;
        }
        mpz_set(&c, b);
        do {
            mpz_add_inpl(&c, &c, &c);
        } while (mpz_cmp(&c, a) <= 0);
        c.len = mpn_shr(c.dig, c.dig, c.len, 1);
        mpz_sub_inpl(a, a, &c);
    }

    mpz_deinit(&c);

    if (b->len == 1 && b->dig[0] == 1) { // compute b == 1; could be mpz_cmp_small_int(b, 1) == 0
        mpz_free(a);
        return b;
    } else {
        mpz_free(b);
        return a;
    }
}

/* computes lcm(z1, z2)
     = abs(z1) / gcd(z1, z2) * abs(z2)
  lcm(z1, z1) >= 0
  lcm(0, 0) = 0
  lcm(z, 0) = 0
*/
mpz_t *mpz_lcm(const mpz_t *z1, const mpz_t *z2) {
    if (z1->len == 0 || z2->len == 0) {
        return mpz_zero();
    }

    mpz_t *gcd = mpz_gcd(z1, z2);
    mpz_t *quo = mpz_zero();
    mpz_t *rem = mpz_zero();
    mpz_divmod_inpl(quo, rem, z1, gcd);
    mpz_mul_inpl(rem, quo, z2);
    mpz_free(gcd);
    mpz_free(quo);
    rem->neg = 0;
    return rem;
}
#endif

/* computes new integers in quo and rem such that:
       quo * rhs + rem = lhs
       0 <= rem < rhs
   can have lhs, rhs the same
   assumes rhs != 0 (undefined behaviour if it is)
*/
void mpz_divmod_inpl(mpz_t *dest_quo, mpz_t *dest_rem, const mpz_t *lhs, const mpz_t *rhs) {
    assert(!mpz_is_zero(rhs));

    mpz_need_dig(dest_quo, lhs->len + 1); // +1 necessary?
    memset(dest_quo->dig, 0, (lhs->len + 1) * sizeof(mpz_dig_t));
    dest_quo->len = 0;
    mpz_need_dig(dest_rem, lhs->len + 1); // +1 necessary?
    mpz_set(dest_rem, lhs);
    mpn_div(dest_rem->dig, &dest_rem->len, rhs->dig, rhs->len, dest_quo->dig, &dest_quo->len);

    // check signs and do Python style modulo
    if (lhs->neg != rhs->neg) {
        dest_quo->neg = 1;
        if (!mpz_is_zero(dest_rem)) {
            mpz_t mpzone;
            mpz_init_from_int(&mpzone, -1);
            mpz_add_inpl(dest_quo, dest_quo, &mpzone);
            mpz_add_inpl(dest_rem, dest_rem, rhs);
        }
    }
}

#if 0
these functions are unused

/* computes floor(lhs / rhs)
   can have lhs, rhs the same
*/
mpz_t *mpz_div(const mpz_t *lhs, const mpz_t *rhs) {
    mpz_t *quo = mpz_zero();
    mpz_t rem;
    mpz_init_zero(&rem);
    mpz_divmod_inpl(quo, &rem, lhs, rhs);
    mpz_deinit(&rem);
    return quo;
}

/* computes lhs % rhs ( >= 0)
   can have lhs, rhs the same
*/
mpz_t *mpz_mod(const mpz_t *lhs, const mpz_t *rhs) {
    mpz_t quo;
    mpz_init_zero(&quo);
    mpz_t *rem = mpz_zero();
    mpz_divmod_inpl(&quo, rem, lhs, rhs);
    mpz_deinit(&quo);
    return rem;
}
#endif

// must return actual int value if it fits in mp_int_t
mp_int_t mpz_hash(const mpz_t *z) {
    mp_uint_t val = 0;
    mpz_dig_t *d = z->dig + z->len;

    while (d-- > z->dig) {
        val = (val << DIG_SIZE) | *d;
    }

    if (z->neg != 0) {
        val = -val;
    }

    return val;
}

bool mpz_as_int_checked(const mpz_t *i, mp_int_t *value) {
    mp_uint_t val = 0;
    mpz_dig_t *d = i->dig + i->len;

    while (d-- > i->dig) {
        if (val > (~(MP_OBJ_WORD_MSBIT_HIGH) >> DIG_SIZE)) {
            // will overflow
            return false;
        }
        val = (val << DIG_SIZE) | *d;
    }

    if (i->neg != 0) {
        val = -val;
    }

    *value = val;
    return true;
}

bool mpz_as_uint_checked(const mpz_t *i, mp_uint_t *value) {
    if (i->neg != 0) {
        // can't represent signed values
        return false;
    }

    mp_uint_t val = 0;
    mpz_dig_t *d = i->dig + i->len;

    while (d-- > i->dig) {
        if (val > (~(MP_OBJ_WORD_MSBIT_HIGH) >> (DIG_SIZE - 1))) {
            // will overflow
            return false;
        }
        val = (val << DIG_SIZE) | *d;
    }

    *value = val;
    return true;
}

void mpz_as_bytes(const mpz_t *z, bool big_endian, size_t len, byte *buf) {
    byte *b = buf;
    if (big_endian) {
        b += len;
    }
    mpz_dig_t *zdig = z->dig;
    int bits = 0;
    mpz_dbl_dig_t d = 0;
    mpz_dbl_dig_t carry = 1;
    for (size_t zlen = z->len; zlen > 0; --zlen) {
        bits += DIG_SIZE;
        d = (d << DIG_SIZE) | *zdig++;
        for (; bits >= 8; bits -= 8, d >>= 8) {
            mpz_dig_t val = d;
            if (z->neg) {
                val = (~val & 0xff) + carry;
                carry = val >> 8;
            }
            if (big_endian) {
                *--b = val;
                if (b == buf) {
                    return;
                }
            } else {
                *b++ = val;
                if (b == buf + len) {
                    return;
                }
            }
        }
    }

    // fill remainder of buf with zero/sign extension of the integer
    if (big_endian) {
        len = b - buf;
    } else {
        len = buf + len - b;
        buf = b;
    }
    memset(buf, z->neg ? 0xff : 0x00, len);
}

#if MICROPY_PY_BUILTINS_FLOAT
mp_float_t mpz_as_float(const mpz_t *i) {
    mp_float_t val = 0;
    mpz_dig_t *d = i->dig + i->len;

    while (d-- > i->dig) {
        val = val * DIG_BASE + *d;
    }

    if (i->neg != 0) {
        val = -val;
    }

    return val;
}
#endif

#if 0
this function is unused
char *mpz_as_str(const mpz_t *i, unsigned int base) {
    char *s = m_new(char, mp_int_format_size(mpz_max_num_bits(i), base, NULL, '\0'));
    mpz_as_str_inpl(i, base, NULL, 'a', '\0', s);
    return s;
}
#endif

// assumes enough space in str as calculated by mp_int_format_size
// base must be between 2 and 32 inclusive
// returns length of string, not including null byte
size_t mpz_as_str_inpl(const mpz_t *i, unsigned int base, const char *prefix, char base_char, char comma, char *str) {
    assert(str != NULL);
    assert(2 <= base && base <= 32);

    size_t ilen = i->len;

    char *s = str;
    if (ilen == 0) {
        if (prefix) {
            while (*prefix) {
                *s++ = *prefix++;
            }
        }
        *s++ = '0';
        *s = '\0';
        return s - str;
    }

    // make a copy of mpz digits, so we can do the div/mod calculation
    mpz_dig_t *dig = m_new(mpz_dig_t, ilen);
    memcpy(dig, i->dig, ilen * sizeof(mpz_dig_t));

    // convert
    char *last_comma = str;
    bool done;
    do {
        mpz_dig_t *d = dig + ilen;
        mpz_dbl_dig_t a = 0;

        // compute next remainder
        while (--d >= dig) {
            a = (a << DIG_SIZE) | *d;
            *d = a / base;
            a %= base;
        }

        // convert to character
        a += '0';
        if (a > '9') {
            a += base_char - '9' - 1;
        }
        *s++ = a;

        // check if number is zero
        done = true;
        for (d = dig; d < dig + ilen; ++d) {
            if (*d != 0) {
                done = false;
                break;
            }
        }
        if (comma && (s - last_comma) == 3) {
            *s++ = comma;
            last_comma = s;
        }
    }
    while (!done);

    // free the copy of the digits array
    m_del(mpz_dig_t, dig, ilen);

    if (prefix) {
        const char *p = &prefix[strlen(prefix)];
        while (p > prefix) {
            *s++ = *--p;
        }
    }
    if (i->neg != 0) {
        *s++ = '-';
    }

    // reverse string
    for (char *u = str, *v = s - 1; u < v; ++u, --v) {
        char temp = *u;
        *u = *v;
        *v = temp;
    }

    *s = '\0'; // null termination

    return s - str;
}

#endif // MICROPY_LONGINT_IMPL == MICROPY_LONGINT_IMPL_MPZ
