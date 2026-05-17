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

#include "py/runtime.h"

#if MICROPY_PY_BUILTINS_FILTER

typedef struct _mp_obj_filter_t {
    mp_obj_base_t base;
    mp_obj_t fun;
    mp_obj_t iter;
} mp_obj_filter_t;

static mp_obj_t filter_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 2, 2, false);
    mp_obj_filter_t *o = mp_obj_malloc(mp_obj_filter_t, type);
    o->fun = args[0];
    o->iter = mp_getiter(args[1], NULL);
    return MP_OBJ_FROM_PTR(o);
}

static mp_obj_t filter_iternext(mp_obj_t self_in) {
    mp_check_self(mp_obj_is_type(self_in, &mp_type_filter));
    mp_obj_filter_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t next;
    while ((next = mp_iternext(self->iter)) != MP_OBJ_STOP_ITERATION) {
        mp_obj_t val;
        if (self->fun != mp_const_none) {
            val = mp_call_function_n_kw(self->fun, 1, 0, &next);
        } else {
            val = next;
        }
        if (mp_obj_is_true(val)) {
            return next;
        }
    }
    return MP_OBJ_STOP_ITERATION;
}

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_filter,
    MP_QSTR_filter,
    MP_TYPE_FLAG_ITER_IS_ITERNEXT,
    make_new, filter_make_new,
    iter, filter_iternext
    );

#endif // MICROPY_PY_BUILTINS_FILTER
