// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Paul Sokolovsky
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2017 Michael McWethy
//
// SPDX-License-Identifier: MIT
#include <assert.h>
#include <string.h>

#include "py/runtime.h"
#include "py/binary.h"
#include "py/parsenum.h"
#include "shared-bindings/struct/__init__.h"

static void struct_validate_format(char fmt) {
    #if MICROPY_NONSTANDARD_TYPECODES
    if (fmt == 'S' || fmt == 'O') {
        mp_raise_RuntimeError(MP_ERROR_TEXT("'S' and 'O' are not supported format types"));
    }
    #endif
}

static char get_fmt_type(const char **fmt) {
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

static mp_uint_t get_fmt_num(const char **p) {
    const char *num = *p;
    uint len = 1;
    while (unichar_isdigit(*++num)) {
        len++;
    }
    mp_uint_t val = (mp_uint_t)MP_OBJ_SMALL_INT_VALUE(mp_parse_num_integer(*p, len, 10, NULL));
    *p = num;
    return val;
}

static mp_uint_t calcsize_items(const char *fmt) {
    mp_uint_t cnt = 0;
    while (*fmt) {
        int num = 1;
        if (unichar_isdigit(*fmt)) {
            num = get_fmt_num(&fmt);
            if (*fmt == 's') {
                num = 1;
            }
        }
        // Pad bytes are skipped and don't get included in the item count.
        if (*fmt != 'x') {
            cnt += num;
        }
        fmt++;
    }
    return cnt;
}

mp_uint_t shared_modules_struct_calcsize(mp_obj_t fmt_in) {
    const char *fmt = mp_obj_str_get_str(fmt_in);
    char fmt_type = get_fmt_type(&fmt);

    mp_uint_t size;
    for (size = 0; *fmt; fmt++) {

        struct_validate_format(*fmt);

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
    return size;
}

void shared_modules_struct_pack_into(mp_obj_t fmt_in, byte *p, byte *end_p, size_t n_args, const mp_obj_t *args) {
    const char *fmt = mp_obj_str_get_str(fmt_in);
    char fmt_type = get_fmt_type(&fmt);
    const mp_uint_t total_sz = shared_modules_struct_calcsize(fmt_in);

    if (p + total_sz > end_p) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Buffer too small"));
    }

    size_t i = 0;
    byte *p_base = p;
    while (*fmt) {
        mp_uint_t sz = 1;
        struct_validate_format(*fmt);

        if (unichar_isdigit(*fmt)) {
            sz = get_fmt_num(&fmt);
        }

        if (*fmt == 's') {
            if (i < n_args) {
                mp_buffer_info_t bufinfo;
                mp_get_buffer_raise(args[i], &bufinfo, MP_BUFFER_READ);
                mp_uint_t to_copy = sz;
                if (bufinfo.len < to_copy) {
                    to_copy = bufinfo.len;
                }
                memcpy(p, bufinfo.buf, to_copy);
                memset(p + to_copy, 0, sz - to_copy);
            }
            i++;
            p += sz;
        } else {
            while (sz--) {
                // Pad bytes don't have a corresponding argument.
                if (*fmt == 'x') {
                    mp_binary_set_val(fmt_type, *fmt, MP_OBJ_NEW_SMALL_INT(0), p_base, &p);
                } else {
                    if (i < n_args) {
                        mp_binary_set_val(fmt_type, *fmt, args[i], p_base, &p);
                    }
                    i++;
                }
            }
        }
        fmt++;
    }
    (void)mp_arg_validate_length(n_args, i, MP_QSTR_values);
}

mp_obj_tuple_t *shared_modules_struct_unpack_from(mp_obj_t fmt_in, byte *p, byte *end_p, bool exact_size) {

    const char *fmt = mp_obj_str_get_str(fmt_in);
    char fmt_type = get_fmt_type(&fmt);
    const mp_uint_t num_items = calcsize_items(fmt);
    const mp_uint_t total_sz = shared_modules_struct_calcsize(fmt_in);
    mp_obj_tuple_t *res = MP_OBJ_TO_PTR(mp_obj_new_tuple(num_items, NULL));

    // If exact_size, make sure the buffer is exactly the right size.
    // Otherwise just make sure it's big enough.
    if (exact_size) {
        if (p + total_sz != end_p) {
            mp_raise_RuntimeError(MP_ERROR_TEXT("buffer size must match format"));
        }
    } else {
        if (p + total_sz > end_p) {
            mp_raise_RuntimeError(MP_ERROR_TEXT("buffer too small"));
        }
    }

    byte *p_base = p;
    for (uint i = 0; i < num_items;) {
        mp_uint_t sz = 1;

        struct_validate_format(*fmt);

        if (unichar_isdigit(*fmt)) {
            sz = get_fmt_num(&fmt);
        }
        mp_obj_t item;
        if (*fmt == 's') {
            item = mp_obj_new_bytes(p, sz);
            p += sz;
            res->items[i++] = item;
        } else {
            while (sz--) {
                item = mp_binary_get_val(fmt_type, *fmt, p_base, &p);
                // Pad bytes are not stored.
                if (*fmt != 'x') {
                    res->items[i++] = item;
                }
            }
        }
        fmt++;
    }
    return res;

}
