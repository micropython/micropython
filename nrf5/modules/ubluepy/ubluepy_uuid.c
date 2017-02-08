/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Glenn Ruben Bakke
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

#include "py/obj.h"
#include "py/runtime.h"

#if MICROPY_PY_UBLUEPY

typedef struct _ubluepy_uuid_obj_t {
    mp_obj_base_t base;
    // UUID value
} ubluepy_uuid_obj_t;

STATIC void ubluepy_uuid_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    ubluepy_uuid_obj_t * self = (ubluepy_uuid_obj_t *)o;
    (void)self;
    mp_printf(print, "UUID(value)");
}

STATIC mp_obj_t ubluepy_uuid_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {

    enum { ARG_NEW_UUID };

    static const mp_arg_t allowed_args[] = {
        { ARG_NEW_UUID, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    ubluepy_uuid_obj_t *s = m_new_obj(ubluepy_uuid_obj_t);
    s->base.type = type;

    // Check if UUID is of one of the following types and convert it:
    // int from 0 -> 0xFFFFFFFF
    // str value
    // another UUID
    // any other value that can be converted to hex string.

    return MP_OBJ_FROM_PTR(s);
}

STATIC const mp_map_elem_t ubluepy_uuid_locals_dict_table[] = {
#if 0
    { MP_OBJ_NEW_QSTR(MP_QSTR_getCommonName), (mp_obj_t)(&ubluepy_uuid_get_common_name_obj) },
    // Properties
    { MP_OBJ_NEW_QSTR(MP_QSTR_binVal), (mp_obj_t)(&ubluepy_uuid_bin_val_obj) },
#endif
};

STATIC MP_DEFINE_CONST_DICT(ubluepy_uuid_locals_dict, ubluepy_uuid_locals_dict_table);

const mp_obj_type_t ubluepy_uuid_type = {
    { &mp_type_type },
    .name = MP_QSTR_UUID,
    .print = ubluepy_uuid_print,
    .make_new = ubluepy_uuid_make_new,
    .locals_dict = (mp_obj_t)&ubluepy_uuid_locals_dict
};

#endif // MICROPY_PY_UBLUEPY
