/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
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
STATIC mp_obj_t hashlib_hash_digest_size_get(mp_obj_t self_in) {
    mp_check_self(mp_obj_is_type(self_in, &hashlib_hash_type));
    hashlib_hash_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_hashlib_hash_get_digest_size(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(hashlib_hash_digest_size_get_obj, hashlib_hash_digest_size_get);
MP_PROPERTY_GETTER(hashlib_hash_digest_size_obj, (mp_obj_t)&hashlib_hash_digest_size_get_obj);

//|     def update(self, data: ReadableBuffer) -> None:
//|         """Update the hash with the given bytes.
//|
//|         :param ~circuitpython_typing.ReadableBuffer data: Update the hash from data in this buffer"""
//|         ...
mp_obj_t hashlib_hash_update(mp_obj_t self_in, mp_obj_t buf_in) {
    mp_check_self(mp_obj_is_type(self_in, &hashlib_hash_type));
    hashlib_hash_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);

    common_hal_hashlib_hash_update(self, bufinfo.buf, bufinfo.len);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(hashlib_hash_update_obj, hashlib_hash_update);

//|     def digest(self) -> bytes:
//|         """Returns the current digest as bytes() with a length of `hashlib.Hash.digest_size`."""
//|         ...
//|
STATIC mp_obj_t hashlib_hash_digest(mp_obj_t self_in) {
    mp_check_self(mp_obj_is_type(self_in, &hashlib_hash_type));
    hashlib_hash_obj_t *self = MP_OBJ_TO_PTR(self_in);

    size_t size = common_hal_hashlib_hash_get_digest_size(self);
    mp_obj_t obj = mp_obj_new_bytes_of_zeros(size);
    mp_obj_str_t *o = MP_OBJ_TO_PTR(obj);

    common_hal_hashlib_hash_digest(self, (uint8_t *)o->data, size);
    return obj;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(hashlib_hash_digest_obj, hashlib_hash_digest);

STATIC const mp_rom_map_elem_t hashlib_hash_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_digest_size), MP_ROM_PTR(&hashlib_hash_digest_size_obj) },
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&hashlib_hash_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_digest), MP_ROM_PTR(&hashlib_hash_digest_obj) },
};

STATIC MP_DEFINE_CONST_DICT(hashlib_hash_locals_dict, hashlib_hash_locals_dict_table);

const mp_obj_type_t hashlib_hash_type = {
    { &mp_type_type },
    .name = MP_QSTR_Hash,
    .locals_dict = (mp_obj_dict_t *)&hashlib_hash_locals_dict,
};
