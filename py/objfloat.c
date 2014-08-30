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
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "mpconfig.h"
#include "nlr.h"
#include "misc.h"
#include "qstr.h"
#include "obj.h"
#include "parsenum.h"
#include "runtime0.h"
#include "runtime.h"

#if MICROPY_PY_BUILTINS_FLOAT

#if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
#include "formatfloat.h"
#endif

STATIC void float_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t o_in, mp_print_kind_t kind) {
    mp_obj_float_t *o = o_in;
#if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
    char buf[32];
    format_float(o->value, buf, sizeof(buf), 'g', 6, '\0');
    print(env, "%s", buf);
    if (strchr(buf, '.') == NULL) {
        // Python floats always have decimal point
        print(env, ".0");
    }
#else
    char buf[32];
    sprintf(buf, "%.17g", (double) o->value);
    print(env, buf);
    if (strchr(buf, '.') == NULL) {
        // Python floats always have decimal point
        print(env, ".0");
    }
#endif
}

STATIC mp_obj_t float_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);

    switch (n_args) {
        case 0:
            return mp_obj_new_float(0);

        case 1:
        default:
            if (MP_OBJ_IS_STR(args[0])) {
                // a string, parse it
                mp_uint_t l;
                const char *s = mp_obj_str_get_data(args[0], &l);
                return mp_parse_num_decimal(s, l, false, false);
            } else if (MP_OBJ_IS_TYPE(args[0], &mp_type_float)) {
                // a float, just return it
                return args[0];
            } else {
                // something else, try to cast it to a float
                return mp_obj_new_float(mp_obj_get_float(args[0]));
            }
    }
}

STATIC mp_obj_t float_unary_op(mp_uint_t op, mp_obj_t o_in) {
    mp_obj_float_t *o = o_in;
    switch (op) {
        case MP_UNARY_OP_BOOL: return MP_BOOL(o->value != 0);
        case MP_UNARY_OP_POSITIVE: return o_in;
        case MP_UNARY_OP_NEGATIVE: return mp_obj_new_float(-o->value);
        default: return MP_OBJ_NULL; // op not supported
    }
}

STATIC mp_obj_t float_binary_op(mp_uint_t op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    mp_obj_float_t *lhs = lhs_in;
#if MICROPY_PY_BUILTINS_COMPLEX
    if (MP_OBJ_IS_TYPE(rhs_in, &mp_type_complex)) {
        return mp_obj_complex_binary_op(op, lhs->value, 0, rhs_in);
    } else
#endif
    {
        return mp_obj_float_binary_op(op, lhs->value, rhs_in);
    }
}

const mp_obj_type_t mp_type_float = {
    { &mp_type_type },
    .name = MP_QSTR_float,
    .print = float_print,
    .make_new = float_make_new,
    .unary_op = float_unary_op,
    .binary_op = float_binary_op,
};

mp_obj_t mp_obj_new_float(mp_float_t value) {
    mp_obj_float_t *o = m_new(mp_obj_float_t, 1);
    o->base.type = &mp_type_float;
    o->value = value;
    return (mp_obj_t)o;
}

mp_float_t mp_obj_float_get(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_float));
    mp_obj_float_t *self = self_in;
    return self->value;
}

mp_obj_t mp_obj_float_binary_op(mp_uint_t op, mp_float_t lhs_val, mp_obj_t rhs_in) {
    mp_float_t rhs_val = mp_obj_get_float(rhs_in); // can be any type, this function will convert to float (if possible)
    switch (op) {
        case MP_BINARY_OP_ADD:
        case MP_BINARY_OP_INPLACE_ADD: lhs_val += rhs_val; break;
        case MP_BINARY_OP_SUBTRACT:
        case MP_BINARY_OP_INPLACE_SUBTRACT: lhs_val -= rhs_val; break;
        case MP_BINARY_OP_MULTIPLY:
        case MP_BINARY_OP_INPLACE_MULTIPLY: lhs_val *= rhs_val; break;
        // TODO: verify that C floor matches Python semantics
        case MP_BINARY_OP_FLOOR_DIVIDE:
        case MP_BINARY_OP_INPLACE_FLOOR_DIVIDE:
            if (rhs_val == 0) {
                zero_division_error:
                nlr_raise(mp_obj_new_exception_msg(&mp_type_ZeroDivisionError, "float division by zero"));
            }
            lhs_val = MICROPY_FLOAT_C_FUN(floor)(lhs_val / rhs_val);
            break;
        case MP_BINARY_OP_TRUE_DIVIDE:
        case MP_BINARY_OP_INPLACE_TRUE_DIVIDE:
            if (rhs_val == 0) {
                goto zero_division_error;
            }
            lhs_val /= rhs_val;
            break;
        case MP_BINARY_OP_POWER:
        case MP_BINARY_OP_INPLACE_POWER: lhs_val = MICROPY_FLOAT_C_FUN(pow)(lhs_val, rhs_val); break;
        case MP_BINARY_OP_LESS: return MP_BOOL(lhs_val < rhs_val);
        case MP_BINARY_OP_MORE: return MP_BOOL(lhs_val > rhs_val);
        case MP_BINARY_OP_EQUAL: return MP_BOOL(lhs_val == rhs_val);
        case MP_BINARY_OP_LESS_EQUAL: return MP_BOOL(lhs_val <= rhs_val);
        case MP_BINARY_OP_MORE_EQUAL: return MP_BOOL(lhs_val >= rhs_val);

        default:
            return MP_OBJ_NULL; // op not supported
    }
    return mp_obj_new_float(lhs_val);
}

#endif // MICROPY_PY_BUILTINS_FLOAT
