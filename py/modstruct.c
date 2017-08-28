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

#include <assert.h>
#include <string.h>

#include "py/runtime.h"
#include "py/builtin.h"
#include "py/objtuple.h"
#include "py/binary.h"
#include "py/parsenum.h"

#if MICROPY_PY_STRUCT

/*
    This module implements most of character typecodes from CPython, with
    some extensions:

    O - (Pointer to) an arbitrary Python object. This is useful for callback
        data, etc. Note that you must keep reference to passed object in
        your Python application, otherwise it may be garbage-collected,
        and then when you get back this value from callback it may be
        invalid (and lead to crash).
    S - Pointer to a string (returned as a Python string). Note the
        difference from "Ns", - the latter says "in this place of structure
        is character data of up to N bytes length", while "S" means
        "in this place of a structure is a pointer to zero-terminated
        character data".
 */

STATIC char get_fmt_type(const char **fmt) {
    char t = **fmt;
    switch (t) {
        case '!':
            t = '>';
            break;
        case '@':
        case '=':
        case '<':
        case '>':
            break;
        default:
            return '@';
    }
    // Skip type char
    (*fmt)++;
    return t;
}

STATIC mp_uint_t get_fmt_num(const char **p) {
    const char *num = *p;
    uint len = 1;
    while (unichar_isdigit(*++num)) {
        len++;
    }
    mp_uint_t val = (mp_uint_t)MP_OBJ_SMALL_INT_VALUE(mp_parse_num_integer(*p, len, 10, NULL));
    *p = num;
    return val;
}

STATIC uint calcsize_items(const char *fmt) {
    uint cnt = 0;
    while (*fmt) {
        int num = 1;
        if (unichar_isdigit(*fmt)) {
            num = get_fmt_num(&fmt);
            if (*fmt == 's') {
                num = 1;
            }
        }
        cnt += num;
        fmt++;
    }
    return cnt;
}

STATIC mp_obj_t struct_calcsize(mp_obj_t fmt_in) {
    const char *fmt = mp_obj_str_get_str(fmt_in);
    char fmt_type = get_fmt_type(&fmt);
    mp_uint_t size;
    for (size = 0; *fmt; fmt++) {
        mp_uint_t cnt = 1;
        if (unichar_isdigit(*fmt)) {
            cnt = get_fmt_num(&fmt);
        }

        if (*fmt == 's') {
            size += cnt;
        } else {
            mp_uint_t align;
            size_t sz = mp_binary_get_size(fmt_type, *fmt, &align);
            while (cnt--) {
                // Apply alignment
                size = (size + align - 1) & ~(align - 1);
                size += sz;
            }
        }
    }
    return MP_OBJ_NEW_SMALL_INT(size);
}
MP_DEFINE_CONST_FUN_OBJ_1(struct_calcsize_obj, struct_calcsize);

STATIC mp_obj_t struct_unpack_from(size_t n_args, const mp_obj_t *args) {
    // unpack requires that the buffer be exactly the right size.
    // unpack_from requires that the buffer be "big enough".
    // Since we implement unpack and unpack_from using the same function
    // we relax the "exact" requirement, and only implement "big enough".
    const char *fmt = mp_obj_str_get_str(args[0]);
    char fmt_type = get_fmt_type(&fmt);
    uint num_items = calcsize_items(fmt);
    mp_obj_tuple_t *res = MP_OBJ_TO_PTR(mp_obj_new_tuple(num_items, NULL));
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_READ);
    byte *p = bufinfo.buf;
    byte *end_p = &p[bufinfo.len];
    mp_int_t offset = 0;

    if (n_args > 2) {
        // offset arg provided
        offset = mp_obj_get_int(args[2]);
        if (offset < 0) {
            // negative offsets are relative to the end of the buffer
            offset = bufinfo.len + offset;
            if (offset < 0) {
                mp_raise_ValueError("buffer too small");
            }
        }
        p += offset;
    }

    for (uint i = 0; i < num_items;) {
        mp_uint_t sz = 1;
        if (unichar_isdigit(*fmt)) {
            sz = get_fmt_num(&fmt);
        }
        if (p + sz > end_p) {
            mp_raise_ValueError("buffer too small");
        }
        mp_obj_t item;
        if (*fmt == 's') {
            item = mp_obj_new_bytes(p, sz);
            p += sz;
            res->items[i++] = item;
        } else {
            while (sz--) {
                item = mp_binary_get_val(fmt_type, *fmt, &p);
                res->items[i++] = item;
            }
        }
        fmt++;
    }
    return MP_OBJ_FROM_PTR(res);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(struct_unpack_from_obj, 2, 3, struct_unpack_from);

STATIC void struct_pack_into_internal(mp_obj_t fmt_in, byte *p, byte* end_p, size_t n_args, const mp_obj_t *args) {
    const char *fmt = mp_obj_str_get_str(fmt_in);
    char fmt_type = get_fmt_type(&fmt);

    size_t i;
    for (i = 0; i < n_args;) {
        mp_uint_t sz = 1;
        if (*fmt == '\0') {
            // more arguments given than used by format string; CPython raises struct.error here
            break;
        }
        if (unichar_isdigit(*fmt)) {
            sz = get_fmt_num(&fmt);
        }
        if (p + sz > end_p) {
            mp_raise_ValueError("buffer too small");
        }

        if (*fmt == 's') {
            mp_buffer_info_t bufinfo;
            mp_get_buffer_raise(args[i++], &bufinfo, MP_BUFFER_READ);
            mp_uint_t to_copy = sz;
            if (bufinfo.len < to_copy) {
                to_copy = bufinfo.len;
            }
            memcpy(p, bufinfo.buf, to_copy);
            memset(p + to_copy, 0, sz - to_copy);
            p += sz;
        } else {
            while (sz--) {
                mp_binary_set_val(fmt_type, *fmt, args[i++], &p);
            }
        }
        fmt++;
    }
}

STATIC mp_obj_t struct_pack(size_t n_args, const mp_obj_t *args) {
    // TODO: "The arguments must match the values required by the format exactly."
    mp_int_t size = MP_OBJ_SMALL_INT_VALUE(struct_calcsize(args[0]));
    vstr_t vstr;
    vstr_init_len(&vstr, size);
    byte *p = (byte*)vstr.buf;
    memset(p, 0, size);
    byte *end_p = &p[size];
    struct_pack_into_internal(args[0], p, end_p, n_args - 1, &args[1]);
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(struct_pack_obj, 1, MP_OBJ_FUN_ARGS_MAX, struct_pack);

STATIC mp_obj_t struct_pack_into(size_t n_args, const mp_obj_t *args) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_WRITE);
    mp_int_t offset = mp_obj_get_int(args[2]);
    if (offset < 0) {
        // negative offsets are relative to the end of the buffer
        offset = (mp_int_t)bufinfo.len + offset;
        if (offset < 0) {
            mp_raise_ValueError("buffer too small");
        }
    }
    byte *p = (byte *)bufinfo.buf;
    byte *end_p = &p[bufinfo.len];
    p += offset;

    struct_pack_into_internal(args[0], p, end_p, n_args - 3, &args[3]);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(struct_pack_into_obj, 3, MP_OBJ_FUN_ARGS_MAX, struct_pack_into);

STATIC const mp_rom_map_elem_t mp_module_struct_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ustruct) },
    { MP_ROM_QSTR(MP_QSTR_calcsize), MP_ROM_PTR(&struct_calcsize_obj) },
    { MP_ROM_QSTR(MP_QSTR_pack), MP_ROM_PTR(&struct_pack_obj) },
    { MP_ROM_QSTR(MP_QSTR_pack_into), MP_ROM_PTR(&struct_pack_into_obj) },
    { MP_ROM_QSTR(MP_QSTR_unpack), MP_ROM_PTR(&struct_unpack_from_obj) },
    { MP_ROM_QSTR(MP_QSTR_unpack_from), MP_ROM_PTR(&struct_unpack_from_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_struct_globals, mp_module_struct_globals_table);

const mp_obj_module_t mp_module_ustruct = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_struct_globals,
};

#endif
