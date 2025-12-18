/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2014 Paul Sokolovsky
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
#include <string.h>

#include "py/smallint.h"
#include "py/objint.h"
#include "py/runtime.h"
#include "py/misc.h"

#if MICROPY_PY_BUILTINS_FLOAT
#include <math.h>
#endif

#if MICROPY_LONGINT_IMPL == MICROPY_LONGINT_IMPL_LONGLONG

#if MICROPY_PY_SYS_MAXSIZE
// Export value for sys.maxsize
const mp_obj_int_t mp_sys_maxsize_obj = {{&mp_type_int}, MP_SSIZE_MAX};
#endif

static void raise_long_long_overflow(void) {
    mp_raise_msg(&mp_type_OverflowError, MP_ERROR_TEXT("result overflows long long storage"));
}

mp_obj_t mp_obj_int_from_bytes_impl(bool big_endian, size_t len, const byte *buf) {
    int delta = 1;
    if (!big_endian) {
        buf += len - 1;
        delta = -1;
    }

    mp_longint_impl_t value = 0;
    for (; len--; buf += delta) {
        value = (value << 8) | *buf;
    }
    return mp_obj_new_int_from_ll(value);
}

bool mp_obj_int_to_bytes_impl(mp_obj_t self_in, bool big_endian, size_t len, byte *buf) {
    assert(mp_obj_is_exact_type(self_in, &mp_type_int));
    mp_obj_int_t *self = self_in;
    long long val = self->val;
    size_t slen; // Number of bytes to represent val

    // This logic has a twin in objint.c
    if (val > 0) {
        slen = (sizeof(long long) * 8 - mp_clzll(val) + 7) / 8;
    } else if (val < -1) {
        slen = (sizeof(long long) * 8 - mp_clzll(~val) + 8) / 8;
    } else {
        // clz of 0 is defined, so 0 and -1 map to 0 and 1
        slen = -val;
    }

    if (slen > len) {
        return false; // Would overflow
        // TODO: Determine whether to copy and truncate, as some callers probably expect this...?
    }

    if (big_endian) {
        byte *b = buf + len;
        while (b > buf) {
            *--b = val;
            val >>= 8;
        }
    } else {
        for (; len > 0; --len) {
            *buf++ = val;
            val >>= 8;
        }
    }
    return true;
}

int mp_obj_int_sign(mp_obj_t self_in) {
    mp_longint_impl_t val;
    if (mp_obj_is_small_int(self_in)) {
        val = MP_OBJ_SMALL_INT_VALUE(self_in);
    } else {
        mp_obj_int_t *self = self_in;
        val = self->val;
    }
    if (val < 0) {
        return -1;
    } else if (val > 0) {
        return 1;
    } else {
        return 0;
    }
}

mp_obj_t mp_obj_int_unary_op(mp_unary_op_t op, mp_obj_t o_in) {
    mp_obj_int_t *o = o_in;
    switch (op) {
        case MP_UNARY_OP_BOOL:
            return mp_obj_new_bool(o->val != 0);

        // truncate value to fit in mp_int_t, which gives the same hash as
        // small int if the value fits without truncation
        case MP_UNARY_OP_HASH:
            return MP_OBJ_NEW_SMALL_INT((mp_int_t)o->val);
        case MP_UNARY_OP_POSITIVE:
            return o_in;
        case MP_UNARY_OP_NEGATIVE:
            return mp_obj_new_int_from_ll(-o->val);
        case MP_UNARY_OP_INVERT:
            return mp_obj_new_int_from_ll(~o->val);
        case MP_UNARY_OP_ABS: {
            mp_obj_int_t *self = MP_OBJ_TO_PTR(o_in);
            if (self->val >= 0) {
                return o_in;
            }
            self = mp_obj_new_int_from_ll(self->val);
            // TODO could overflow long long
            self->val = -self->val;
            return MP_OBJ_FROM_PTR(self);
        }
        case MP_UNARY_OP_INT_MAYBE:
            return o_in;
        default:
            return MP_OBJ_NULL;      // op not supported
    }
}

mp_obj_t mp_obj_int_binary_op(mp_binary_op_t op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    long long lhs_val;
    long long rhs_val;
    bool overflow = false;
    long long result;

    if (mp_obj_is_small_int(lhs_in)) {
        lhs_val = MP_OBJ_SMALL_INT_VALUE(lhs_in);
    } else {
        assert(mp_obj_is_exact_type(lhs_in, &mp_type_int));
        lhs_val = ((mp_obj_int_t *)lhs_in)->val;
    }

    if (mp_obj_is_small_int(rhs_in)) {
        rhs_val = MP_OBJ_SMALL_INT_VALUE(rhs_in);
    } else if (mp_obj_is_exact_type(rhs_in, &mp_type_int)) {
        rhs_val = ((mp_obj_int_t *)rhs_in)->val;
    #if MICROPY_PY_BUILTINS_FLOAT
    } else if (mp_obj_is_float(rhs_in)) {
        return mp_obj_float_binary_op(op, (mp_float_t)lhs_val, rhs_in);
    #endif
    #if MICROPY_PY_BUILTINS_COMPLEX
    } else if (mp_obj_is_type(rhs_in, &mp_type_complex)) {
        return mp_obj_complex_binary_op(op, (mp_float_t)lhs_val, 0, rhs_in);
    #endif
    } else {
        // delegate to generic function to check for extra cases
        return mp_obj_int_binary_op_extra_cases(op, lhs_in, rhs_in);
    }

    #if MICROPY_PY_BUILTINS_FLOAT
    if (op == MP_BINARY_OP_TRUE_DIVIDE || op == MP_BINARY_OP_INPLACE_TRUE_DIVIDE) {
        if (rhs_val == 0) {
            goto zero_division;
        }
        return mp_obj_new_float((mp_float_t)lhs_val / (mp_float_t)rhs_val);
    }
    #endif

    switch (op) {
        case MP_BINARY_OP_ADD:
        case MP_BINARY_OP_INPLACE_ADD:
            overflow = mp_add_ll_overflow(lhs_val, rhs_val, &result);
            break;
        case MP_BINARY_OP_SUBTRACT:
        case MP_BINARY_OP_INPLACE_SUBTRACT:
            overflow = mp_sub_ll_overflow(lhs_val, rhs_val, &result);
            break;
        case MP_BINARY_OP_MULTIPLY:
        case MP_BINARY_OP_INPLACE_MULTIPLY:
            overflow = mp_mul_ll_overflow(lhs_val, rhs_val, &result);
            break;
        case MP_BINARY_OP_FLOOR_DIVIDE:
        case MP_BINARY_OP_INPLACE_FLOOR_DIVIDE:
            if (rhs_val == 0) {
                goto zero_division;
            }
            return mp_obj_new_int_from_ll(lhs_val / rhs_val);
        case MP_BINARY_OP_MODULO:
        case MP_BINARY_OP_INPLACE_MODULO:
            if (rhs_val == 0) {
                goto zero_division;
            }
            return mp_obj_new_int_from_ll(lhs_val % rhs_val);

        case MP_BINARY_OP_AND:
        case MP_BINARY_OP_INPLACE_AND:
            return mp_obj_new_int_from_ll(lhs_val & rhs_val);
        case MP_BINARY_OP_OR:
        case MP_BINARY_OP_INPLACE_OR:
            return mp_obj_new_int_from_ll(lhs_val | rhs_val);
        case MP_BINARY_OP_XOR:
        case MP_BINARY_OP_INPLACE_XOR:
            return mp_obj_new_int_from_ll(lhs_val ^ rhs_val);

        case MP_BINARY_OP_LSHIFT:
        case MP_BINARY_OP_INPLACE_LSHIFT:
            if (rhs_val < 0) {
                // negative shift not allowed
                mp_raise_ValueError(MP_ERROR_TEXT("negative shift count"));
            }
            overflow = rhs_val >= (sizeof(long long) * MP_BITS_PER_BYTE)
                || lhs_val > (LLONG_MAX >> rhs_val)
                || lhs_val < (LLONG_MIN >> rhs_val);
            result = (unsigned long long)lhs_val << rhs_val;
            break;
        case MP_BINARY_OP_RSHIFT:
        case MP_BINARY_OP_INPLACE_RSHIFT:
            if ((int)rhs_val < 0) {
                // negative shift not allowed
                mp_raise_ValueError(MP_ERROR_TEXT("negative shift count"));
            }
            return mp_obj_new_int_from_ll(lhs_val >> (int)rhs_val);

        case MP_BINARY_OP_POWER:
        case MP_BINARY_OP_INPLACE_POWER: {
            if (rhs_val < 0) {
                #if MICROPY_PY_BUILTINS_FLOAT
                return mp_obj_float_binary_op(op, lhs_val, rhs_in);
                #else
                mp_raise_ValueError(MP_ERROR_TEXT("negative power with no float support"));
                #endif
            }
            result = 1;
            while (rhs_val > 0 && !overflow) {
                if (rhs_val & 1) {
                    overflow = mp_mul_ll_overflow(result, lhs_val, &result);
                }
                if (rhs_val == 1 || overflow) {
                    break;
                }
                rhs_val /= 2;
                overflow = mp_mul_ll_overflow(lhs_val, lhs_val, &lhs_val);
            }
            break;
        }

        case MP_BINARY_OP_LESS:
            return mp_obj_new_bool(lhs_val < rhs_val);
        case MP_BINARY_OP_MORE:
            return mp_obj_new_bool(lhs_val > rhs_val);
        case MP_BINARY_OP_LESS_EQUAL:
            return mp_obj_new_bool(lhs_val <= rhs_val);
        case MP_BINARY_OP_MORE_EQUAL:
            return mp_obj_new_bool(lhs_val >= rhs_val);
        case MP_BINARY_OP_EQUAL:
            return mp_obj_new_bool(lhs_val == rhs_val);

        default:
            return MP_OBJ_NULL; // op not supported
    }

    if (overflow) {
        raise_long_long_overflow();
    }

    return mp_obj_new_int_from_ll(result);

zero_division:
    mp_raise_msg(&mp_type_ZeroDivisionError, MP_ERROR_TEXT("divide by zero"));
}

mp_obj_t mp_obj_new_int(mp_int_t value) {
    return mp_obj_new_int_from_ll(value);
}

mp_obj_t mp_obj_new_int_from_uint(mp_uint_t value) {
    return mp_obj_new_int_from_ll(value);
}

mp_obj_t mp_obj_new_int_from_ll(long long val) {
    if ((long long)(mp_int_t)val == val && MP_SMALL_INT_FITS(val)) {
        return MP_OBJ_NEW_SMALL_INT(val);
    }

    mp_obj_int_t *o = mp_obj_malloc(mp_obj_int_t, &mp_type_int);
    o->val = val;
    return MP_OBJ_FROM_PTR(o);
}

mp_obj_t mp_obj_new_int_from_ull(unsigned long long val) {
    if (val >> (sizeof(unsigned long long) * 8 - 1) != 0) {
        raise_long_long_overflow();
    }
    return mp_obj_new_int_from_ll(val);
}

mp_int_t mp_obj_int_get_truncated(mp_const_obj_t self_in) {
    if (mp_obj_is_small_int(self_in)) {
        return MP_OBJ_SMALL_INT_VALUE(self_in);
    } else {
        const mp_obj_int_t *self = self_in;
        return self->val;
    }
}

mp_int_t mp_obj_int_get_checked(mp_const_obj_t self_in) {
    if (mp_obj_is_small_int(self_in)) {
        return MP_OBJ_SMALL_INT_VALUE(self_in);
    } else {
        const mp_obj_int_t *self = self_in;
        long long value = self->val;
        mp_int_t truncated = (mp_int_t)value;
        if ((long long)truncated == value) {
            return truncated;
        }
    }
    mp_raise_msg(&mp_type_OverflowError, MP_ERROR_TEXT("overflow converting long int to machine word"));
}

mp_uint_t mp_obj_int_get_uint_checked(mp_const_obj_t self_in) {
    if (mp_obj_is_small_int(self_in)) {
        if (MP_OBJ_SMALL_INT_VALUE(self_in) >= 0) {
            return MP_OBJ_SMALL_INT_VALUE(self_in);
        }
    } else {
        const mp_obj_int_t *self = self_in;
        long long value = self->val;
        mp_uint_t truncated = (mp_uint_t)value;
        if (value >= 0 && (long long)truncated == value) {
            return truncated;
        }
    }
    mp_raise_msg(&mp_type_OverflowError, MP_ERROR_TEXT("overflow converting long int to machine word"));
}

#if MICROPY_PY_BUILTINS_FLOAT
mp_float_t mp_obj_int_as_float_impl(mp_obj_t self_in) {
    assert(mp_obj_is_exact_type(self_in, &mp_type_int));
    mp_obj_int_t *self = self_in;
    return self->val;
}
#endif

#endif
