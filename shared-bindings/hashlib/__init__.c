/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include <string.h>

#include "py/obj.h"
#include "py/mpconfig.h"
#include "py/runtime.h"
#include "shared-bindings/hashlib/__init__.h"
#include "shared-bindings/hashlib/Hash.h"

//| """Hashing related functions
//|
//| |see_cpython_module| :mod:`cpython:hashlib`.
//| """
//|
//| def new(name: str, data: bytes = b"") -> hashlib.Hash:
//|     """Returns a Hash object setup for the named algorithm. Raises ValueError when the named
//|        algorithm is unsupported.
//|
//|     :return: a hash object for the given algorithm
//|     :rtype: hashlib.Hash"""
//|     ...
//|
STATIC mp_obj_t hashlib_new(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_name, ARG_data };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_name, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_data,  MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const char *algorithm = mp_obj_str_get_str(args[ARG_name].u_obj);

    hashlib_hash_obj_t *self = mp_obj_malloc(hashlib_hash_obj_t, &hashlib_hash_type);

    if (!common_hal_hashlib_new(self, algorithm)) {
        mp_raise_ValueError(MP_ERROR_TEXT("Unsupported hash algorithm"));
    }

    if (args[ARG_data].u_obj != mp_const_none) {
        hashlib_hash_update(self, args[ARG_data].u_obj);
    }
    return self;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(hashlib_new_obj, 1, hashlib_new);

STATIC const mp_rom_map_elem_t hashlib_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_hashlib) },

    { MP_ROM_QSTR(MP_QSTR_new), MP_ROM_PTR(&hashlib_new_obj) },

    // Hash is deliberately omitted here because CPython doesn't expose the
    // object on `hashlib` only the internal `_hashlib`.
};

STATIC MP_DEFINE_CONST_DICT(hashlib_module_globals, hashlib_module_globals_table);

const mp_obj_module_t hashlib_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&hashlib_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_hashlib, hashlib_module);
