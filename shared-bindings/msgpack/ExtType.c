/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019  Bernhard Boser
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
#include "py/smallint.h"
#include "py/objproperty.h"
#include "shared-bindings/msgpack/ExtType.h"

//| class ExtType:
//|     """ExtType represents ext type in msgpack."""
//|     def __init__(self, code: int, data: bytes) -> None:
//|         """Constructor
//|         :param int code: type code in range 0~127.
//|         :param bytes data: representation."""
//|
STATIC mp_obj_t mod_msgpack_exttype_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mod_msgpack_extype_obj_t *self = m_new_obj(mod_msgpack_extype_obj_t);
    self->base.type = &mod_msgpack_exttype_type;
    enum { ARG_code, ARG_data };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_code, MP_ARG_INT | MP_ARG_REQUIRED },
        { MP_QSTR_data, MP_ARG_OBJ | MP_ARG_REQUIRED },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int code = args[ARG_code].u_int;
    if (code < 0 || code > 127) {
        mp_raise_AttributeError(translate("code outside range 0~127"));
    }
    self->code = code;

    mp_obj_t data = args[ARG_data].u_obj;
    self->data = data;
    return MP_OBJ_FROM_PTR(self);
}


//|     code: int
//|     """The type code, in range 0~127."""
//|     ...
//|
STATIC mp_obj_t mod_msgpack_exttype_get_code(mp_obj_t self_in) {
    mod_msgpack_extype_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(self->code);
}
MP_DEFINE_CONST_FUN_OBJ_1(mod_msgpack_exttype_get_code_obj, mod_msgpack_exttype_get_code);

STATIC mp_obj_t mod_msgpack_exttype_set_code(mp_obj_t self_in, mp_obj_t code_in) {
    mod_msgpack_extype_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int code = mp_obj_get_int(code_in);
    if (code < 0 || code > 127) {
        mp_raise_AttributeError(translate("code outside range 0~127"));
    }
    self->code = code;
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(mod_msgpack_exttype_set_code_obj, mod_msgpack_exttype_set_code);

const mp_obj_property_t mod_msgpack_exttype_code_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&mod_msgpack_exttype_get_code_obj,
              (mp_obj_t)&mod_msgpack_exttype_set_code_obj,
              MP_ROM_NONE},
};

//|     data: bytes
//|     """Data."""
//|     ...
//|
STATIC mp_obj_t mod_msgpack_exttype_get_data(mp_obj_t self_in) {
    mod_msgpack_extype_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return self->data;
}
MP_DEFINE_CONST_FUN_OBJ_1(mod_msgpack_exttype_get_data_obj, mod_msgpack_exttype_get_data);

STATIC mp_obj_t mod_msgpack_exttype_set_data(mp_obj_t self_in, mp_obj_t data_in) {
    mod_msgpack_extype_obj_t *self = MP_OBJ_TO_PTR(self_in);
    self->data = data_in;
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(mod_msgpack_exttype_set_data_obj, mod_msgpack_exttype_set_data);

const mp_obj_property_t mod_msgpack_exttype_data_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&mod_msgpack_exttype_get_data_obj,
              (mp_obj_t)&mod_msgpack_exttype_set_data_obj,
              MP_ROM_NONE},
};

STATIC mp_rom_map_elem_t mod_msgpack_exttype_locals_dict_table[] = {
    // Properties
    { MP_ROM_QSTR(MP_QSTR_code), MP_ROM_PTR(&mod_msgpack_exttype_code_obj) },
    { MP_ROM_QSTR(MP_QSTR_data), MP_ROM_PTR(&mod_msgpack_exttype_data_obj) },
};
STATIC MP_DEFINE_CONST_DICT(mod_msgpack_exttype_locals_dict, mod_msgpack_exttype_locals_dict_table);

const mp_obj_type_t mod_msgpack_exttype_type = {
    { &mp_type_type },
    .name = MP_QSTR_ExtType,
    .make_new = mod_msgpack_exttype_make_new,
    .locals_dict = (mp_obj_dict_t *)&mod_msgpack_exttype_locals_dict,
};
