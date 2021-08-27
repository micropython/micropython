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

#include <stdlib.h>

#include "py/runtime.h"

#if MICROPY_OBJ_IMMEDIATE_OBJS

#define BOOL_VALUE(o) ((o) == mp_const_false ? 0 : 1)

#else

#define BOOL_VALUE(o) (((mp_obj_bool_t *)MP_OBJ_TO_PTR(o))->value)

typedef struct _mp_obj_bool_t {
    mp_obj_base_t base;
    bool value;
} mp_obj_bool_t;

#endif

STATIC void bool_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    bool value = BOOL_VALUE(self_in);
    if (MICROPY_PY_UJSON && kind == PRINT_JSON) {
        if (value) {
            mp_print_str(print, "true");
        } else {
            mp_print_str(print, "false");
        }
    } else {
        if (value) {
            mp_print_str(print, "True");
        } else {
            mp_print_str(print, "False");
        }
    }
}

STATIC mp_obj_t bool_make_new(const mp_obj_type_t *type_in, size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    (void)type_in;
    mp_arg_check_num(n_args, kw_args, 0, 1, false);

    if (n_args == 0) {
        return mp_const_false;
    } else {
        return mp_obj_new_bool(mp_obj_is_true(args[0]));
    }
}

STATIC mp_obj_t bool_unary_op(mp_unary_op_t op, mp_obj_t o_in) {
    if (op == MP_UNARY_OP_LEN) {
        return MP_OBJ_NULL;
    }
    bool value = BOOL_VALUE(o_in);
    return mp_unary_op(op, MP_OBJ_NEW_SMALL_INT(value));
}

STATIC mp_obj_t bool_binary_op(mp_binary_op_t op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    bool value = BOOL_VALUE(lhs_in);
    return mp_binary_op(op, MP_OBJ_NEW_SMALL_INT(value), rhs_in);
}

const mp_obj_type_t mp_type_bool = {
    { &mp_type_type },
    .flags = MP_TYPE_FLAG_EQ_CHECKS_OTHER_TYPE | MP_TYPE_FLAG_EXTENDED, // can match all numeric types
    .name = MP_QSTR_bool,
    .print = bool_print,
    .make_new = bool_make_new,
    MP_TYPE_EXTENDED_FIELDS(
        .unary_op = bool_unary_op,
        .binary_op = bool_binary_op,
        ),
};

#if !MICROPY_OBJ_IMMEDIATE_OBJS
const mp_obj_bool_t mp_const_false_obj = {{&mp_type_bool}, false};
const mp_obj_bool_t mp_const_true_obj = {{&mp_type_bool}, true};
#endif
