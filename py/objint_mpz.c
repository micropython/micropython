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

#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "py/parsenumbase.h"
#include "py/smallint.h"
#include "py/objint.h"
#include "py/runtime.h"

#if MICROPY_PY_BUILTINS_FLOAT
#include <math.h>
#endif

#if MICROPY_LONGINT_IMPL == MICROPY_LONGINT_IMPL_MPZ

#if MICROPY_PY_SYS_MAXSIZE
// Export value for sys.maxsize
// *FORMAT-OFF*
#define DIG_MASK ((MPZ_LONG_1 << MPZ_DIG_SIZE) - 1)
static const mpz_dig_t maxsize_dig[] = {
    #define NUM_DIG 1
    (MP_SSIZE_MAX >> MPZ_DIG_SIZE * 0) & DIG_MASK,
    #if (MP_SSIZE_MAX >> MPZ_DIG_SIZE * 0) > DIG_MASK
     #undef NUM_DIG
     #define NUM_DIG 2
     (MP_SSIZE_MAX >> MPZ_DIG_SIZE * 1) & DIG_MASK,
     #if (MP_SSIZE_MAX >> MPZ_DIG_SIZE * 1) > DIG_MASK
      #undef NUM_DIG
      #define NUM_DIG 3
      (MP_SSIZE_MAX >> MPZ_DIG_SIZE * 2) & DIG_MASK,
      #if (MP_SSIZE_MAX >> MPZ_DIG_SIZE * 2) > DIG_MASK
       #undef NUM_DIG
       #define NUM_DIG 4
       (MP_SSIZE_MAX >> MPZ_DIG_SIZE * 3) & DIG_MASK,
       #if (MP_SSIZE_MAX >> MPZ_DIG_SIZE * 3) > DIG_MASK
        #error cannot encode MP_SSIZE_MAX as mpz
       #endif
      #endif
     #endif
    #endif
};
// *FORMAT-ON*
const mp_obj_int_t mp_sys_maxsize_obj = {
    {&mp_type_int},
    {.fixed_dig = 1, .len = NUM_DIG, .alloc = NUM_DIG, .dig = (mpz_dig_t *)maxsize_dig}
};
#undef DIG_MASK
#undef NUM_DIG
#endif

mp_obj_int_t *mp_obj_int_new_mpz(void) {
    mp_obj_int_t *o = mp_obj_malloc(mp_obj_int_t, &mp_type_int);
    mpz_init_zero(&o->mpz);
    return o;
}

// This routine expects you to pass in a buffer and size (in *buf and buf_size).
// If, for some reason, this buffer is too small, then it will allocate a
// buffer and return the allocated buffer and size in *buf and *buf_size. It
// is the callers responsibility to free this allocated buffer.
//
// The resulting formatted string will be returned from this function and the
// formatted size will be in *fmt_size.
//
// This particular routine should only be called for the mpz representation of the int.
char *mp_obj_int_formatted_impl(char **buf, size_t *buf_size, size_t *fmt_size, mp_const_obj_t self_in,
    int base, const char *prefix, char base_char, char comma) {
    assert(mp_obj_is_exact_type(self_in, &mp_type_int));
    const mp_obj_int_t *self = MP_OBJ_TO_PTR(self_in);

    size_t needed_size = mp_int_format_size(mpz_max_num_bits(&self->mpz), base, prefix, comma);
    if (needed_size > *buf_size) {
        *buf = m_new(char, needed_size);
        *buf_size = needed_size;
    }
    char *str = *buf;

    *fmt_size = mpz_as_str_inpl(&self->mpz, base, prefix, base_char, comma, str);

    return str;
}

mp_obj_t mp_obj_int_from_bytes_impl(bool big_endian, size_t len, const byte *buf) {
    mp_obj_int_t *o = mp_obj_int_new_mpz();
    mpz_set_from_bytes(&o->mpz, big_endian, len, buf);
    return MP_OBJ_FROM_PTR(o);
}

bool mp_obj_int_to_bytes_impl(mp_obj_t self_in, bool big_endian, size_t len, byte *buf) {
    assert(mp_obj_is_exact_type(self_in, &mp_type_int));
    mp_obj_int_t *self = MP_OBJ_TO_PTR(self_in);
    return mpz_as_bytes(&self->mpz, big_endian, self->mpz.neg, len, buf);
}

int mp_obj_int_sign(mp_obj_t self_in) {
    if (mp_obj_is_small_int(self_in)) {
        mp_int_t val = MP_OBJ_SMALL_INT_VALUE(self_in);
        if (val < 0) {
            return -1;
        } else if (val > 0) {
            return 1;
        } else {
            return 0;
        }
    }
    mp_obj_int_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->mpz.len == 0) {
        return 0;
    } else if (self->mpz.neg == 0) {
        return 1;
    } else {
        return -1;
    }
}

mp_obj_t mp_obj_int_unary_op(mp_unary_op_t op, mp_obj_t o_in) {
    mp_obj_int_t *o = MP_OBJ_TO_PTR(o_in);
    switch (op) {
        case MP_UNARY_OP_BOOL:
            return mp_obj_new_bool(!mpz_is_zero(&o->mpz));
        case MP_UNARY_OP_HASH:
            return MP_OBJ_NEW_SMALL_INT(mpz_hash(&o->mpz));
        case MP_UNARY_OP_POSITIVE:
            return o_in;
        case MP_UNARY_OP_NEGATIVE: { mp_obj_int_t *o2 = mp_obj_int_new_mpz();
                                     mpz_neg_inpl(&o2->mpz, &o->mpz);
                                     return MP_OBJ_FROM_PTR(o2);
        }
        case MP_UNARY_OP_INVERT: { mp_obj_int_t *o2 = mp_obj_int_new_mpz();
                                   mpz_not_inpl(&o2->mpz, &o->mpz);
                                   return MP_OBJ_FROM_PTR(o2);
        }
        case MP_UNARY_OP_ABS: {
            mp_obj_int_t *self = MP_OBJ_TO_PTR(o_in);
            if (self->mpz.neg == 0) {
                return o_in;
            }
            mp_obj_int_t *self2 = mp_obj_int_new_mpz();
            mpz_abs_inpl(&self2->mpz, &self->mpz);
            return MP_OBJ_FROM_PTR(self2);
        }
        case MP_UNARY_OP_INT_MAYBE:
            return o_in;
        default:
            return MP_OBJ_NULL;      // op not supported
    }
}

mp_obj_t mp_obj_int_binary_op(mp_binary_op_t op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    const mpz_t *zlhs;
    const mpz_t *zrhs;
    mpz_t z_int;
    mpz_dig_t z_int_dig[MPZ_NUM_DIG_FOR_INT];

    // lhs could be a small int (eg small-int + mpz)
    if (mp_obj_is_small_int(lhs_in)) {
        mpz_init_fixed_from_int(&z_int, z_int_dig, MPZ_NUM_DIG_FOR_INT, MP_OBJ_SMALL_INT_VALUE(lhs_in));
        zlhs = &z_int;
    } else {
        assert(mp_obj_is_exact_type(lhs_in, &mp_type_int));
        zlhs = &((mp_obj_int_t *)MP_OBJ_TO_PTR(lhs_in))->mpz;
    }

    // if rhs is small int, then lhs was not (otherwise mp_binary_op handles it)
    if (mp_obj_is_small_int(rhs_in)) {
        mpz_init_fixed_from_int(&z_int, z_int_dig, MPZ_NUM_DIG_FOR_INT, MP_OBJ_SMALL_INT_VALUE(rhs_in));
        zrhs = &z_int;
    } else if (mp_obj_is_exact_type(rhs_in, &mp_type_int)) {
        zrhs = &((mp_obj_int_t *)MP_OBJ_TO_PTR(rhs_in))->mpz;
    #if MICROPY_PY_BUILTINS_FLOAT
    } else if (mp_obj_is_float(rhs_in)) {
        return mp_obj_float_binary_op(op, mpz_as_float(zlhs), rhs_in);
    #endif
    #if MICROPY_PY_BUILTINS_COMPLEX
    } else if (mp_obj_is_type(rhs_in, &mp_type_complex)) {
        return mp_obj_complex_binary_op(op, mpz_as_float(zlhs), 0, rhs_in);
    #endif
    } else {
        // delegate to generic function to check for extra cases
        return mp_obj_int_binary_op_extra_cases(op, lhs_in, rhs_in);
    }

    #if MICROPY_PY_BUILTINS_FLOAT
    if (op == MP_BINARY_OP_TRUE_DIVIDE || op == MP_BINARY_OP_INPLACE_TRUE_DIVIDE) {
        if (mpz_is_zero(zrhs)) {
            goto zero_division_error;
        }
        mp_float_t flhs = mpz_as_float(zlhs);
        mp_float_t frhs = mpz_as_float(zrhs);
        return mp_obj_new_float(flhs / frhs);
    }
    #endif

    if (op >= MP_BINARY_OP_INPLACE_OR && op < MP_BINARY_OP_CONTAINS) {
        mp_obj_int_t *res = mp_obj_int_new_mpz();

        switch (op) {
            case MP_BINARY_OP_ADD:
            case MP_BINARY_OP_INPLACE_ADD:
                mpz_add_inpl(&res->mpz, zlhs, zrhs);
                break;
            case MP_BINARY_OP_SUBTRACT:
            case MP_BINARY_OP_INPLACE_SUBTRACT:
                mpz_sub_inpl(&res->mpz, zlhs, zrhs);
                break;
            case MP_BINARY_OP_MULTIPLY:
            case MP_BINARY_OP_INPLACE_MULTIPLY:
                mpz_mul_inpl(&res->mpz, zlhs, zrhs);
                break;
            case MP_BINARY_OP_FLOOR_DIVIDE:
            case MP_BINARY_OP_INPLACE_FLOOR_DIVIDE: {
                if (mpz_is_zero(zrhs)) {
                zero_division_error:
                    mp_raise_msg(&mp_type_ZeroDivisionError, MP_ERROR_TEXT("divide by zero"));
                }
                mpz_t rem;
                mpz_init_zero(&rem);
                mpz_divmod_inpl(&res->mpz, &rem, zlhs, zrhs);
                mpz_deinit(&rem);
                break;
            }
            case MP_BINARY_OP_MODULO:
            case MP_BINARY_OP_INPLACE_MODULO: {
                if (mpz_is_zero(zrhs)) {
                    goto zero_division_error;
                }
                mpz_t quo;
                mpz_init_zero(&quo);
                mpz_divmod_inpl(&quo, &res->mpz, zlhs, zrhs);
                mpz_deinit(&quo);
                break;
            }

            case MP_BINARY_OP_AND:
            case MP_BINARY_OP_INPLACE_AND:
                mpz_and_inpl(&res->mpz, zlhs, zrhs);
                break;
            case MP_BINARY_OP_OR:
            case MP_BINARY_OP_INPLACE_OR:
                mpz_or_inpl(&res->mpz, zlhs, zrhs);
                break;
            case MP_BINARY_OP_XOR:
            case MP_BINARY_OP_INPLACE_XOR:
                mpz_xor_inpl(&res->mpz, zlhs, zrhs);
                break;

            case MP_BINARY_OP_LSHIFT:
            case MP_BINARY_OP_INPLACE_LSHIFT:
            case MP_BINARY_OP_RSHIFT:
            case MP_BINARY_OP_INPLACE_RSHIFT: {
                mp_int_t irhs = mp_obj_int_get_checked(rhs_in);
                if (irhs < 0) {
                    mp_raise_ValueError(MP_ERROR_TEXT("negative shift count"));
                }
                if (op == MP_BINARY_OP_LSHIFT || op == MP_BINARY_OP_INPLACE_LSHIFT) {
                    mpz_shl_inpl(&res->mpz, zlhs, irhs);
                } else {
                    mpz_shr_inpl(&res->mpz, zlhs, irhs);
                }
                break;
            }

            case MP_BINARY_OP_POWER:
            case MP_BINARY_OP_INPLACE_POWER:
                if (mpz_is_neg(zrhs)) {
                    #if MICROPY_PY_BUILTINS_FLOAT
                    return mp_obj_float_binary_op(op, mpz_as_float(zlhs), rhs_in);
                    #else
                    mp_raise_ValueError(MP_ERROR_TEXT("negative power with no float support"));
                    #endif
                }
                mpz_pow_inpl(&res->mpz, zlhs, zrhs);
                break;

            case MP_BINARY_OP_DIVMOD: {
                if (mpz_is_zero(zrhs)) {
                    goto zero_division_error;
                }
                mp_obj_int_t *quo = mp_obj_int_new_mpz();
                mpz_divmod_inpl(&quo->mpz, &res->mpz, zlhs, zrhs);
                mp_obj_t tuple[2] = {MP_OBJ_FROM_PTR(quo), MP_OBJ_FROM_PTR(res)};
                return mp_obj_new_tuple(2, tuple);
            }

            default:
                return MP_OBJ_NULL; // op not supported
        }

        // Check if the result fits in a small-int, and if so just return that.
        mp_int_t res_small;
        if (mpz_as_int_checked(&res->mpz, &res_small)) {
            if (MP_SMALL_INT_FITS(res_small)) {
                return MP_OBJ_NEW_SMALL_INT(res_small);
            }
        }

        return MP_OBJ_FROM_PTR(res);

    } else {
        int cmp = mpz_cmp(zlhs, zrhs);
        switch (op) {
            case MP_BINARY_OP_LESS:
                return mp_obj_new_bool(cmp < 0);
            case MP_BINARY_OP_MORE:
                return mp_obj_new_bool(cmp > 0);
            case MP_BINARY_OP_LESS_EQUAL:
                return mp_obj_new_bool(cmp <= 0);
            case MP_BINARY_OP_MORE_EQUAL:
                return mp_obj_new_bool(cmp >= 0);
            case MP_BINARY_OP_EQUAL:
                return mp_obj_new_bool(cmp == 0);

            default:
                return MP_OBJ_NULL; // op not supported
        }
    }
}

#if MICROPY_PY_BUILTINS_POW3
static mpz_t *mp_mpz_for_int(mp_obj_t arg, mpz_t *temp) {
    if (mp_obj_is_small_int(arg)) {
        mpz_init_from_int(temp, MP_OBJ_SMALL_INT_VALUE(arg));
        return temp;
    } else {
        mp_obj_int_t *arp_p = MP_OBJ_TO_PTR(arg);
        return &(arp_p->mpz);
    }
}

mp_obj_t mp_obj_int_pow3(mp_obj_t base, mp_obj_t exponent,  mp_obj_t modulus) {
    if (!mp_obj_is_int(base) || !mp_obj_is_int(exponent) || !mp_obj_is_int(modulus)) {
        mp_raise_TypeError(MP_ERROR_TEXT("pow() with 3 arguments requires integers"));
    } else {
        mp_obj_t result = mp_obj_new_int_from_ull(0); // Use the _from_ull version as this forces an mpz int
        mp_obj_int_t *res_p = (mp_obj_int_t *)MP_OBJ_TO_PTR(result);

        mpz_t l_temp, r_temp, m_temp;
        mpz_t *lhs = mp_mpz_for_int(base,     &l_temp);
        mpz_t *rhs = mp_mpz_for_int(exponent, &r_temp);
        mpz_t *mod = mp_mpz_for_int(modulus,  &m_temp);

        mpz_pow3_inpl(&(res_p->mpz), lhs, rhs, mod);

        if (lhs == &l_temp) {
            mpz_deinit(lhs);
        }
        if (rhs == &r_temp) {
            mpz_deinit(rhs);
        }
        if (mod == &m_temp) {
            mpz_deinit(mod);
        }
        return result;
    }
}
#endif

mp_obj_t mp_obj_new_int(mp_int_t value) {
    if (MP_SMALL_INT_FITS(value)) {
        return MP_OBJ_NEW_SMALL_INT(value);
    }
    return mp_obj_new_int_from_ll(value);
}

mp_obj_t mp_obj_new_int_from_ll(long long val) {
    mp_obj_int_t *o = mp_obj_int_new_mpz();
    mpz_set_from_ll(&o->mpz, val, true);
    return MP_OBJ_FROM_PTR(o);
}

mp_obj_t mp_obj_new_int_from_ull(unsigned long long val) {
    mp_obj_int_t *o = mp_obj_int_new_mpz();
    mpz_set_from_ll(&o->mpz, val, false);
    return MP_OBJ_FROM_PTR(o);
}

mp_obj_t mp_obj_new_int_from_uint(mp_uint_t value) {
    // SMALL_INT accepts only signed numbers, so make sure the input
    // value fits completely in the small-int positive range.
    if ((value & ~MP_SMALL_INT_POSITIVE_MASK) == 0) {
        return MP_OBJ_NEW_SMALL_INT(value);
    }
    return mp_obj_new_int_from_ull(value);
}

mp_obj_t mp_obj_new_int_from_str_len(const char **str, size_t len, bool neg, unsigned int base) {
    mp_obj_int_t *o = mp_obj_int_new_mpz();
    size_t n = mpz_set_from_str(&o->mpz, *str, len, neg, base);
    *str += n;
    return MP_OBJ_FROM_PTR(o);
}

mp_int_t mp_obj_int_get_truncated(mp_const_obj_t self_in) {
    if (mp_obj_is_small_int(self_in)) {
        return MP_OBJ_SMALL_INT_VALUE(self_in);
    } else {
        const mp_obj_int_t *self = MP_OBJ_TO_PTR(self_in);
        // hash returns actual int value if it fits in mp_int_t
        return mpz_hash(&self->mpz);
    }
}

mp_int_t mp_obj_int_get_checked(mp_const_obj_t self_in) {
    if (mp_obj_is_small_int(self_in)) {
        return MP_OBJ_SMALL_INT_VALUE(self_in);
    } else {
        const mp_obj_int_t *self = MP_OBJ_TO_PTR(self_in);
        mp_int_t value;
        if (mpz_as_int_checked(&self->mpz, &value)) {
            // mp_obj_int_t objects should always contain a value that is a large
            // integer (if the value fits in a small-int then it should have been
            // converted to a small-int object), and so this code-path should never
            // be taken in normal circumstances.
            return value;
        } else {
            // overflow
            mp_raise_msg(&mp_type_OverflowError, MP_ERROR_TEXT("overflow converting long int to machine word"));
        }
    }
}

mp_uint_t mp_obj_int_get_uint_checked(mp_const_obj_t self_in) {
    if (mp_obj_is_small_int(self_in)) {
        if (MP_OBJ_SMALL_INT_VALUE(self_in) >= 0) {
            return MP_OBJ_SMALL_INT_VALUE(self_in);
        }
    } else {
        const mp_obj_int_t *self = MP_OBJ_TO_PTR(self_in);
        mp_uint_t value;
        if (mpz_as_uint_checked(&self->mpz, &value)) {
            return value;
        }
    }

    mp_raise_msg(&mp_type_OverflowError, MP_ERROR_TEXT("overflow converting long int to machine word"));
}

#if MICROPY_PY_BUILTINS_FLOAT
mp_float_t mp_obj_int_as_float_impl(mp_obj_t self_in) {
    assert(mp_obj_is_exact_type(self_in, &mp_type_int));
    mp_obj_int_t *self = MP_OBJ_TO_PTR(self_in);
    return mpz_as_float(&self->mpz);
}
#endif

#endif
