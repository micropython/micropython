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
#include <assert.h>

#include "py/runtime.h"

#if MICROPY_PY_BUILTINS_ENUMERATE

typedef struct _mp_obj_enumerate_t {
    mp_obj_base_t base;
    mp_obj_t iter;
    mp_int_t cur;
} mp_obj_enumerate_t;

STATIC mp_obj_t enumerate_iternext(mp_obj_t self_in);

STATIC mp_obj_t enumerate_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    #if MICROPY_CPYTHON_COMPAT
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_iterable, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_start, MP_ARG_INT, {.u_int = 0} },
    };

    // parse args
    struct {
        mp_arg_val_t iterable, start;
    } arg_vals;
    mp_arg_parse_all_kw_array(n_args, n_kw, args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, (mp_arg_val_t *)&arg_vals);

    // create enumerate object
    mp_obj_enumerate_t *o = mp_obj_malloc(mp_obj_enumerate_t, type);
    o->iter = mp_getiter(arg_vals.iterable.u_obj, NULL);
    o->cur = arg_vals.start.u_int;
    #else
    mp_arg_check_num(n_args, n_kw, 1, 2, false);
    mp_obj_enumerate_t *o = mp_obj_malloc(mp_obj_enumerate_t, type);
    o->iter = mp_getiter(args[0], NULL);
    o->cur = n_args > 1 ? mp_obj_get_int(args[1]) : 0;
    #endif

    return MP_OBJ_FROM_PTR(o);
}

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_enumerate,
    MP_QSTR_enumerate,
    MP_TYPE_FLAG_ITER_IS_ITERNEXT,
    make_new, enumerate_make_new,
    iter, enumerate_iternext
    );

STATIC mp_obj_t enumerate_iternext(mp_obj_t self_in) {
    assert(mp_obj_is_type(self_in, &mp_type_enumerate));
    mp_obj_enumerate_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t next = mp_iternext(self->iter);
    if (next == MP_OBJ_STOP_ITERATION) {
        return MP_OBJ_STOP_ITERATION;
    } else {
        mp_obj_t items[] = {MP_OBJ_NEW_SMALL_INT(self->cur++), next};
        return mp_obj_new_tuple(2, items);
    }
}

#endif // MICROPY_PY_BUILTINS_ENUMERATE
