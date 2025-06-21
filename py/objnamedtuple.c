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
#include "py/objnamedtuple.h"

#if MICROPY_PY_COLLECTIONS

size_t mp_obj_namedtuple_find_field(const mp_obj_namedtuple_type_t *type, qstr name) {
    for (size_t i = 0; i < type->n_fields; i++) {
        if (type->fields[i] == name) {
            return i;
        }
    }
    return (size_t)-1;
}

#if MICROPY_PY_COLLECTIONS_NAMEDTUPLE__ASDICT
static mp_obj_t namedtuple_asdict(mp_obj_t self_in) {
    mp_obj_namedtuple_t *self = MP_OBJ_TO_PTR(self_in);
    const qstr *fields = ((mp_obj_namedtuple_type_t *)self->tuple.base.type)->fields;
    mp_obj_t dict = mp_obj_new_dict(self->tuple.len);
    // make it an OrderedDict
    mp_obj_dict_t *dictObj = MP_OBJ_TO_PTR(dict);
    dictObj->base.type = &mp_type_ordereddict;
    dictObj->map.is_ordered = 1;
    for (size_t i = 0; i < self->tuple.len; ++i) {
        mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(fields[i]), self->tuple.items[i]);
    }
    return dict;
}
MP_DEFINE_CONST_FUN_OBJ_1(namedtuple_asdict_obj, namedtuple_asdict);
#endif

static void namedtuple_print(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_namedtuple_t *o = MP_OBJ_TO_PTR(o_in);
    mp_printf(print, "%q", (qstr)o->tuple.base.type->name);
    const qstr *fields = ((mp_obj_namedtuple_type_t *)o->tuple.base.type)->fields;
    mp_obj_attrtuple_print_helper(print, fields, &o->tuple);
}

static void namedtuple_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        mp_obj_namedtuple_t *self = MP_OBJ_TO_PTR(self_in);
        #if MICROPY_PY_COLLECTIONS_NAMEDTUPLE__ASDICT
        if (attr == MP_QSTR__asdict) {
            dest[0] = MP_OBJ_FROM_PTR(&namedtuple_asdict_obj);
            dest[1] = self_in;
            return;
        }
        #endif
        size_t id = mp_obj_namedtuple_find_field((mp_obj_namedtuple_type_t *)self->tuple.base.type, attr);
        if (id == (size_t)-1) {
            return;
        }
        dest[0] = self->tuple.items[id];
    } else {
        // delete/store attribute
        // provide more detailed error message than we'd get by just returning
        mp_raise_msg(&mp_type_AttributeError, MP_ERROR_TEXT("can't set attribute"));
    }
}

static mp_obj_t namedtuple_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    const mp_obj_namedtuple_type_t *type = (const mp_obj_namedtuple_type_t *)type_in;
    size_t num_fields = type->n_fields;
    if (n_args + n_kw != num_fields) {
        #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
        mp_arg_error_terse_mismatch();
        #elif MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_NORMAL
        mp_raise_msg_varg(&mp_type_TypeError,
            MP_ERROR_TEXT("function takes %d positional arguments but %d were given"),
            num_fields, n_args + n_kw);
        #elif MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_DETAILED
        mp_raise_msg_varg(&mp_type_TypeError,
            MP_ERROR_TEXT("%q() takes %d positional arguments but %d were given"),
            ((mp_obj_type_t *)&type->base)->name, num_fields, n_args + n_kw);
        #endif
    }

    // Create a namedtuple with explicit malloc. Calling mp_obj_new_tuple
    // with num_fields=0 returns a read-only object.
    mp_obj_tuple_t *tuple = mp_obj_malloc_var(mp_obj_tuple_t, items, mp_obj_t, num_fields, type_in);
    tuple->len = num_fields;

    // Copy the positional args into the first slots of the namedtuple
    memcpy(&tuple->items[0], args, sizeof(mp_obj_t) * n_args);

    // Fill in the remaining slots with the keyword args
    memset(&tuple->items[n_args], 0, sizeof(mp_obj_t) * n_kw);
    for (size_t i = n_args; i < n_args + 2 * n_kw; i += 2) {
        qstr kw = mp_obj_str_get_qstr(args[i]);
        size_t id = mp_obj_namedtuple_find_field(type, kw);
        if (id == (size_t)-1) {
            #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
            mp_arg_error_terse_mismatch();
            #else
            mp_raise_msg_varg(&mp_type_TypeError, MP_ERROR_TEXT("unexpected keyword argument '%q'"), kw);
            #endif
        }
        if (tuple->items[id] != MP_OBJ_NULL) {
            #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
            mp_arg_error_terse_mismatch();
            #else
            mp_raise_msg_varg(&mp_type_TypeError,
                MP_ERROR_TEXT("function got multiple values for argument '%q'"), kw);
            #endif
        }
        tuple->items[id] = args[i + 1];
    }

    return MP_OBJ_FROM_PTR(tuple);
}

mp_obj_namedtuple_type_t *mp_obj_new_namedtuple_base(size_t n_fields, mp_obj_t *fields) {
    mp_obj_namedtuple_type_t *o = m_new_obj_var0(mp_obj_namedtuple_type_t, fields, qstr, n_fields);
    o->n_fields = n_fields;
    for (size_t i = 0; i < n_fields; i++) {
        o->fields[i] = mp_obj_str_get_qstr(fields[i]);
    }
    return o;
}

static mp_obj_t mp_obj_new_namedtuple_type(qstr name, size_t n_fields, mp_obj_t *fields) {
    mp_obj_namedtuple_type_t *o = mp_obj_new_namedtuple_base(n_fields, fields);
    mp_obj_type_t *type = (mp_obj_type_t *)&o->base;
    type->base.type = &mp_type_type;
    type->flags = MP_TYPE_FLAG_EQ_CHECKS_OTHER_TYPE; // can match tuple
    type->name = name;
    MP_OBJ_TYPE_SET_SLOT(type, make_new, namedtuple_make_new, 0);
    MP_OBJ_TYPE_SET_SLOT(type, print, namedtuple_print, 1);
    MP_OBJ_TYPE_SET_SLOT(type, unary_op, mp_obj_tuple_unary_op, 2);
    MP_OBJ_TYPE_SET_SLOT(type, binary_op, mp_obj_tuple_binary_op, 3);
    MP_OBJ_TYPE_SET_SLOT(type, attr, namedtuple_attr, 4);
    MP_OBJ_TYPE_SET_SLOT(type, subscr, mp_obj_tuple_subscr, 5);
    MP_OBJ_TYPE_SET_SLOT(type, iter, mp_obj_tuple_getiter, 6);
    MP_OBJ_TYPE_SET_SLOT(type, parent, &mp_type_tuple, 7);
    return MP_OBJ_FROM_PTR(o);
}

static mp_obj_t new_namedtuple_type(mp_obj_t name_in, mp_obj_t fields_in) {
    qstr name = mp_obj_str_get_qstr(name_in);
    size_t n_fields;
    mp_obj_t *fields;
    #if MICROPY_CPYTHON_COMPAT
    if (mp_obj_is_str(fields_in)) {
        fields_in = mp_obj_str_split(1, &fields_in);
    }
    #endif
    mp_obj_get_array(fields_in, &n_fields, &fields);
    return mp_obj_new_namedtuple_type(name, n_fields, fields);
}
MP_DEFINE_CONST_FUN_OBJ_2(mp_namedtuple_obj, new_namedtuple_type);

#endif // MICROPY_PY_COLLECTIONS
