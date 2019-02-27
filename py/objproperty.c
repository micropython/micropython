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

#if MICROPY_PY_BUILTINS_PROPERTY

typedef struct _mp_obj_property_t {
    mp_obj_base_t base;
    mp_obj_t proxy[3]; // getter, setter, deleter
} mp_obj_property_t;

STATIC mp_obj_t property_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    enum { ARG_fget, ARG_fset, ARG_fdel, ARG_doc };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj)} },
        { MP_QSTR_, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj)} },
        { MP_QSTR_, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj)} },
        { MP_QSTR_doc, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj)} },
    };
    mp_arg_val_t vals[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, args, MP_ARRAY_SIZE(allowed_args), allowed_args, vals);

    mp_obj_property_t *o = m_new_obj(mp_obj_property_t);
    o->base.type = type;
    o->proxy[0] = vals[ARG_fget].u_obj;
    o->proxy[1] = vals[ARG_fset].u_obj;
    o->proxy[2] = vals[ARG_fdel].u_obj;
    // vals[ARG_doc] is silently discarded
    return MP_OBJ_FROM_PTR(o);
}

STATIC mp_obj_t property_getter(mp_obj_t self_in, mp_obj_t getter) {
    mp_obj_property_t *p2 = m_new_obj(mp_obj_property_t);
    *p2 = *(mp_obj_property_t*)MP_OBJ_TO_PTR(self_in);
    p2->proxy[0] = getter;
    return MP_OBJ_FROM_PTR(p2);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(property_getter_obj, property_getter);

STATIC mp_obj_t property_setter(mp_obj_t self_in, mp_obj_t setter) {
    mp_obj_property_t *p2 = m_new_obj(mp_obj_property_t);
    *p2 = *(mp_obj_property_t*)MP_OBJ_TO_PTR(self_in);
    p2->proxy[1] = setter;
    return MP_OBJ_FROM_PTR(p2);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(property_setter_obj, property_setter);

STATIC mp_obj_t property_deleter(mp_obj_t self_in, mp_obj_t deleter) {
    mp_obj_property_t *p2 = m_new_obj(mp_obj_property_t);
    *p2 = *(mp_obj_property_t*)MP_OBJ_TO_PTR(self_in);
    p2->proxy[2] = deleter;
    return MP_OBJ_FROM_PTR(p2);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(property_deleter_obj, property_deleter);

STATIC const mp_rom_map_elem_t property_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_getter), MP_ROM_PTR(&property_getter_obj) },
    { MP_ROM_QSTR(MP_QSTR_setter), MP_ROM_PTR(&property_setter_obj) },
    { MP_ROM_QSTR(MP_QSTR_deleter), MP_ROM_PTR(&property_deleter_obj) },
};

STATIC MP_DEFINE_CONST_DICT(property_locals_dict, property_locals_dict_table);

const mp_obj_type_t mp_type_property = {
    { &mp_type_type },
    .name = MP_QSTR_property,
    .make_new = property_make_new,
    .locals_dict = (mp_obj_dict_t*)&property_locals_dict,
};

const mp_obj_t *mp_obj_property_get(mp_obj_t self_in) {
    mp_check_self(mp_obj_is_type(self_in, &mp_type_property));
    mp_obj_property_t *self = MP_OBJ_TO_PTR(self_in);
    return self->proxy;
}

#endif // MICROPY_PY_BUILTINS_PROPERTY
