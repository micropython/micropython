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

#include "py/objtype.h"
#include "py/runtime.h"

typedef struct _mp_obj_object_t {
    mp_obj_base_t base;
} mp_obj_object_t;

static mp_obj_t object_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)args;
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    mp_obj_object_t *o = mp_obj_malloc(mp_obj_object_t, type);
    return MP_OBJ_FROM_PTR(o);
}

#if MICROPY_CPYTHON_COMPAT
static mp_obj_t object___init__(mp_obj_t self) {
    (void)self;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(object___init___obj, object___init__);

static mp_obj_t object___new__(mp_obj_t cls) {
    if (!mp_obj_is_type(cls, &mp_type_type) || !mp_obj_is_instance_type((mp_obj_type_t *)MP_OBJ_TO_PTR(cls))) {
        mp_raise_TypeError(MP_ERROR_TEXT("arg must be user-type"));
    }
    // This executes only "__new__" part of instance creation.
    // TODO: This won't work well for classes with native bases.
    // TODO: This is a hack, should be resolved along the lines of
    // https://github.com/micropython/micropython/issues/606#issuecomment-43685883
    const mp_obj_type_t *native_base;
    return MP_OBJ_FROM_PTR(mp_obj_new_instance(MP_OBJ_TO_PTR(cls), &native_base));
}
static MP_DEFINE_CONST_FUN_OBJ_1(object___new___fun_obj, object___new__);
static MP_DEFINE_CONST_STATICMETHOD_OBJ(object___new___obj, MP_ROM_PTR(&object___new___fun_obj));

#if MICROPY_PY_DELATTR_SETATTR
static mp_obj_t object___setattr__(mp_obj_t self_in, mp_obj_t attr, mp_obj_t value) {
    if (!mp_obj_is_instance_type(mp_obj_get_type(self_in))) {
        mp_raise_TypeError(MP_ERROR_TEXT("arg must be user-type"));
    }

    if (!mp_obj_is_str(attr)) {
        mp_raise_TypeError(NULL);
    }

    mp_obj_instance_t *self = MP_OBJ_TO_PTR(self_in);
    mp_map_lookup(&self->members, attr, MP_MAP_LOOKUP_ADD_IF_NOT_FOUND)->value = value;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(object___setattr___obj, object___setattr__);

static mp_obj_t object___delattr__(mp_obj_t self_in, mp_obj_t attr) {
    if (!mp_obj_is_instance_type(mp_obj_get_type(self_in))) {
        mp_raise_TypeError(MP_ERROR_TEXT("arg must be user-type"));
    }

    if (!mp_obj_is_str(attr)) {
        mp_raise_TypeError(NULL);
    }

    mp_obj_instance_t *self = MP_OBJ_TO_PTR(self_in);
    if (mp_map_lookup(&self->members, attr, MP_MAP_LOOKUP_REMOVE_IF_FOUND) == NULL) {
        mp_raise_msg(&mp_type_AttributeError, MP_ERROR_TEXT("no such attribute"));
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(object___delattr___obj, object___delattr__);
#endif

static const mp_rom_map_elem_t object_locals_dict_table[] = {
    #if MICROPY_CPYTHON_COMPAT
    { MP_ROM_QSTR(MP_QSTR___init__), MP_ROM_PTR(&object___init___obj) },
    #endif
    #if MICROPY_CPYTHON_COMPAT
    { MP_ROM_QSTR(MP_QSTR___new__), MP_ROM_PTR(&object___new___obj) },
    #endif
    #if MICROPY_PY_DELATTR_SETATTR
    { MP_ROM_QSTR(MP_QSTR___setattr__), MP_ROM_PTR(&object___setattr___obj) },
    { MP_ROM_QSTR(MP_QSTR___delattr__), MP_ROM_PTR(&object___delattr___obj) },
    #endif
};

static MP_DEFINE_CONST_DICT(object_locals_dict, object_locals_dict_table);
#endif

#if MICROPY_CPYTHON_COMPAT
#define OBJECT_TYPE_LOCALS_DICT , locals_dict, &object_locals_dict
#else
#define OBJECT_TYPE_LOCALS_DICT
#endif

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_object,
    MP_QSTR_object,
    MP_TYPE_FLAG_NONE,
    make_new, object_make_new
    OBJECT_TYPE_LOCALS_DICT
    );
