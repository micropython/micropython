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

#include <string.h>

#include "py/objtuple.h"
#include "py/runtime.h"
#include "py/objstr.h"

#if MICROPY_PY_COLLECTIONS

typedef struct _mp_obj_namedtuple_type_t {
    mp_obj_type_t base;
    size_t n_fields;
    qstr fields[];
} mp_obj_namedtuple_type_t;

typedef struct _mp_obj_namedtuple_t {
    mp_obj_tuple_t tuple;
} mp_obj_namedtuple_t;

STATIC size_t namedtuple_find_field(const mp_obj_namedtuple_type_t *type, qstr name) {
    for (size_t i = 0; i < type->n_fields; i++) {
        if (type->fields[i] == name) {
            return i;
        }
    }
    return (size_t)-1;
}

STATIC void namedtuple_print(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_namedtuple_t *o = MP_OBJ_TO_PTR(o_in);
    mp_printf(print, "%q", o->tuple.base.type->name);
    const qstr *fields = ((mp_obj_namedtuple_type_t*)o->tuple.base.type)->fields;
    mp_obj_attrtuple_print_helper(print, fields, &o->tuple);
}

STATIC void namedtuple_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        mp_obj_namedtuple_t *self = MP_OBJ_TO_PTR(self_in);
        size_t id = namedtuple_find_field((mp_obj_namedtuple_type_t*)self->tuple.base.type, attr);
        if (id == (size_t)-1) {
            return;
        }
        dest[0] = self->tuple.items[id];
    } else {
        // delete/store attribute
        // provide more detailed error message than we'd get by just returning
        mp_raise_msg(&mp_type_AttributeError, "can't set attribute");
    }
}

STATIC mp_obj_t namedtuple_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    const mp_obj_namedtuple_type_t *type = (const mp_obj_namedtuple_type_t*)type_in;
    size_t num_fields = type->n_fields;
    if (n_args + n_kw != num_fields) {
        if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
            mp_arg_error_terse_mismatch();
        } else if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_NORMAL) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError,
                "function takes %d positional arguments but %d were given",
                num_fields, n_args + n_kw));
        } else if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_DETAILED) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError,
                "%q() takes %d positional arguments but %d were given",
                type->base.name, num_fields, n_args + n_kw));
        }
    }

    // Create a tuple and set the type to this namedtuple
    mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR(mp_obj_new_tuple(num_fields, NULL));
    tuple->base.type = type_in;

    // Copy the positional args into the first slots of the namedtuple
    memcpy(&tuple->items[0], args, sizeof(mp_obj_t) * n_args);

    // Fill in the remaining slots with the keyword args
    memset(&tuple->items[n_args], 0, sizeof(mp_obj_t) * n_kw);
    for (size_t i = n_args; i < n_args + 2 * n_kw; i += 2) {
        qstr kw = mp_obj_str_get_qstr(args[i]);
        size_t id = namedtuple_find_field(type, kw);
        if (id == (size_t)-1) {
            if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
                mp_arg_error_terse_mismatch();
            } else {
                nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError,
                    "unexpected keyword argument '%q'", kw));
            }
        }
        if (tuple->items[id] != MP_OBJ_NULL) {
            if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
                mp_arg_error_terse_mismatch();
            } else {
                nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError,
                    "function got multiple values for argument '%q'", kw));
            }
        }
        tuple->items[id] = args[i + 1];
    }

    return MP_OBJ_FROM_PTR(tuple);
}

STATIC mp_obj_t mp_obj_new_namedtuple_type(qstr name, size_t n_fields, mp_obj_t *fields) {
    mp_obj_namedtuple_type_t *o = m_new_obj_var(mp_obj_namedtuple_type_t, qstr, n_fields);
    memset(&o->base, 0, sizeof(o->base));
    o->base.base.type = &mp_type_type;
    o->base.name = name;
    o->base.print = namedtuple_print;
    o->base.make_new = namedtuple_make_new;
    o->base.unary_op = mp_obj_tuple_unary_op;
    o->base.binary_op = mp_obj_tuple_binary_op;
    o->base.attr = namedtuple_attr;
    o->base.subscr = mp_obj_tuple_subscr;
    o->base.getiter = mp_obj_tuple_getiter;
    o->base.parent = &mp_type_tuple;
    o->n_fields = n_fields;
    for (size_t i = 0; i < n_fields; i++) {
        o->fields[i] = mp_obj_str_get_qstr(fields[i]);
    }
    return MP_OBJ_FROM_PTR(o);
}

STATIC mp_obj_t new_namedtuple_type(mp_obj_t name_in, mp_obj_t fields_in) {
    qstr name = mp_obj_str_get_qstr(name_in);
    size_t n_fields;
    mp_obj_t *fields;
    #if MICROPY_CPYTHON_COMPAT
    if (MP_OBJ_IS_STR(fields_in)) {
        fields_in = mp_obj_str_split(1, &fields_in);
    }
    #endif
    mp_obj_get_array(fields_in, &n_fields, &fields);
    return mp_obj_new_namedtuple_type(name, n_fields, fields);
}
MP_DEFINE_CONST_FUN_OBJ_2(mp_namedtuple_obj, new_namedtuple_type);

#endif // MICROPY_PY_COLLECTIONS
