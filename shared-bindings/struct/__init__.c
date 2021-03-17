/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2014 Paul Sokolovsky
 * Copyright (c) 2017 Michael McWethy
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

#include <assert.h>
#include <string.h>

#include "py/runtime.h"
#include "py/builtin.h"
#include "py/objtuple.h"
#include "py/binary.h"
#include "py/parsenum.h"
#include "shared-bindings/struct/__init__.h"
#include "shared-module/struct/__init__.h"
#include "supervisor/shared/translate.h"

//| """Manipulation of c-style data
//|
//| This module implements a subset of the corresponding CPython module,
//| as described below. For more information, refer to the original CPython
//| documentation: struct.
//|
//| Supported size/byte order prefixes: *@*, *<*, *>*, *!*.
//|
//| Supported format codes: *b*, *B*, *x*, *h*, *H*, *i*, *I*, *l*, *L*, *q*, *Q*,
//| *s*, *P*, *f*, *d* (the latter 2 depending on the floating-point support)."""
//|


//| def calcsize(fmt: str) -> int:
//|     """Return the number of bytes needed to store the given fmt."""
//|     ...
//|

STATIC mp_obj_t struct_calcsize(mp_obj_t fmt_in) {

    return MP_OBJ_NEW_SMALL_INT(shared_modules_struct_calcsize(fmt_in));
}
MP_DEFINE_CONST_FUN_OBJ_1(struct_calcsize_obj, struct_calcsize);

//| def pack(fmt: str, *values: Any) -> bytes:
//|     """Pack the values according to the format string fmt.
//|     The return value is a bytes object encoding the values."""
//|     ...
//|

STATIC mp_obj_t struct_pack(size_t n_args, const mp_obj_t *args) {
    mp_int_t size = MP_OBJ_SMALL_INT_VALUE(struct_calcsize(args[0]));
    vstr_t vstr;
    vstr_init_len(&vstr, size);
    byte *p = (byte *)vstr.buf;
    memset(p, 0, size);
    byte *end_p = &p[size];
    shared_modules_struct_pack_into(args[0], p, end_p, n_args - 1, &args[1]);
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(struct_pack_obj, 1, MP_OBJ_FUN_ARGS_MAX, struct_pack);

//| def pack_into(fmt: str, buffer: WriteableBuffer, offset: int, *values: Any) -> None:
//|     """Pack the values according to the format string fmt into a buffer
//|     starting at offset. offset may be negative to count from the end of buffer."""
//|     ...
//|

STATIC mp_obj_t struct_pack_into(size_t n_args, const mp_obj_t *args) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_WRITE);
    mp_int_t offset = mp_obj_get_int(args[2]);
    if (offset < 0) {
        // negative offsets are relative to the end of the buffer
        offset = (mp_int_t)bufinfo.len + offset;
        if (offset < 0) {
            mp_raise_RuntimeError(translate("buffer too small"));
        }
    }
    byte *p = (byte *)bufinfo.buf;
    byte *end_p = &p[bufinfo.len];
    p += offset;

    shared_modules_struct_pack_into(args[0], p, end_p, n_args - 3, &args[3]);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(struct_pack_into_obj, 3, MP_OBJ_FUN_ARGS_MAX, struct_pack_into);

//| def unpack(fmt: str, data: ReadableBuffer) -> Tuple[Any, ...]:
//|     """Unpack from the data according to the format string fmt. The return value
//|     is a tuple of the unpacked values. The buffer size must match the size
//|     required by the format."""
//|     ...
//|

STATIC mp_obj_t struct_unpack(size_t n_args, const mp_obj_t *args) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_READ);
    byte *p = bufinfo.buf;
    byte *end_p = &p[bufinfo.len];

    // true means check the size must be exactly right.
    return MP_OBJ_FROM_PTR(shared_modules_struct_unpack_from(args[0], p, end_p, true));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(struct_unpack_obj, 2, 3, struct_unpack);

//| def unpack_from(fmt: str, data: ReadableBuffer, offset: int = 0) -> Tuple[Any, ...]:
//|     """Unpack from the data starting at offset according to the format string fmt.
//|     offset may be negative to count from the end of buffer. The return value is
//|     a tuple of the unpacked values. The buffer size must be at least as big
//|     as the size required by the form."""
//|     ...
//|

STATIC mp_obj_t struct_unpack_from(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_format, ARG_buffer, ARG_offset };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_format, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_buffer, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_offset, MP_ARG_INT, {.u_int = 0} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_READ);
    byte *p = bufinfo.buf;
    byte *end_p = &p[bufinfo.len];

    mp_int_t offset = args[ARG_offset].u_int;
    if (offset < 0) {
        // negative offsets are relative to the end of the buffer
        offset = bufinfo.len + offset;
        if (offset < 0) {
            mp_raise_RuntimeError(translate("buffer too small"));
        }
    }
    p += offset;

    // false means the size doesn't have to be exact. struct.unpack_from() only requires
    // that be buffer be big enough.
    return MP_OBJ_FROM_PTR(shared_modules_struct_unpack_from(args[ARG_format].u_obj, p, end_p, false));
}
MP_DEFINE_CONST_FUN_OBJ_KW(struct_unpack_from_obj, 0, struct_unpack_from);

STATIC const mp_rom_map_elem_t mp_module_struct_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_struct) },
    { MP_ROM_QSTR(MP_QSTR_calcsize), MP_ROM_PTR(&struct_calcsize_obj) },
    { MP_ROM_QSTR(MP_QSTR_pack), MP_ROM_PTR(&struct_pack_obj) },
    { MP_ROM_QSTR(MP_QSTR_pack_into), MP_ROM_PTR(&struct_pack_into_obj) },
    { MP_ROM_QSTR(MP_QSTR_unpack), MP_ROM_PTR(&struct_unpack_obj) },
    { MP_ROM_QSTR(MP_QSTR_unpack_from), MP_ROM_PTR(&struct_unpack_from_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_struct_globals, mp_module_struct_globals_table);

const mp_obj_module_t struct_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_struct_globals,
};
