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

typedef uint16_t mpz_dig_t;
typedef uint32_t mpz_dbl_dig_t;
typedef int32_t mpz_dbl_dig_signed_t;

typedef struct _mpz_t {
    mp_uint_t neg : 1;
    mp_uint_t fixed_dig : 1;
    mp_uint_t alloc : 30;
    mp_uint_t len;
    mpz_dig_t *dig;
} mpz_t;

#define MPZ_DIG_SIZE (15) // see mpn_div for why this needs to be at most 15
#define MPZ_NUM_DIG_FOR_INT (sizeof(mp_int_t) * 8 / MPZ_DIG_SIZE + 1)
#define MPZ_NUM_DIG_FOR_LL (sizeof(long long) * 8 / MPZ_DIG_SIZE + 1)

// convenience macro to declare an mpz with a digit array from the stack, initialised by an integer
#define MPZ_CONST_INT(z, val) mpz_t z; mpz_dig_t z ## _digits[MPZ_NUM_DIG_FOR_INT]; mpz_init_fixed_from_int(&z, z_digits, MPZ_NUM_DIG_FOR_INT, val);

void mpz_init_zero(mpz_t *z);
void mpz_init_from_int(mpz_t *z, mp_int_t val);
void mpz_init_fixed_from_int(mpz_t *z, mpz_dig_t *dig, uint dig_alloc, mp_int_t val);
void mpz_deinit(mpz_t *z);

mpz_t *mpz_zero();
mpz_t *mpz_from_int(mp_int_t i);
mpz_t *mpz_from_ll(long long i);
mpz_t *mpz_from_str(const char *str, uint len, bool neg, uint base);
void mpz_free(mpz_t *z);

mpz_t *mpz_clone(const mpz_t *src);

void mpz_set(mpz_t *dest, const mpz_t *src);
void mpz_set_from_int(mpz_t *z, mp_int_t src);
void mpz_set_from_ll(mpz_t *z, long long i);
uint mpz_set_from_str(mpz_t *z, const char *str, uint len, bool neg, uint base);

bool mpz_is_zero(const mpz_t *z);
bool mpz_is_pos(const mpz_t *z);
bool mpz_is_neg(const mpz_t *z);
bool mpz_is_odd(const mpz_t *z);
bool mpz_is_even(const mpz_t *z);

int mpz_cmp(const mpz_t *lhs, const mpz_t *rhs);

mpz_t *mpz_abs(const mpz_t *z);
mpz_t *mpz_neg(const mpz_t *z);
mpz_t *mpz_add(const mpz_t *lhs, const mpz_t *rhs);
mpz_t *mpz_sub(const mpz_t *lhs, const mpz_t *rhs);
mpz_t *mpz_mul(const mpz_t *lhs, const mpz_t *rhs);
mpz_t *mpz_pow(const mpz_t *lhs, const mpz_t *rhs);

void mpz_abs_inpl(mpz_t *dest, const mpz_t *z);
void mpz_neg_inpl(mpz_t *dest, const mpz_t *z);
void mpz_not_inpl(mpz_t *dest, const mpz_t *z);
void mpz_shl_inpl(mpz_t *dest, const mpz_t *lhs, mp_int_t rhs);
void mpz_shr_inpl(mpz_t *dest, const mpz_t *lhs, mp_int_t rhs);
void mpz_add_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs);
void mpz_sub_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs);
void mpz_mul_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs);
void mpz_pow_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs);
void mpz_and_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs);
void mpz_or_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs);
void mpz_xor_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs);

mpz_t *mpz_gcd(const mpz_t *z1, const mpz_t *z2);
mpz_t *mpz_lcm(const mpz_t *z1, const mpz_t *z2);
void mpz_divmod(const mpz_t *lhs, const mpz_t *rhs, mpz_t **quo, mpz_t **rem);
void mpz_divmod_inpl(mpz_t *dest_quo, mpz_t *dest_rem, const mpz_t *lhs, const mpz_t *rhs);
mpz_t *mpz_div(const mpz_t *lhs, const mpz_t *rhs);
mpz_t *mpz_mod(const mpz_t *lhs, const mpz_t *rhs);

mp_int_t mpz_hash(const mpz_t *z);
bool mpz_as_int_checked(const mpz_t *z, mp_int_t *value);
bool mpz_as_uint_checked(const mpz_t *z, mp_uint_t *value);
#if MICROPY_PY_BUILTINS_FLOAT
mp_float_t mpz_as_float(const mpz_t *z);
#endif
uint mpz_as_str_size(const mpz_t *z, uint base);
uint mpz_as_str_size_formatted(const mpz_t *i, uint base, const char *prefix, char comma);
char *mpz_as_str(const mpz_t *z, uint base);
uint mpz_as_str_inpl(const mpz_t *z, uint base, const char *prefix, char base_char, char comma, char *str);
