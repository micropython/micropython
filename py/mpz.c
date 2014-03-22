#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "misc.h"
#include "mpconfig.h"
#include "mpz.h"

#if MICROPY_LONGINT_IMPL == MICROPY_LONGINT_IMPL_MPZ

#define DIG_SIZE (MPZ_DIG_SIZE)
#define DIG_MASK ((1 << DIG_SIZE) - 1)

/*
 mpz is an arbitrary precision integer type with a public API.

 mpn functions act on non-negative integers represented by an array of generalised
 digits (eg a word per digit).  You also need to specify separately the length of the
 array.  There is no public API for mpn.  Rather, the functions are used by mpz to
 implement its features.

 Integer values are stored little endian (first digit is first in memory).

 Definition of normalise: ?
*/

/* compares i with j
   returns sign(i - j)
   assumes i, j are normalised
*/
STATIC int mpn_cmp(const mpz_dig_t *idig, uint ilen, const mpz_dig_t *jdig, uint jlen) {
    if (ilen < jlen) { return -1; }
    if (ilen > jlen) { return 1; }

    for (idig += ilen, jdig += ilen; ilen > 0; --ilen) {
        int cmp = *(--idig) - *(--jdig);
        if (cmp < 0) { return -1; }
        if (cmp > 0) { return 1; }
    }

    return 0;
}

/* computes i = j << n
   returns number of digits in i
   assumes enough memory in i; assumes normalised j; assumes n > 0
   can have i, j pointing to same memory
*/
STATIC uint mpn_shl(mpz_dig_t *idig, mpz_dig_t *jdig, uint jlen, uint n) {
    uint n_whole = (n + DIG_SIZE - 1) / DIG_SIZE;
    uint n_part = n % DIG_SIZE;
    if (n_part == 0) {
        n_part = DIG_SIZE;
    }

    // start from the high end of the digit arrays
    idig += jlen + n_whole - 1;
    jdig += jlen - 1;

    // shift the digits
    mpz_dbl_dig_t d = 0;
    for (uint i = jlen; i > 0; i--, idig--, jdig--) {
        d |= *jdig;
        *idig = d >> (DIG_SIZE - n_part);
        d <<= DIG_SIZE;
    }

    // store remaining bits
    *idig = d >> (DIG_SIZE - n_part);
    idig -= n_whole - 1;
    memset(idig, 0, (n_whole - 1) * sizeof(mpz_dig_t));

    // work out length of result
    jlen += n_whole;
    if (idig[jlen - 1] == 0) {
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
STATIC uint mpn_shr(mpz_dig_t *idig, mpz_dig_t *jdig, uint jlen, uint n) {
    uint n_whole = n / DIG_SIZE;
    uint n_part = n % DIG_SIZE;

    if (n_whole >= jlen) {
        return 0;
    }

    jdig += n_whole;
    jlen -= n_whole;

    for (uint i = jlen; i > 0; i--, idig++, jdig++) {
        mpz_dbl_dig_t d = *jdig;
        if (i > 1) {
            d |= jdig[1] << DIG_SIZE;
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
STATIC uint mpn_add(mpz_dig_t *idig, const mpz_dig_t *jdig, uint jlen, const mpz_dig_t *kdig, uint klen) {
    mpz_dig_t *oidig = idig;
    mpz_dbl_dig_t carry = 0;

    jlen -= klen;

    for (; klen > 0; --klen, ++idig, ++jdig, ++kdig) {
        carry += *jdig + *kdig;
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
STATIC uint mpn_sub(mpz_dig_t *idig, const mpz_dig_t *jdig, uint jlen, const mpz_dig_t *kdig, uint klen) {
    mpz_dig_t *oidig = idig;
    mpz_dbl_dig_signed_t borrow = 0;

    jlen -= klen;

    for (; klen > 0; --klen, ++idig, ++jdig, ++kdig) {
        borrow += *jdig - *kdig;
        *idig = borrow & DIG_MASK;
        borrow >>= DIG_SIZE;
    }

    for (; jlen > 0; --jlen, ++idig, ++jdig) {
        borrow += *jdig;
        *idig = borrow & DIG_MASK;
        borrow >>= DIG_SIZE;
    }

    for (--idig; idig >= oidig && *idig == 0; --idig) {
    }

    return idig + 1 - oidig;
}

/* computes i = j & k
   returns number of digits in i
   assumes enough memory in i; assumes normalised j, k; assumes jlen >= klen
   can have i, j, k pointing to same memory
*/
STATIC uint mpn_and(mpz_dig_t *idig, const mpz_dig_t *jdig, uint jlen, const mpz_dig_t *kdig, uint klen) {
    mpz_dig_t *oidig = idig;

    jlen -= klen;

    for (; klen > 0; --klen, ++idig, ++jdig, ++kdig) {
        *idig = *jdig & *kdig;
    }

    for (; jlen > 0; --jlen, ++idig) {
        *idig = 0;
    }

    return idig - oidig;
}

/* computes i = j | k
   returns number of digits in i
   assumes enough memory in i; assumes normalised j, k; assumes jlen >= klen
   can have i, j, k pointing to same memory
*/
STATIC uint mpn_or(mpz_dig_t *idig, const mpz_dig_t *jdig, uint jlen, const mpz_dig_t *kdig, uint klen) {
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

/* computes i = j ^ k
   returns number of digits in i
   assumes enough memory in i; assumes normalised j, k; assumes jlen >= klen
   can have i, j, k pointing to same memory
*/
STATIC uint mpn_xor(mpz_dig_t *idig, const mpz_dig_t *jdig, uint jlen, const mpz_dig_t *kdig, uint klen) {
    mpz_dig_t *oidig = idig;

    jlen -= klen;

    for (; klen > 0; --klen, ++idig, ++jdig, ++kdig) {
        *idig = *jdig ^ *kdig;
    }

    for (; jlen > 0; --jlen, ++idig, ++jdig) {
        *idig = *jdig;
    }

    return idig - oidig;
}

/* computes i = i * d1 + d2
   returns number of digits in i
   assumes enough memory in i; assumes normalised i; assumes dmul != 0
*/
STATIC uint mpn_mul_dig_add_dig(mpz_dig_t *idig, uint ilen, mpz_dig_t dmul, mpz_dig_t dadd) {
    mpz_dig_t *oidig = idig;
    mpz_dbl_dig_t carry = dadd;

    for (; ilen > 0; --ilen, ++idig) {
        carry += *idig * dmul; // will never overflow so long as DIG_SIZE <= WORD_SIZE / 2
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
STATIC uint mpn_mul(mpz_dig_t *idig, mpz_dig_t *jdig, uint jlen, mpz_dig_t *kdig, uint klen) {
    mpz_dig_t *oidig = idig;
    uint ilen = 0;

    for (; klen > 0; --klen, ++idig, ++kdig) {
        mpz_dig_t *id = idig;
        mpz_dbl_dig_t carry = 0;

        uint jl = jlen;
        for (mpz_dig_t *jd = jdig; jl > 0; --jl, ++jd, ++id) {
            carry += *id + *jd * *kdig; // will never overflow so long as DIG_SIZE <= WORD_SIZE / 2
            *id = carry & DIG_MASK;
            carry >>= DIG_SIZE;
        }

        if (carry != 0) {
            *id++ = carry;
        }

        ilen = id - oidig;
    }

    return ilen;
}

/* natural_div - quo * den + new_num = old_num (ie num is replaced with rem)
   assumes den != 0
   assumes num_dig has enough memory to be extended by 1 digit
   assumes quo_dig has enough memory (as many digits as num)
   assumes quo_dig is filled with zeros
   modifies den_dig memory, but restors it to original state at end
*/

STATIC void mpn_div(mpz_dig_t *num_dig, machine_uint_t *num_len, mpz_dig_t *den_dig, machine_uint_t den_len, mpz_dig_t *quo_dig, machine_uint_t *quo_len) {
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

    // count number of leading zeros in leading digit of denominator
    {
        mpz_dig_t d = den_dig[den_len - 1];
        while ((d & (1 << (DIG_SIZE - 1))) == 0) {
            d <<= 1;
            ++norm_shift;
        }
    }

    // normalise denomenator (leading bit of leading digit is 1)
    for (mpz_dig_t *den = den_dig, carry = 0; den < den_dig + den_len; ++den) {
        mpz_dig_t d = *den;
        *den = ((d << norm_shift) | carry) & DIG_MASK;
        carry = d >> (DIG_SIZE - norm_shift);
    }

    // now need to shift numerator by same amount as denominator
    // first, increase length of numerator in case we need more room to shift
    num_dig[*num_len] = 0;
    ++(*num_len);
    for (mpz_dig_t *num = num_dig, carry = 0; num < num_dig + *num_len; ++num) {
        mpz_dig_t n = *num;
        *num = ((n << norm_shift) | carry) & DIG_MASK;
        carry = n >> (DIG_SIZE - norm_shift);
    }

    // cache the leading digit of the denominator
    lead_den_digit = den_dig[den_len - 1];

    // point num_dig to last digit in numerator
    num_dig += *num_len - 1;

    // calculate number of digits in quotient
    *quo_len = *num_len - den_len;

    // point to last digit to store for quotient
    quo_dig += *quo_len - 1;

    // keep going while we have enough digits to divide
    while (*num_len > den_len) {
        mpz_dbl_dig_t quo = (*num_dig << DIG_SIZE) | num_dig[-1];

        // get approximate quotient
        quo /= lead_den_digit;

        // multiply quo by den and subtract from num get remainder
        {
            mpz_dbl_dig_signed_t borrow = 0;

            for (mpz_dig_t *n = num_dig - den_len, *d = den_dig; n < num_dig; ++n, ++d) {
                borrow += *n - quo * *d; // will overflow if DIG_SIZE >= 16
                *n = borrow & DIG_MASK;
                borrow >>= DIG_SIZE;
            }
            borrow += *num_dig; // will overflow if DIG_SIZE >= 16
            *num_dig = borrow & DIG_MASK;
            borrow >>= DIG_SIZE;

            // adjust quotient if it is too big
            for (; borrow != 0; --quo) {
                mpz_dbl_dig_t carry = 0;
                for (mpz_dig_t *n = num_dig - den_len, *d = den_dig; n < num_dig; ++n, ++d) {
                    carry += *n + *d;
                    *n = carry & DIG_MASK;
                    carry >>= DIG_SIZE;
                }
                carry += *num_dig;
                *num_dig = carry & DIG_MASK;
                carry >>= DIG_SIZE;

                borrow += carry;
            }
        }

        // store this digit of the quotient
        *quo_dig = quo & DIG_MASK;
        --quo_dig;

        // move down to next digit of numerator
        --num_dig;
        --(*num_len);
    }

    // unnormalise denomenator
    for (mpz_dig_t *den = den_dig + den_len - 1, carry = 0; den >= den_dig; --den) {
        mpz_dig_t d = *den;
        *den = ((d >> norm_shift) | carry) & DIG_MASK;
        carry = d << (DIG_SIZE - norm_shift);
    }

    // unnormalise numerator (remainder now)
    for (mpz_dig_t *num = orig_num_dig + *num_len - 1, carry = 0; num >= orig_num_dig; --num) {
        mpz_dig_t n = *num;
        *num = ((n >> norm_shift) | carry) & DIG_MASK;
        carry = n << (DIG_SIZE - norm_shift);
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

static const uint log_base2_floor[] = {
    0,
    0, 1, 1, 2,
    2, 2, 2, 3,
    3, 3, 3, 3,
    3, 3, 3, 4,
    4, 4, 4, 4,
    4, 4, 4, 4,
    4, 4, 4, 4,
    4, 4, 4, 5
};

void mpz_init_zero(mpz_t *z) {
    z->neg = 0;
    z->fixed_dig = 0;
    z->alloc = 0;
    z->len = 0;
    z->dig = NULL;
}

void mpz_init_from_int(mpz_t *z, machine_int_t val) {
    mpz_init_zero(z);
    mpz_set_from_int(z, val);
}

void mpz_init_fixed_from_int(mpz_t *z, mpz_dig_t *dig, uint alloc, machine_int_t val) {
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

mpz_t *mpz_zero(void) {
    mpz_t *z = m_new_obj(mpz_t);
    mpz_init_zero(z);
    return z;
}

mpz_t *mpz_from_int(machine_int_t val) {
    mpz_t *z = mpz_zero();
    mpz_set_from_int(z, val);
    return z;
}

mpz_t *mpz_from_ll(long long val) {
    mpz_t *z = mpz_zero();
    mpz_set_from_ll(z, val);
    return z;
}

mpz_t *mpz_from_str(const char *str, uint len, bool neg, uint base) {
    mpz_t *z = mpz_zero();
    mpz_set_from_str(z, str, len, neg, base);
    return z;
}

void mpz_free(mpz_t *z) {
    if (z != NULL) {
        m_del(mpz_dig_t, z->dig, z->alloc);
        m_del_obj(mpz_t, z);
    }
}

STATIC void mpz_need_dig(mpz_t *z, uint need) {
    if (need < MIN_ALLOC) {
        need = MIN_ALLOC;
    }

    if (z->dig == NULL || z->alloc < need) {
        if (z->fixed_dig) {
            // cannot reallocate fixed buffers
            assert(0);
            return;
        }
        z->dig = m_renew(mpz_dig_t, z->dig, z->alloc, need);
        z->alloc = need;
    }
}

mpz_t *mpz_clone(const mpz_t *src) {
    mpz_t *z = m_new_obj(mpz_t);
    z->neg = src->neg;
    z->fixed_dig = 0;
    z->alloc = src->alloc;
    z->len = src->len;
    if (src->dig == NULL) {
        z->dig = NULL;
    } else {
        z->dig = m_new(mpz_dig_t, z->alloc);
        memcpy(z->dig, src->dig, src->alloc * sizeof(mpz_dig_t));
    }
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

void mpz_set_from_int(mpz_t *z, machine_int_t val) {
    mpz_need_dig(z, MPZ_NUM_DIG_FOR_INT);

    machine_uint_t uval;
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

void mpz_set_from_ll(mpz_t *z, long long val) {
    mpz_need_dig(z, MPZ_NUM_DIG_FOR_LL);

    unsigned long long uval;
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

// returns number of bytes from str that were processed
uint mpz_set_from_str(mpz_t *z, const char *str, uint len, bool neg, uint base) {
    assert(base < 36);

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
        //uint v = char_to_numeric(cur#); // XXX UTF8 get char
        uint v = *cur;
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

bool mpz_is_zero(const mpz_t *z) {
    return z->len == 0;
}

bool mpz_is_pos(const mpz_t *z) {
    return z->len > 0 && z->neg == 0;
}

bool mpz_is_neg(const mpz_t *z) {
    return z->len > 0 && z->neg != 0;
}

bool mpz_is_odd(const mpz_t *z) {
    return z->len > 0 && (z->dig[0] & 1) != 0;
}

bool mpz_is_even(const mpz_t *z) {
    return z->len == 0 || (z->dig[0] & 1) == 0;
}

int mpz_cmp(const mpz_t *z1, const mpz_t *z2) {
    int cmp = z2->neg - z1->neg;
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
int mpz_cmp_sml_int(const mpz_t *z, machine_int_t sml_int) {
    int cmp;
    if (z->neg == 0) {
        if (sml_int < 0) return 1;
        if (sml_int == 0) {
            if (z->len == 0) return 0;
            return 1;
        }
        if (z->len == 0) return -1;
        assert(sml_int < (1 << DIG_SIZE));
        if (z->len != 1) return 1;
        cmp = z->dig[0] - sml_int;
    } else {
        if (sml_int > 0) return -1;
        if (sml_int == 0) {
            if (z->len == 0) return 0;
            return -1;
        }
        if (z->len == 0) return 1;
        assert(sml_int > -(1 << DIG_SIZE));
        if (z->len != 1) return -1;
        cmp = -z->dig[0] - sml_int;
    }
    if (cmp < 0) return -1;
    if (cmp > 0) return 1;
    return 0;
}
#endif

#if 0
these functions are unused

/* returns abs(z)
*/
mpz_t *mpz_abs(const mpz_t *z) {
    mpz_t *z2 = mpz_clone(z);
    z2->neg = 0;
    return z2;
}

/* returns -z
*/
mpz_t *mpz_neg(const mpz_t *z) {
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
    if (dest->neg) {
        dest->neg = 0;
        mpz_dig_t k = 1;
        dest->len = mpn_sub(dest->dig, dest->dig, dest->len, &k, 1);
    } else {
        mpz_dig_t k = 1;
        dest->len = mpn_add(dest->dig, dest->dig, dest->len, &k, 1);
        dest->neg = 1;
    }
}

/* computes dest = lhs << rhs
   can have dest, lhs the same
*/
void mpz_shl_inpl(mpz_t *dest, const mpz_t *lhs, machine_int_t rhs) {
    if (lhs->len == 0 || rhs == 0) {
        mpz_set(dest, lhs);
    } else if (rhs < 0) {
        mpz_shr_inpl(dest, lhs, -rhs);
    } else {
        mpz_need_dig(dest, lhs->len + (rhs + DIG_SIZE - 1) / DIG_SIZE);
        dest->len = mpn_shl(dest->dig, lhs->dig, lhs->len, rhs);
        dest->neg = lhs->neg;
    }
}

/* computes dest = lhs >> rhs
   can have dest, lhs the same
*/
void mpz_shr_inpl(mpz_t *dest, const mpz_t *lhs, machine_int_t rhs) {
    if (lhs->len == 0 || rhs == 0) {
        mpz_set(dest, lhs);
    } else if (rhs < 0) {
        mpz_shl_inpl(dest, lhs, -rhs);
    } else {
        mpz_need_dig(dest, lhs->len);
        dest->len = mpn_shr(dest->dig, lhs->dig, lhs->len, rhs);
        dest->neg = lhs->neg;
        if (dest->neg) {
            // arithmetic shift right, rounding to negative infinity
            uint n_whole = rhs / DIG_SIZE;
            uint n_part = rhs % DIG_SIZE;
            mpz_dig_t round_up = 0;
            for (uint i = 0; i < lhs->len && i < n_whole; i++) {
                if (lhs->dig[i] != 0) {
                    round_up = 1;
                    break;
                }
            }
            if (n_whole < lhs->len && (lhs->dig[n_whole] & ((1 << n_part) - 1)) != 0) {
                round_up = 1;
            }
            if (round_up) {
                dest->len = mpn_add(dest->dig, dest->dig, dest->len, &round_up, 1);
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
    if (mpn_cmp(lhs->dig, lhs->len, rhs->dig, rhs->len) < 0) {
        const mpz_t *temp = lhs;
        lhs = rhs;
        rhs = temp;
    }

    if (lhs->neg == rhs->neg) {
        mpz_need_dig(dest, lhs->len);
        dest->len = mpn_and(dest->dig, lhs->dig, lhs->len, rhs->dig, rhs->len);
    } else {
        mpz_need_dig(dest, lhs->len);
        // TODO
        assert(0);
//        dest->len = mpn_and_neg(dest->dig, lhs->dig, lhs->len, rhs->dig, rhs->len);
    }

    dest->neg = lhs->neg;
}

/* computes dest = lhs | rhs
   can have dest, lhs, rhs the same
*/
void mpz_or_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs) {
    if (mpn_cmp(lhs->dig, lhs->len, rhs->dig, rhs->len) < 0) {
        const mpz_t *temp = lhs;
        lhs = rhs;
        rhs = temp;
    }

    if (lhs->neg == rhs->neg) {
        mpz_need_dig(dest, lhs->len);
        dest->len = mpn_or(dest->dig, lhs->dig, lhs->len, rhs->dig, rhs->len);
    } else {
        mpz_need_dig(dest, lhs->len);
        // TODO
        assert(0);
//        dest->len = mpn_or_neg(dest->dig, lhs->dig, lhs->len, rhs->dig, rhs->len);
    }

    dest->neg = lhs->neg;
}

/* computes dest = lhs ^ rhs
   can have dest, lhs, rhs the same
*/
void mpz_xor_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs) {
    if (mpn_cmp(lhs->dig, lhs->len, rhs->dig, rhs->len) < 0) {
        const mpz_t *temp = lhs;
        lhs = rhs;
        rhs = temp;
    }

    if (lhs->neg == rhs->neg) {
        mpz_need_dig(dest, lhs->len);
        dest->len = mpn_xor(dest->dig, lhs->dig, lhs->len, rhs->dig, rhs->len);
    } else {
        mpz_need_dig(dest, lhs->len);
        // TODO
        assert(0);
//        dest->len = mpn_xor_neg(dest->dig, lhs->dig, lhs->len, rhs->dig, rhs->len);
    }

    dest->neg = 0;
}

/* computes dest = lhs * rhs
   can have dest, lhs, rhs the same
*/
void mpz_mul_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs)
{
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
        if (mpz_is_odd(n)) {
            mpz_mul_inpl(dest, dest, x);
        }
        mpz_mul_inpl(x, x, x);
        n->len = mpn_shr(n->dig, n->dig, n->len, 1);
    }

    mpz_free(x);
    mpz_free(n);
}

/* computes gcd(z1, z2)
   based on Knuth's modified gcd algorithm (I think?)
   gcd(z1, z2) >= 0
   gcd(0, 0) = 0
   gcd(z, 0) = abs(z)
*/
mpz_t *mpz_gcd(const mpz_t *z1, const mpz_t *z2) {
    if (z1->len == 0) {
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
    mpz_t c; mpz_init_zero(&c);
    a->neg = 0;
    b->neg = 0;

    for (;;) {
        if (mpz_cmp(a, b) < 0) {
            if (a->len == 0) {
                mpz_free(a);
                mpz_deinit(&c);
                return b;
            }
            mpz_t *t = a; a = b; b = t;
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
mpz_t *mpz_lcm(const mpz_t *z1, const mpz_t *z2)
{
    if (z1->len == 0 || z2->len == 0)
        return mpz_zero();

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

/* computes new integers in quo and rem such that:
       quo * rhs + rem = lhs
       0 <= rem < rhs
   can have lhs, rhs the same
*/
void mpz_divmod_inpl(mpz_t *dest_quo, mpz_t *dest_rem, const mpz_t *lhs, const mpz_t *rhs) {
    if (rhs->len == 0) {
        mpz_set_from_int(dest_quo, 0);
        mpz_set_from_int(dest_rem, 0);
        return;
    }

    mpz_need_dig(dest_quo, lhs->len + 1); // +1 necessary?
    memset(dest_quo->dig, 0, (lhs->len + 1) * sizeof(mpz_dig_t));
    dest_quo->len = 0;
    mpz_need_dig(dest_rem, lhs->len + 1); // +1 necessary?
    mpz_set(dest_rem, lhs);
    //rhs->dig[rhs->len] = 0;
    mpn_div(dest_rem->dig, &dest_rem->len, rhs->dig, rhs->len, dest_quo->dig, &dest_quo->len);

    if (lhs->neg != rhs->neg) {
        dest_quo->neg = 1;
    }
}

#if 0
these functions are unused

/* computes floor(lhs / rhs)
   can have lhs, rhs the same
*/
mpz_t *mpz_div(const mpz_t *lhs, const mpz_t *rhs) {
    mpz_t *quo = mpz_zero();
    mpz_t rem; mpz_init_zero(&rem);
    mpz_divmod_inpl(quo, &rem, lhs, rhs);
    mpz_deinit(&rem);
    return quo;
}

/* computes lhs % rhs ( >= 0)
   can have lhs, rhs the same
*/
mpz_t *mpz_mod(const mpz_t *lhs, const mpz_t *rhs) {
    mpz_t quo; mpz_init_zero(&quo);
    mpz_t *rem = mpz_zero();
    mpz_divmod_inpl(&quo, rem, lhs, rhs);
    mpz_deinit(&quo);
    return rem;
}
#endif

machine_int_t mpz_as_int(const mpz_t *i) {
    machine_int_t val = 0;
    mpz_dig_t *d = i->dig + i->len;

    while (--d >= i->dig) {
        machine_int_t oldval = val;
        val = (val << DIG_SIZE) | *d;
        if (val < oldval) {
            // TODO need better handling of conversion overflow
            if (i->neg == 0) {
                return 0x7fffffff;
            } else {
                return 0x80000000;
            }
        }
    }

    if (i->neg != 0) {
        val = -val;
    }

    return val;
}

#if MICROPY_ENABLE_FLOAT
mp_float_t mpz_as_float(const mpz_t *i) {
    mp_float_t val = 0;
    mpz_dig_t *d = i->dig + i->len;

    while (--d >= i->dig) {
        val = val * (1 << DIG_SIZE) + *d;
    }

    if (i->neg != 0) {
        val = -val;
    }

    return val;
}
#endif

uint mpz_as_str_size(const mpz_t *i, uint base) {
    if (base < 2 || base > 32) {
        return 0;
    }

    return i->len * DIG_SIZE / log_base2_floor[base] + 2 + 1; // +1 for null byte termination
}

char *mpz_as_str(const mpz_t *i, uint base) {
    char *s = m_new(char, mpz_as_str_size(i, base));
    mpz_as_str_inpl(i, base, s);
    return s;
}

// assumes enough space as calculated by mpz_as_str_size
// returns length of string, not including null byte
uint mpz_as_str_inpl(const mpz_t *i, uint base, char *str) {
    if (str == NULL || base < 2 || base > 32) {
        str[0] = 0;
        return 0;
    }

    uint ilen = i->len;

    if (ilen == 0) {
        str[0] = '0';
        str[1] = 0;
        return 1;
    }

    // make a copy of mpz digits
    mpz_dig_t *dig = m_new(mpz_dig_t, ilen);
    memcpy(dig, i->dig, ilen * sizeof(mpz_dig_t));

    // convert
    char *s = str;
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
            a += 'a' - '9' - 1;
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
    } while (!done);

    if (i->neg != 0) {
        *s++ = '-';
    }

    // reverse string
    for (char *u = str, *v = s - 1; u < v; ++u, --v) {
        char temp = *u;
        *u = *v;
        *v = temp;
    }

    s[0] = 0; // null termination

    return s - str;
}

#endif // MICROPY_LONGINT_IMPL == MICROPY_LONGINT_IMPL_MPZ
