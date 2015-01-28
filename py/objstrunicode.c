/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include <string.h>
#include <assert.h>

#include "py/nlr.h"
#include "py/objstr.h"
#include "py/objlist.h"
#include "py/runtime0.h"
#include "py/runtime.h"
#include "py/pfenv.h"

#if MICROPY_PY_BUILTINS_STR_UNICODE

STATIC mp_obj_t mp_obj_new_str_iterator(mp_obj_t str);

/******************************************************************************/
/* str                                                                        */

STATIC void uni_print_quoted(void (*print)(void *env, const char *fmt, ...), void *env, const byte *str_data, uint str_len) {
    // this escapes characters, but it will be very slow to print (calling print many times)
    bool has_single_quote = false;
    bool has_double_quote = false;
    for (const byte *s = str_data, *top = str_data + str_len; !has_double_quote && s < top; s++) {
        if (*s == '\'') {
            has_single_quote = true;
        } else if (*s == '"') {
            has_double_quote = true;
        }
    }
    int quote_char = '\'';
    if (has_single_quote && !has_double_quote) {
        quote_char = '"';
    }
    print(env, "%c", quote_char);
    const byte *s = str_data, *top = str_data + str_len;
    while (s < top) {
        unichar ch;
        ch = utf8_get_char(s);
        s = utf8_next_char(s);
        if (ch == quote_char) {
            print(env, "\\%c", quote_char);
        } else if (ch == '\\') {
            print(env, "\\\\");
        } else if (32 <= ch && ch <= 126) {
            print(env, "%c", ch);
        } else if (ch == '\n') {
            print(env, "\\n");
        } else if (ch == '\r') {
            print(env, "\\r");
        } else if (ch == '\t') {
            print(env, "\\t");
        } else if (ch < 0x100) {
            print(env, "\\x%02x", ch);
        } else if (ch < 0x10000) {
            print(env, "\\u%04x", ch);
        } else {
            print(env, "\\U%08x", ch);
        }
    }
    print(env, "%c", quote_char);
}

STATIC void uni_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    GET_STR_DATA_LEN(self_in, str_data, str_len);
    #if MICROPY_PY_UJSON
    if (kind == PRINT_JSON) {
        mp_str_print_json(print, env, str_data, str_len);
        return;
    }
    #endif
    if (kind == PRINT_STR) {
        print(env, "%.*s", str_len, str_data);
    } else {
        uni_print_quoted(print, env, str_data, str_len);
    }
}

STATIC mp_obj_t uni_unary_op(mp_uint_t op, mp_obj_t self_in) {
    GET_STR_DATA_LEN(self_in, str_data, str_len);
    switch (op) {
        case MP_UNARY_OP_BOOL:
            return MP_BOOL(str_len != 0);
        case MP_UNARY_OP_LEN:
            return MP_OBJ_NEW_SMALL_INT(unichar_charlen((const char *)str_data, str_len));
        default:
            return MP_OBJ_NULL; // op not supported
    }
}

// Convert an index into a pointer to its lead byte. Out of bounds indexing will raise IndexError or
// be capped to the first/last character of the string, depending on is_slice.
const byte *str_index_to_ptr(const mp_obj_type_t *type, const byte *self_data, mp_uint_t self_len,
                             mp_obj_t index, bool is_slice) {
    (void)type;
    mp_int_t i;
    // Copied from mp_get_index; I don't want bounds checking, just give me
    // the integer as-is. (I can't bounds-check without scanning the whole
    // string; an out-of-bounds index will be caught in the loops below.)
    if (MP_OBJ_IS_SMALL_INT(index)) {
        i = MP_OBJ_SMALL_INT_VALUE(index);
    } else if (!mp_obj_get_int_maybe(index, &i)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "string indices must be integers, not %s", mp_obj_get_type_str(index)));
    }
    const byte *s, *top = self_data + self_len;
    if (i < 0)
    {
        // Negative indexing is performed by counting from the end of the string.
        for (s = top - 1; i; --s) {
            if (s < self_data) {
                if (is_slice) {
                    return self_data;
                }
                nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_IndexError, "string index out of range"));
            }
            if (!UTF8_IS_CONT(*s)) {
                ++i;
            }
        }
        ++s;
    } else if (!i) {
        return self_data; // Shortcut - str[0] is its base pointer
    } else {
        // Positive indexing, correspondingly, counts from the start of the string.
        // It's assumed that negative indexing will generally be used with small
        // absolute values (eg str[-1], not str[-1000000]), which means it'll be
        // more efficient this way.
        for (s = self_data; true; ++s) {
            if (s >= top) {
                if (is_slice) {
                    return top;
                }
                nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_IndexError, "string index out of range"));
            }
            while (UTF8_IS_CONT(*s)) {
                ++s;
            }
            if (!i--) {
                return s;
            }
        }
    }
    return s;
}

STATIC mp_obj_t str_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value) {
    mp_obj_type_t *type = mp_obj_get_type(self_in);
    GET_STR_DATA_LEN(self_in, self_data, self_len);
    if (value == MP_OBJ_SENTINEL) {
        // load
#if MICROPY_PY_BUILTINS_SLICE
        if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
            mp_obj_t ostart, ostop, ostep;
            mp_obj_slice_get(index, &ostart, &ostop, &ostep);
            if (ostep != mp_const_none && ostep != MP_OBJ_NEW_SMALL_INT(1)) {
                nlr_raise(mp_obj_new_exception_msg(&mp_type_NotImplementedError,
                    "only slices with step=1 (aka None) are supported"));
            }

            if (type == &mp_type_bytes) {
                mp_int_t start = 0, stop = self_len;
                if (ostart != mp_const_none) {
                    start = MP_OBJ_SMALL_INT_VALUE(ostart);
                    if (start < 0) {
                        start = self_len + start;
                    }
                }
                if (ostop != mp_const_none) {
                    stop = MP_OBJ_SMALL_INT_VALUE(ostop);
                    if (stop < 0) {
                        stop = self_len + stop;
                    }
                }
                return mp_obj_new_str_of_type(type, self_data + start, stop - start);
            }
            const byte *pstart, *pstop;
            if (ostart != mp_const_none) {
                pstart = str_index_to_ptr(type, self_data, self_len, ostart, true);
            } else {
                pstart = self_data;
            }
            if (ostop != mp_const_none) {
                // pstop will point just after the stop character. This depends on
                // the \0 at the end of the string.
                pstop = str_index_to_ptr(type, self_data, self_len, ostop, true);
            } else {
                pstop = self_data + self_len;
            }
            if (pstop < pstart) {
                return MP_OBJ_NEW_QSTR(MP_QSTR_);
            }
            return mp_obj_new_str_of_type(type, (const byte *)pstart, pstop - pstart);
        }
#endif
        if (type == &mp_type_bytes) {
            uint index_val = mp_get_index(type, self_len, index, false);
            return MP_OBJ_NEW_SMALL_INT(self_data[index_val]);
        }
        const byte *s = str_index_to_ptr(type, self_data, self_len, index, false);
        int len = 1;
        if (UTF8_IS_NONASCII(*s)) {
            // Count the number of 1 bits (after the first)
            for (char mask = 0x40; *s & mask; mask >>= 1) {
                ++len;
            }
        }
        return mp_obj_new_str((const char*)s, len, true); // This will create a one-character string
    } else {
        return MP_OBJ_NULL; // op not supported
    }
}

STATIC const mp_map_elem_t struni_locals_dict_table[] = {
#if MICROPY_CPYTHON_COMPAT
    { MP_OBJ_NEW_QSTR(MP_QSTR_encode), (mp_obj_t)&str_encode_obj },
#endif
    { MP_OBJ_NEW_QSTR(MP_QSTR_find), (mp_obj_t)&str_find_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_rfind), (mp_obj_t)&str_rfind_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_index), (mp_obj_t)&str_index_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_rindex), (mp_obj_t)&str_rindex_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_join), (mp_obj_t)&str_join_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_split), (mp_obj_t)&str_split_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_rsplit), (mp_obj_t)&str_rsplit_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_startswith), (mp_obj_t)&str_startswith_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_endswith), (mp_obj_t)&str_endswith_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_strip), (mp_obj_t)&str_strip_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_lstrip), (mp_obj_t)&str_lstrip_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_rstrip), (mp_obj_t)&str_rstrip_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_format), (mp_obj_t)&str_format_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_replace), (mp_obj_t)&str_replace_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_count), (mp_obj_t)&str_count_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_partition), (mp_obj_t)&str_partition_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_rpartition), (mp_obj_t)&str_rpartition_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_lower), (mp_obj_t)&str_lower_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_upper), (mp_obj_t)&str_upper_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_isspace), (mp_obj_t)&str_isspace_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_isalpha), (mp_obj_t)&str_isalpha_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_isdigit), (mp_obj_t)&str_isdigit_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_isupper), (mp_obj_t)&str_isupper_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_islower), (mp_obj_t)&str_islower_obj },
};

STATIC MP_DEFINE_CONST_DICT(struni_locals_dict, struni_locals_dict_table);

const mp_obj_type_t mp_type_str = {
    { &mp_type_type },
    .name = MP_QSTR_str,
    .print = uni_print,
    .make_new = mp_obj_str_make_new,
    .unary_op = uni_unary_op,
    .binary_op = mp_obj_str_binary_op,
    .subscr = str_subscr,
    .getiter = mp_obj_new_str_iterator,
    .buffer_p = { .get_buffer = mp_obj_str_get_buffer },
    .locals_dict = (mp_obj_t)&struni_locals_dict,
};

/******************************************************************************/
/* str iterator                                                               */

typedef struct _mp_obj_str_it_t {
    mp_obj_base_t base;
    mp_obj_t str;
    mp_uint_t cur;
} mp_obj_str_it_t;

STATIC mp_obj_t str_it_iternext(mp_obj_t self_in) {
    mp_obj_str_it_t *self = self_in;
    GET_STR_DATA_LEN(self->str, str, len);
    if (self->cur < len) {
        const byte *cur = str + self->cur;
        const byte *end = utf8_next_char(str + self->cur);
        mp_obj_t o_out = mp_obj_new_str((const char*)cur, end - cur, true);
        self->cur += end - cur;
        return o_out;
    } else {
        return MP_OBJ_STOP_ITERATION;
    }
}

STATIC const mp_obj_type_t mp_type_str_it = {
    { &mp_type_type },
    .name = MP_QSTR_iterator,
    .getiter = mp_identity,
    .iternext = str_it_iternext,
};

mp_obj_t mp_obj_new_str_iterator(mp_obj_t str) {
    mp_obj_str_it_t *o = m_new_obj(mp_obj_str_it_t);
    o->base.type = &mp_type_str_it;
    o->str = str;
    o->cur = 0;
    return o;
}

#endif // MICROPY_PY_BUILTINS_STR_UNICODE
