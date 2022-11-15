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

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "py/parsenum.h"
#include "py/runtime.h"

#if MICROPY_PY_BUILTINS_COMPLEX

#include <math.h>
#include "py/formatfloat.h"

typedef struct _mp_obj_complex_t {
    mp_obj_base_t base;
    mp_float_t real;
    mp_float_t imag;
} mp_obj_complex_t;

STATIC void complex_print(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_complex_t *o = MP_OBJ_TO_PTR(o_in);
    #if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
    char buf[16];
    #if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_C
    const int precision = 6;
    #else
    const int precision = 7;
    #endif
    #else
    char buf[32];
    const int precision = 16;
    #endif
    if (o->real == 0) {
        mp_format_float(o->imag, buf, sizeof(buf), 'g', precision, '\0');
        mp_printf(print, "%sj", buf);
    } else {
        mp_format_float(o->real, buf, sizeof(buf), 'g', precision, '\0');
        mp_printf(print, "(%s", buf);
        if (o->imag >= 0 || isnan(o->imag)) {
            mp_print_str(print, "+");
        }
        mp_format_float(o->imag, buf, sizeof(buf), 'g', precision, '\0');
        mp_printf(print, "%sj)", buf);
    }
}

STATIC mp_obj_t complex_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)type_in;
    mp_arg_check_num(n_args, n_kw, 0, 2, false);

    switch (n_args) {
        case 0:
            return mp_obj_new_complex(0, 0);

        case 1:
            if (mp_obj_is_str(args[0])) {
                // a string, parse it
                size_t l;
                const char *s = mp_obj_str_get_data(args[0], &l);
                return mp_parse_num_complex(s, l, NULL);
            } else if (mp_obj_is_type(args[0], &mp_type_complex)) {
                // a complex, just return it
                return args[0];
            } else {
                mp_float_t real, imag;
                mp_obj_get_complex(args[0], &real, &imag);
                return mp_obj_new_complex(real, imag);
            }

        case 2:
        default: {
            mp_float_t real, imag;
            if (mp_obj_is_type(args[0], &mp_type_complex)) {
                mp_obj_complex_get(args[0], &real, &imag);
            } else {
                real = mp_obj_get_float(args[0]);
                imag = 0;
            }
            if (mp_obj_is_type(args[1], &mp_type_complex)) {
                mp_float_t real2, imag2;
                mp_obj_complex_get(args[1], &real2, &imag2);
                real -= imag2;
                imag += real2;
            } else {
                imag += mp_obj_get_float(args[1]);
            }
            return mp_obj_new_complex(real, imag);
        }
    }
}

STATIC mp_obj_t complex_unary_op(mp_unary_op_t op, mp_obj_t o_in) {
    mp_obj_complex_t *o = MP_OBJ_TO_PTR(o_in);
    switch (op) {
        case MP_UNARY_OP_BOOL:
            return mp_obj_new_bool(o->real != 0 || o->imag != 0);
        case MP_UNARY_OP_HASH:
            return MP_OBJ_NEW_SMALL_INT(mp_float_hash(o->real) ^ mp_float_hash(o->imag));
        case MP_UNARY_OP_POSITIVE:
            return o_in;
        case MP_UNARY_OP_NEGATIVE:
            return mp_obj_new_complex(-o->real, -o->imag);
        case MP_UNARY_OP_ABS:
            return mp_obj_new_float(MICROPY_FLOAT_C_FUN(sqrt)(o->real * o->real + o->imag * o->imag));
        default:
            return MP_OBJ_NULL;      // op not supported
    }
}

STATIC mp_obj_t complex_binary_op(mp_binary_op_t op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    mp_obj_complex_t *lhs = MP_OBJ_TO_PTR(lhs_in);
    return mp_obj_complex_binary_op(op, lhs->real, lhs->imag, rhs_in);
}

STATIC void complex_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    if (dest[0] != MP_OBJ_NULL) {
        // not load attribute
        return;
    }
    mp_obj_complex_t *self = MP_OBJ_TO_PTR(self_in);
    if (attr == MP_QSTR_real) {
        dest[0] = mp_obj_new_float(self->real);
    } else if (attr == MP_QSTR_imag) {
        dest[0] = mp_obj_new_float(self->imag);
    }
}

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_complex, MP_QSTR_complex, MP_TYPE_FLAG_EQ_NOT_REFLEXIVE | MP_TYPE_FLAG_EQ_CHECKS_OTHER_TYPE,
    make_new, complex_make_new,
    print, complex_print,
    unary_op, complex_unary_op,
    binary_op, complex_binary_op,
    attr, complex_attr
    );

mp_obj_t mp_obj_new_complex(mp_float_t real, mp_float_t imag) {
    mp_obj_complex_t *o = mp_obj_malloc(mp_obj_complex_t, &mp_type_complex);
    o->real = real;
    o->imag = imag;
    return MP_OBJ_FROM_PTR(o);
}

void mp_obj_complex_get(mp_obj_t self_in, mp_float_t *real, mp_float_t *imag) {
    assert(mp_obj_is_type(self_in, &mp_type_complex));
    mp_obj_complex_t *self = MP_OBJ_TO_PTR(self_in);
    *real = self->real;
    *imag = self->imag;
}

mp_obj_t mp_obj_complex_binary_op(mp_binary_op_t op, mp_float_t lhs_real, mp_float_t lhs_imag, mp_obj_t rhs_in) {
    mp_float_t rhs_real, rhs_imag;
    if (!mp_obj_get_complex_maybe(rhs_in, &rhs_real, &rhs_imag)) {
        return MP_OBJ_NULL; // op not supported
    }

    switch (op) {
        case MP_BINARY_OP_ADD:
        case MP_BINARY_OP_INPLACE_ADD:
            lhs_real += rhs_real;
            lhs_imag += rhs_imag;
            break;
        case MP_BINARY_OP_SUBTRACT:
        case MP_BINARY_OP_INPLACE_SUBTRACT:
            lhs_real -= rhs_real;
            lhs_imag -= rhs_imag;
            break;
        case MP_BINARY_OP_MULTIPLY:
        case MP_BINARY_OP_INPLACE_MULTIPLY: {
            mp_float_t real;
        multiply:
            real = lhs_real * rhs_real - lhs_imag * rhs_imag;
            lhs_imag = lhs_real * rhs_imag + lhs_imag * rhs_real;
            lhs_real = real;
            break;
        }
        case MP_BINARY_OP_FLOOR_DIVIDE:
        case MP_BINARY_OP_INPLACE_FLOOR_DIVIDE:
            mp_raise_TypeError(MP_ERROR_TEXT("can't truncate-divide a complex number"));

        case MP_BINARY_OP_TRUE_DIVIDE:
        case MP_BINARY_OP_INPLACE_TRUE_DIVIDE:
            if (rhs_imag == 0) {
                if (rhs_real == 0) {
                    mp_raise_msg(&mp_type_ZeroDivisionError, MP_ERROR_TEXT("complex divide by zero"));
                }
                lhs_real /= rhs_real;
                lhs_imag /= rhs_real;
            } else if (rhs_real == 0) {
                mp_float_t real = lhs_imag / rhs_imag;
                lhs_imag = -lhs_real / rhs_imag;
                lhs_real = real;
            } else {
                mp_float_t rhs_len_sq = rhs_real * rhs_real + rhs_imag * rhs_imag;
                rhs_real /= rhs_len_sq;
                rhs_imag /= -rhs_len_sq;
                goto multiply;
            }
            break;

        case MP_BINARY_OP_POWER:
        case MP_BINARY_OP_INPLACE_POWER: {
            // z1**z2 = exp(z2*ln(z1))
            //        = exp(z2*(ln(|z1|)+i*arg(z1)))
            //        = exp( (x2*ln1 - y2*arg1) + i*(y2*ln1 + x2*arg1) )
            //        = exp(x3 + i*y3)
            //        = exp(x3)*(cos(y3) + i*sin(y3))
            mp_float_t abs1 = MICROPY_FLOAT_C_FUN(sqrt)(lhs_real * lhs_real + lhs_imag * lhs_imag);
            if (abs1 == 0) {
                if (rhs_imag == 0 && rhs_real >= 0) {
                    lhs_real = (rhs_real == 0);
                } else {
                    mp_raise_msg(&mp_type_ZeroDivisionError, MP_ERROR_TEXT("0.0 to a complex power"));
                }
            } else {
                mp_float_t ln1 = MICROPY_FLOAT_C_FUN(log)(abs1);
                mp_float_t arg1 = MICROPY_FLOAT_C_FUN(atan2)(lhs_imag, lhs_real);
                mp_float_t x3 = rhs_real * ln1 - rhs_imag * arg1;
                mp_float_t y3 = rhs_imag * ln1 + rhs_real * arg1;
                mp_float_t exp_x3 = MICROPY_FLOAT_C_FUN(exp)(x3);
                lhs_real = exp_x3 * MICROPY_FLOAT_C_FUN(cos)(y3);
                lhs_imag = exp_x3 * MICROPY_FLOAT_C_FUN(sin)(y3);
            }
            break;
        }

        case MP_BINARY_OP_EQUAL:
            return mp_obj_new_bool(lhs_real == rhs_real && lhs_imag == rhs_imag);

        default:
            return MP_OBJ_NULL; // op not supported
    }
    return mp_obj_new_complex(lhs_real, lhs_imag);
}

#endif
