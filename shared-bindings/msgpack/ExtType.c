// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019  Bernhard Boser
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"
#include "py/smallint.h"
#include "py/objproperty.h"
#include "shared-bindings/msgpack/ExtType.h"

//| class ExtType:
//|     """ExtType represents ext type in msgpack."""
//|
//|     def __init__(self, code: int, data: bytes) -> None:
//|         """Constructor
//|         :param int code: type code in range 0~127.
//|         :param bytes data: representation."""
static mp_obj_t mod_msgpack_exttype_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mod_msgpack_extype_obj_t *self = mp_obj_malloc(mod_msgpack_extype_obj_t, &mod_msgpack_exttype_type);
    enum { ARG_code, ARG_data };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_code, MP_ARG_INT | MP_ARG_REQUIRED },
        { MP_QSTR_data, MP_ARG_OBJ | MP_ARG_REQUIRED },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int code = mp_arg_validate_int_range(args[ARG_code].u_int, 0, 127, MP_QSTR_code);

    self->code = code;

    mp_obj_t data = args[ARG_data].u_obj;
    self->data = data;
    return MP_OBJ_FROM_PTR(self);
}


//|     code: int
//|     """The type code, in range 0~127."""
//|     ...
static mp_obj_t mod_msgpack_exttype_get_code(mp_obj_t self_in) {
    mod_msgpack_extype_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(self->code);
}
MP_DEFINE_CONST_FUN_OBJ_1(mod_msgpack_exttype_get_code_obj, mod_msgpack_exttype_get_code);

static mp_obj_t mod_msgpack_exttype_set_code(mp_obj_t self_in, mp_obj_t code_in) {
    mod_msgpack_extype_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int code = mp_obj_get_int(code_in);
    if (code < 0 || code > 127) {
        mp_raise_AttributeError(MP_ERROR_TEXT("code outside range 0~127"));
    }
    self->code = code;
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(mod_msgpack_exttype_set_code_obj, mod_msgpack_exttype_set_code);

MP_PROPERTY_GETSET(mod_msgpack_exttype_code_obj,
    (mp_obj_t)&mod_msgpack_exttype_get_code_obj,
    (mp_obj_t)&mod_msgpack_exttype_set_code_obj);

//|     data: bytes
//|     """Data."""
//|     ...
//|
static mp_obj_t mod_msgpack_exttype_get_data(mp_obj_t self_in) {
    mod_msgpack_extype_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return self->data;
}
MP_DEFINE_CONST_FUN_OBJ_1(mod_msgpack_exttype_get_data_obj, mod_msgpack_exttype_get_data);

static mp_obj_t mod_msgpack_exttype_set_data(mp_obj_t self_in, mp_obj_t data_in) {
    mod_msgpack_extype_obj_t *self = MP_OBJ_TO_PTR(self_in);
    self->data = data_in;
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(mod_msgpack_exttype_set_data_obj, mod_msgpack_exttype_set_data);

MP_PROPERTY_GETSET(mod_msgpack_exttype_data_obj,
    (mp_obj_t)&mod_msgpack_exttype_get_data_obj,
    (mp_obj_t)&mod_msgpack_exttype_set_data_obj);

static mp_rom_map_elem_t mod_msgpack_exttype_locals_dict_table[] = {
    // Properties
    { MP_ROM_QSTR(MP_QSTR_code), MP_ROM_PTR(&mod_msgpack_exttype_code_obj) },
    { MP_ROM_QSTR(MP_QSTR_data), MP_ROM_PTR(&mod_msgpack_exttype_data_obj) },
};
static MP_DEFINE_CONST_DICT(mod_msgpack_exttype_locals_dict, mod_msgpack_exttype_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mod_msgpack_exttype_type,
    MP_QSTR_ExtType,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, mod_msgpack_exttype_make_new,
    locals_dict, &mod_msgpack_exttype_locals_dict
    );
