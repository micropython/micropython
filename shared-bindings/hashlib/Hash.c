// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/hashlib/Hash.h"

#include "py/obj.h"
#include "py/objproperty.h"
#include "py/objstr.h"
#include "py/runtime.h"

//| class Hash:
//|     """In progress hash algorithm. This object is always created by a `hashlib.new()`. It has no
//|     user-visible constructor."""
//|

//|     digest_size: int
//|     """Digest size in bytes"""
static mp_obj_t hashlib_hash_digest_size_get(mp_obj_t self_in) {
    mp_check_self(mp_obj_is_type(self_in, &hashlib_hash_type));
    hashlib_hash_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_hashlib_hash_get_digest_size(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(hashlib_hash_digest_size_get_obj, hashlib_hash_digest_size_get);
MP_PROPERTY_GETTER(hashlib_hash_digest_size_obj, (mp_obj_t)&hashlib_hash_digest_size_get_obj);

//|     def update(self, data: ReadableBuffer) -> None:
//|         """Update the hash with the given bytes.
//|
//|         :param ~circuitpython_typing.ReadableBuffer data: Update the hash from data in this buffer
//|         """
//|         ...
mp_obj_t hashlib_hash_update(mp_obj_t self_in, mp_obj_t buf_in) {
    mp_check_self(mp_obj_is_type(self_in, &hashlib_hash_type));
    hashlib_hash_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);

    common_hal_hashlib_hash_update(self, bufinfo.buf, bufinfo.len);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(hashlib_hash_update_obj, hashlib_hash_update);

//|     def digest(self) -> bytes:
//|         """Returns the current digest as bytes() with a length of `hashlib.Hash.digest_size`."""
//|         ...
//|
static mp_obj_t hashlib_hash_digest(mp_obj_t self_in) {
    mp_check_self(mp_obj_is_type(self_in, &hashlib_hash_type));
    hashlib_hash_obj_t *self = MP_OBJ_TO_PTR(self_in);

    size_t size = common_hal_hashlib_hash_get_digest_size(self);
    mp_obj_t obj = mp_obj_new_bytes_of_zeros(size);
    mp_obj_str_t *o = MP_OBJ_TO_PTR(obj);

    common_hal_hashlib_hash_digest(self, (uint8_t *)o->data, size);
    return obj;
}
static MP_DEFINE_CONST_FUN_OBJ_1(hashlib_hash_digest_obj, hashlib_hash_digest);

static const mp_rom_map_elem_t hashlib_hash_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_digest_size), MP_ROM_PTR(&hashlib_hash_digest_size_obj) },
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&hashlib_hash_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_digest), MP_ROM_PTR(&hashlib_hash_digest_obj) },
};

static MP_DEFINE_CONST_DICT(hashlib_hash_locals_dict, hashlib_hash_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    hashlib_hash_type,
    MP_QSTR_Hash,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    locals_dict, &hashlib_hash_locals_dict
    );
