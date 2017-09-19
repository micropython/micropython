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

#include <string.h>
#include <assert.h>

#include "py/nlr.h"
#include "py/unicode.h"
#include "py/objstr.h"
#include "py/objlist.h"
#include "py/runtime0.h"
#include "py/runtime.h"
#include "py/stackctrl.h"

STATIC mp_obj_t str_modulo_format(mp_obj_t pattern, size_t n_args, const mp_obj_t *args, mp_obj_t dict);

STATIC mp_obj_t mp_obj_new_bytes_iterator(mp_obj_t str, mp_obj_iter_buf_t *iter_buf);
STATIC NORETURN void bad_implicit_conversion(mp_obj_t self_in);

/******************************************************************************/
/* str                                                                        */

void mp_str_print_quoted(const mp_print_t *print, const byte *str_data, size_t str_len, bool is_bytes) {
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
    mp_printf(print, "%c", quote_char);
    for (const byte *s = str_data, *top = str_data + str_len; s < top; s++) {
        if (*s == quote_char) {
            mp_printf(print, "\\%c", quote_char);
        } else if (*s == '\\') {
            mp_print_str(print, "\\\\");
        } else if (*s >= 0x20 && *s != 0x7f && (!is_bytes || *s < 0x80)) {
            // In strings, anything which is not ascii control character
            // is printed as is, this includes characters in range 0x80-0xff
            // (which can be non-Latin letters, etc.)
            mp_printf(print, "%c", *s);
        } else if (*s == '\n') {
            mp_print_str(print, "\\n");
        } else if (*s == '\r') {
            mp_print_str(print, "\\r");
        } else if (*s == '\t') {
            mp_print_str(print, "\\t");
        } else {
            mp_printf(print, "\\x%02x", *s);
        }
    }
    mp_printf(print, "%c", quote_char);
}

#if MICROPY_PY_UJSON
void mp_str_print_json(const mp_print_t *print, const byte *str_data, size_t str_len) {
    // for JSON spec, see http://www.ietf.org/rfc/rfc4627.txt
    // if we are given a valid utf8-encoded string, we will print it in a JSON-conforming way
    mp_print_str(print, "\"");
    for (const byte *s = str_data, *top = str_data + str_len; s < top; s++) {
        if (*s == '"' || *s == '\\') {
            mp_printf(print, "\\%c", *s);
        } else if (*s >= 32) {
            // this will handle normal and utf-8 encoded chars
            mp_printf(print, "%c", *s);
        } else if (*s == '\n') {
            mp_print_str(print, "\\n");
        } else if (*s == '\r') {
            mp_print_str(print, "\\r");
        } else if (*s == '\t') {
            mp_print_str(print, "\\t");
        } else {
            // this will handle control chars
            mp_printf(print, "\\u%04x", *s);
        }
    }
    mp_print_str(print, "\"");
}
#endif

STATIC void str_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    GET_STR_DATA_LEN(self_in, str_data, str_len);
    #if MICROPY_PY_UJSON
    if (kind == PRINT_JSON) {
        mp_str_print_json(print, str_data, str_len);
        return;
    }
    #endif
    #if !MICROPY_PY_BUILTINS_STR_UNICODE
    bool is_bytes = MP_OBJ_IS_TYPE(self_in, &mp_type_bytes);
    #else
    bool is_bytes = true;
    #endif
    if (kind == PRINT_RAW || (!MICROPY_PY_BUILTINS_STR_UNICODE && kind == PRINT_STR && !is_bytes)) {
        mp_printf(print, "%.*s", str_len, str_data);
    } else {
        if (is_bytes) {
            mp_print_str(print, "b");
        }
        mp_str_print_quoted(print, str_data, str_len, is_bytes);
    }
}

mp_obj_t mp_obj_str_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
#if MICROPY_CPYTHON_COMPAT
    if (n_kw != 0) {
        mp_arg_error_unimpl_kw();
    }
#endif

    mp_arg_check_num(n_args, n_kw, 0, 3, false);

    switch (n_args) {
        case 0:
            return MP_OBJ_NEW_QSTR(MP_QSTR_);

        case 1: {
            vstr_t vstr;
            mp_print_t print;
            vstr_init_print(&vstr, 16, &print);
            mp_obj_print_helper(&print, args[0], PRINT_STR);
            return mp_obj_new_str_from_vstr(type, &vstr);
        }

        default: // 2 or 3 args
            // TODO: validate 2nd/3rd args
            if (MP_OBJ_IS_TYPE(args[0], &mp_type_bytes)) {
                GET_STR_DATA_LEN(args[0], str_data, str_len);
                GET_STR_HASH(args[0], str_hash);
                if (str_hash == 0) {
                    str_hash = qstr_compute_hash(str_data, str_len);
                }
                #if MICROPY_PY_BUILTINS_STR_UNICODE_CHECK
                if (!utf8_check(str_data, str_len)) {
                    mp_raise_msg(&mp_type_UnicodeError, NULL);
                }
                #endif
                mp_obj_str_t *o = MP_OBJ_TO_PTR(mp_obj_new_str_of_type(type, NULL, str_len));
                o->data = str_data;
                o->hash = str_hash;
                return MP_OBJ_FROM_PTR(o);
            } else {
                mp_buffer_info_t bufinfo;
                mp_get_buffer_raise(args[0], &bufinfo, MP_BUFFER_READ);
                #if MICROPY_PY_BUILTINS_STR_UNICODE_CHECK
                if (!utf8_check(bufinfo.buf, bufinfo.len)) {
                    mp_raise_msg(&mp_type_UnicodeError, NULL);
                }
                #endif
                return mp_obj_new_str(bufinfo.buf, bufinfo.len, false);
            }
    }
}

STATIC mp_obj_t bytes_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)type_in;

    #if MICROPY_CPYTHON_COMPAT
    if (n_kw != 0) {
        mp_arg_error_unimpl_kw();
    }
    #else
    (void)n_kw;
    #endif

    if (n_args == 0) {
        return mp_const_empty_bytes;
    }

    if (MP_OBJ_IS_STR(args[0])) {
        if (n_args < 2 || n_args > 3) {
            goto wrong_args;
        }
        GET_STR_DATA_LEN(args[0], str_data, str_len);
        GET_STR_HASH(args[0], str_hash);
        if (str_hash == 0) {
            str_hash = qstr_compute_hash(str_data, str_len);
        }
        mp_obj_str_t *o = MP_OBJ_TO_PTR(mp_obj_new_str_of_type(&mp_type_bytes, NULL, str_len));
        o->data = str_data;
        o->hash = str_hash;
        return MP_OBJ_FROM_PTR(o);
    }

    if (n_args > 1) {
        goto wrong_args;
    }

    if (MP_OBJ_IS_SMALL_INT(args[0])) {
        uint len = MP_OBJ_SMALL_INT_VALUE(args[0]);
        vstr_t vstr;
        vstr_init_len(&vstr, len);
        memset(vstr.buf, 0, len);
        return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
    }

    // check if argument has the buffer protocol
    mp_buffer_info_t bufinfo;
    if (mp_get_buffer(args[0], &bufinfo, MP_BUFFER_READ)) {
        return mp_obj_new_str_of_type(&mp_type_bytes, bufinfo.buf, bufinfo.len);
    }

    vstr_t vstr;
    // Try to create array of exact len if initializer len is known
    mp_obj_t len_in = mp_obj_len_maybe(args[0]);
    if (len_in == MP_OBJ_NULL) {
        vstr_init(&vstr, 16);
    } else {
        mp_int_t len = MP_OBJ_SMALL_INT_VALUE(len_in);
        vstr_init(&vstr, len);
    }

    mp_obj_iter_buf_t iter_buf;
    mp_obj_t iterable = mp_getiter(args[0], &iter_buf);
    mp_obj_t item;
    while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        mp_int_t val = mp_obj_get_int(item);
        #if MICROPY_FULL_CHECKS
        if (val < 0 || val > 255) {
            mp_raise_ValueError("bytes value out of range");
        }
        #endif
        vstr_add_byte(&vstr, val);
    }

    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);

wrong_args:
    mp_raise_TypeError("wrong number of arguments");
}

// like strstr but with specified length and allows \0 bytes
// TODO replace with something more efficient/standard
const byte *find_subbytes(const byte *haystack, size_t hlen, const byte *needle, size_t nlen, int direction) {
    if (hlen >= nlen) {
        size_t str_index, str_index_end;
        if (direction > 0) {
            str_index = 0;
            str_index_end = hlen - nlen;
        } else {
            str_index = hlen - nlen;
            str_index_end = 0;
        }
        for (;;) {
            if (memcmp(&haystack[str_index], needle, nlen) == 0) {
                //found
                return haystack + str_index;
            }
            if (str_index == str_index_end) {
                //not found
                break;
            }
            str_index += direction;
        }
    }
    return NULL;
}

// Note: this function is used to check if an object is a str or bytes, which
// works because both those types use it as their binary_op method.  Revisit
// MP_OBJ_IS_STR_OR_BYTES if this fact changes.
mp_obj_t mp_obj_str_binary_op(mp_binary_op_t op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    // check for modulo
    if (op == MP_BINARY_OP_MODULO) {
        mp_obj_t *args = &rhs_in;
        size_t n_args = 1;
        mp_obj_t dict = MP_OBJ_NULL;
        if (MP_OBJ_IS_TYPE(rhs_in, &mp_type_tuple)) {
            // TODO: Support tuple subclasses?
            mp_obj_tuple_get(rhs_in, &n_args, &args);
        } else if (MP_OBJ_IS_TYPE(rhs_in, &mp_type_dict)) {
            dict = rhs_in;
        }
        return str_modulo_format(lhs_in, n_args, args, dict);
    }

    // from now on we need lhs type and data, so extract them
    mp_obj_type_t *lhs_type = mp_obj_get_type(lhs_in);
    GET_STR_DATA_LEN(lhs_in, lhs_data, lhs_len);

    // check for multiply
    if (op == MP_BINARY_OP_MULTIPLY) {
        mp_int_t n;
        if (!mp_obj_get_int_maybe(rhs_in, &n)) {
            return MP_OBJ_NULL; // op not supported
        }
        if (n <= 0) {
            if (lhs_type == &mp_type_str) {
                return MP_OBJ_NEW_QSTR(MP_QSTR_); // empty str
            } else {
                return mp_const_empty_bytes;
            }
        }
        vstr_t vstr;
        vstr_init_len(&vstr, lhs_len * n);
        mp_seq_multiply(lhs_data, sizeof(*lhs_data), lhs_len, n, vstr.buf);
        return mp_obj_new_str_from_vstr(lhs_type, &vstr);
    }

    // From now on all operations allow:
    //    - str with str
    //    - bytes with bytes
    //    - bytes with bytearray
    //    - bytes with array.array
    // To do this efficiently we use the buffer protocol to extract the raw
    // data for the rhs, but only if the lhs is a bytes object.
    //
    // NOTE: CPython does not allow comparison between bytes ard array.array
    // (even if the array is of type 'b'), even though it allows addition of
    // such types.  We are not compatible with this (we do allow comparison
    // of bytes with anything that has the buffer protocol).  It would be
    // easy to "fix" this with a bit of extra logic below, but it costs code
    // size and execution time so we don't.

    const byte *rhs_data;
    size_t rhs_len;
    if (lhs_type == mp_obj_get_type(rhs_in)) {
        GET_STR_DATA_LEN(rhs_in, rhs_data_, rhs_len_);
        rhs_data = rhs_data_;
        rhs_len = rhs_len_;
    } else if (lhs_type == &mp_type_bytes) {
        mp_buffer_info_t bufinfo;
        if (!mp_get_buffer(rhs_in, &bufinfo, MP_BUFFER_READ)) {
            return MP_OBJ_NULL; // op not supported
        }
        rhs_data = bufinfo.buf;
        rhs_len = bufinfo.len;
    } else {
        // LHS is str and RHS has an incompatible type
        // (except if operation is EQUAL, but that's handled by mp_obj_equal)
        bad_implicit_conversion(rhs_in);
    }

    switch (op) {
        case MP_BINARY_OP_ADD:
        case MP_BINARY_OP_INPLACE_ADD: {
            if (lhs_len == 0 && mp_obj_get_type(rhs_in) == lhs_type) {
                return rhs_in;
            }
            if (rhs_len == 0) {
                return lhs_in;
            }

            vstr_t vstr;
            vstr_init_len(&vstr, lhs_len + rhs_len);
            memcpy(vstr.buf, lhs_data, lhs_len);
            memcpy(vstr.buf + lhs_len, rhs_data, rhs_len);
            return mp_obj_new_str_from_vstr(lhs_type, &vstr);
        }

        case MP_BINARY_OP_IN:
            /* NOTE `a in b` is `b.__contains__(a)` */
            return mp_obj_new_bool(find_subbytes(lhs_data, lhs_len, rhs_data, rhs_len, 1) != NULL);

        //case MP_BINARY_OP_NOT_EQUAL: // This is never passed here
        case MP_BINARY_OP_EQUAL: // This will be passed only for bytes, str is dealt with in mp_obj_equal()
        case MP_BINARY_OP_LESS:
        case MP_BINARY_OP_LESS_EQUAL:
        case MP_BINARY_OP_MORE:
        case MP_BINARY_OP_MORE_EQUAL:
            return mp_obj_new_bool(mp_seq_cmp_bytes(op, lhs_data, lhs_len, rhs_data, rhs_len));

        default:
            return MP_OBJ_NULL; // op not supported
    }
}

#if !MICROPY_PY_BUILTINS_STR_UNICODE
// objstrunicode defines own version
const byte *str_index_to_ptr(const mp_obj_type_t *type, const byte *self_data, size_t self_len,
                             mp_obj_t index, bool is_slice) {
    size_t index_val = mp_get_index(type, self_len, index, is_slice);
    return self_data + index_val;
}
#endif

// This is used for both bytes and 8-bit strings. This is not used for unicode strings.
STATIC mp_obj_t bytes_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value) {
    mp_obj_type_t *type = mp_obj_get_type(self_in);
    GET_STR_DATA_LEN(self_in, self_data, self_len);
    if (value == MP_OBJ_SENTINEL) {
        // load
#if MICROPY_PY_BUILTINS_SLICE
        if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
            mp_bound_slice_t slice;
            if (!mp_seq_get_fast_slice_indexes(self_len, index, &slice)) {
                mp_raise_NotImplementedError("only slices with step=1 (aka None) are supported");
            }
            return mp_obj_new_str_of_type(type, self_data + slice.start, slice.stop - slice.start);
        }
#endif
        size_t index_val = mp_get_index(type, self_len, index, false);
        // If we have unicode enabled the type will always be bytes, so take the short cut.
        if (MICROPY_PY_BUILTINS_STR_UNICODE || type == &mp_type_bytes) {
            return MP_OBJ_NEW_SMALL_INT(self_data[index_val]);
        } else {
            return mp_obj_new_str((char*)&self_data[index_val], 1, true);
        }
    } else {
        return MP_OBJ_NULL; // op not supported
    }
}

STATIC mp_obj_t str_join(mp_obj_t self_in, mp_obj_t arg) {
    mp_check_self(MP_OBJ_IS_STR_OR_BYTES(self_in));
    const mp_obj_type_t *self_type = mp_obj_get_type(self_in);

    // get separation string
    GET_STR_DATA_LEN(self_in, sep_str, sep_len);

    // process args
    size_t seq_len;
    mp_obj_t *seq_items;

    if (!MP_OBJ_IS_TYPE(arg, &mp_type_list) && !MP_OBJ_IS_TYPE(arg, &mp_type_tuple)) {
        // arg is not a list nor a tuple, try to convert it to a list
        // TODO: Try to optimize?
        arg = mp_type_list.make_new(&mp_type_list, 1, 0, &arg);
    }
    mp_obj_get_array(arg, &seq_len, &seq_items);

    // count required length
    size_t required_len = 0;
    for (size_t i = 0; i < seq_len; i++) {
        if (mp_obj_get_type(seq_items[i]) != self_type) {
            mp_raise_TypeError(
                "join expects a list of str/bytes objects consistent with self object");
        }
        if (i > 0) {
            required_len += sep_len;
        }
        GET_STR_LEN(seq_items[i], l);
        required_len += l;
    }

    // make joined string
    vstr_t vstr;
    vstr_init_len(&vstr, required_len);
    byte *data = (byte*)vstr.buf;
    for (size_t i = 0; i < seq_len; i++) {
        if (i > 0) {
            memcpy(data, sep_str, sep_len);
            data += sep_len;
        }
        GET_STR_DATA_LEN(seq_items[i], s, l);
        memcpy(data, s, l);
        data += l;
    }

    // return joined string
    return mp_obj_new_str_from_vstr(self_type, &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_2(str_join_obj, str_join);

mp_obj_t mp_obj_str_split(size_t n_args, const mp_obj_t *args) {
    const mp_obj_type_t *self_type = mp_obj_get_type(args[0]);
    mp_int_t splits = -1;
    mp_obj_t sep = mp_const_none;
    if (n_args > 1) {
        sep = args[1];
        if (n_args > 2) {
            splits = mp_obj_get_int(args[2]);
        }
    }

    mp_obj_t res = mp_obj_new_list(0, NULL);
    GET_STR_DATA_LEN(args[0], s, len);
    const byte *top = s + len;

    if (sep == mp_const_none) {
        // sep not given, so separate on whitespace

        // Initial whitespace is not counted as split, so we pre-do it
        while (s < top && unichar_isspace(*s)) s++;
        while (s < top && splits != 0) {
            const byte *start = s;
            while (s < top && !unichar_isspace(*s)) s++;
            mp_obj_list_append(res, mp_obj_new_str_of_type(self_type, start, s - start));
            if (s >= top) {
                break;
            }
            while (s < top && unichar_isspace(*s)) s++;
            if (splits > 0) {
                splits--;
            }
        }

        if (s < top) {
            mp_obj_list_append(res, mp_obj_new_str_of_type(self_type, s, top - s));
        }

    } else {
        // sep given
        if (mp_obj_get_type(sep) != self_type) {
            bad_implicit_conversion(sep);
        }

        size_t sep_len;
        const char *sep_str = mp_obj_str_get_data(sep, &sep_len);

        if (sep_len == 0) {
            mp_raise_ValueError("empty separator");
        }

        for (;;) {
            const byte *start = s;
            for (;;) {
                if (splits == 0 || s + sep_len > top) {
                    s = top;
                    break;
                } else if (memcmp(s, sep_str, sep_len) == 0) {
                    break;
                }
                s++;
            }
            mp_obj_list_append(res, mp_obj_new_str_of_type(self_type, start, s - start));
            if (s >= top) {
                break;
            }
            s += sep_len;
            if (splits > 0) {
                splits--;
            }
        }
    }

    return res;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_split_obj, 1, 3, mp_obj_str_split);

#if MICROPY_PY_BUILTINS_STR_SPLITLINES
STATIC mp_obj_t str_splitlines(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_keepends };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_keepends, MP_ARG_BOOL, {.u_bool = false} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mp_obj_type_t *self_type = mp_obj_get_type(pos_args[0]);
    mp_obj_t res = mp_obj_new_list(0, NULL);

    GET_STR_DATA_LEN(pos_args[0], s, len);
    const byte *top = s + len;

    while (s < top) {
        const byte *start = s;
        size_t match = 0;
        while (s < top) {
            if (*s == '\n') {
                match = 1;
                break;
            } else if (*s == '\r') {
                if (s[1] == '\n') {
                    match = 2;
                } else {
                    match = 1;
                }
                break;
            }
            s++;
        }
        size_t sub_len = s - start;
        if (args[ARG_keepends].u_bool) {
            sub_len += match;
        }
        mp_obj_list_append(res, mp_obj_new_str_of_type(self_type, start, sub_len));
        s += match;
    }

    return res;
}
MP_DEFINE_CONST_FUN_OBJ_KW(str_splitlines_obj, 1, str_splitlines);
#endif

STATIC mp_obj_t str_rsplit(size_t n_args, const mp_obj_t *args) {
    if (n_args < 3) {
        // If we don't have split limit, it doesn't matter from which side
        // we split.
        return mp_obj_str_split(n_args, args);
    }
    const mp_obj_type_t *self_type = mp_obj_get_type(args[0]);
    mp_obj_t sep = args[1];
    GET_STR_DATA_LEN(args[0], s, len);

    mp_int_t splits = mp_obj_get_int(args[2]);
    if (splits < 0) {
        // Negative limit means no limit, so delegate to split().
        return mp_obj_str_split(n_args, args);
    }

    mp_int_t org_splits = splits;
    // Preallocate list to the max expected # of elements, as we
    // will fill it from the end.
    mp_obj_list_t *res = MP_OBJ_TO_PTR(mp_obj_new_list(splits + 1, NULL));
    mp_int_t idx = splits;

    if (sep == mp_const_none) {
        mp_raise_NotImplementedError("rsplit(None,n)");
    } else {
        size_t sep_len;
        const char *sep_str = mp_obj_str_get_data(sep, &sep_len);

        if (sep_len == 0) {
            mp_raise_ValueError("empty separator");
        }

        const byte *beg = s;
        const byte *last = s + len;
        for (;;) {
            s = last - sep_len;
            for (;;) {
                if (splits == 0 || s < beg) {
                    break;
                } else if (memcmp(s, sep_str, sep_len) == 0) {
                    break;
                }
                s--;
            }
            if (s < beg || splits == 0) {
                res->items[idx] = mp_obj_new_str_of_type(self_type, beg, last - beg);
                break;
            }
            res->items[idx--] = mp_obj_new_str_of_type(self_type, s + sep_len, last - s - sep_len);
            last = s;
            if (splits > 0) {
                splits--;
            }
        }
        if (idx != 0) {
            // We split less parts than split limit, now go cleanup surplus
            size_t used = org_splits + 1 - idx;
            memmove(res->items, &res->items[idx], used * sizeof(mp_obj_t));
            mp_seq_clear(res->items, used, res->alloc, sizeof(*res->items));
            res->len = used;
        }
    }

    return MP_OBJ_FROM_PTR(res);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_rsplit_obj, 1, 3, str_rsplit);

STATIC mp_obj_t str_finder(size_t n_args, const mp_obj_t *args, int direction, bool is_index) {
    const mp_obj_type_t *self_type = mp_obj_get_type(args[0]);
    mp_check_self(MP_OBJ_IS_STR_OR_BYTES(args[0]));

    // check argument type
    if (mp_obj_get_type(args[1]) != self_type) {
        bad_implicit_conversion(args[1]);
    }

    GET_STR_DATA_LEN(args[0], haystack, haystack_len);
    GET_STR_DATA_LEN(args[1], needle, needle_len);

    const byte *start = haystack;
    const byte *end = haystack + haystack_len;
    if (n_args >= 3 && args[2] != mp_const_none) {
        start = str_index_to_ptr(self_type, haystack, haystack_len, args[2], true);
    }
    if (n_args >= 4 && args[3] != mp_const_none) {
        end = str_index_to_ptr(self_type, haystack, haystack_len, args[3], true);
    }

    const byte *p = find_subbytes(start, end - start, needle, needle_len, direction);
    if (p == NULL) {
        // not found
        if (is_index) {
            mp_raise_ValueError("substring not found");
        } else {
            return MP_OBJ_NEW_SMALL_INT(-1);
        }
    } else {
        // found
        #if MICROPY_PY_BUILTINS_STR_UNICODE
        if (self_type == &mp_type_str) {
            return MP_OBJ_NEW_SMALL_INT(utf8_ptr_to_index(haystack, p));
        }
        #endif
        return MP_OBJ_NEW_SMALL_INT(p - haystack);
    }
}

STATIC mp_obj_t str_find(size_t n_args, const mp_obj_t *args) {
    return str_finder(n_args, args, 1, false);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_find_obj, 2, 4, str_find);

STATIC mp_obj_t str_rfind(size_t n_args, const mp_obj_t *args) {
    return str_finder(n_args, args, -1, false);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_rfind_obj, 2, 4, str_rfind);

STATIC mp_obj_t str_index(size_t n_args, const mp_obj_t *args) {
    return str_finder(n_args, args, 1, true);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_index_obj, 2, 4, str_index);

STATIC mp_obj_t str_rindex(size_t n_args, const mp_obj_t *args) {
    return str_finder(n_args, args, -1, true);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_rindex_obj, 2, 4, str_rindex);

// TODO: (Much) more variety in args
STATIC mp_obj_t str_startswith(size_t n_args, const mp_obj_t *args) {
    const mp_obj_type_t *self_type = mp_obj_get_type(args[0]);
    GET_STR_DATA_LEN(args[0], str, str_len);
    size_t prefix_len;
    const char *prefix = mp_obj_str_get_data(args[1], &prefix_len);
    const byte *start = str;
    if (n_args > 2) {
        start = str_index_to_ptr(self_type, str, str_len, args[2], true);
    }
    if (prefix_len + (start - str) > str_len) {
        return mp_const_false;
    }
    return mp_obj_new_bool(memcmp(start, prefix, prefix_len) == 0);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_startswith_obj, 2, 3, str_startswith);

STATIC mp_obj_t str_endswith(size_t n_args, const mp_obj_t *args) {
    GET_STR_DATA_LEN(args[0], str, str_len);
    size_t suffix_len;
    const char *suffix = mp_obj_str_get_data(args[1], &suffix_len);
    if (n_args > 2) {
        mp_raise_NotImplementedError("start/end indices");
    }

    if (suffix_len > str_len) {
        return mp_const_false;
    }
    return mp_obj_new_bool(memcmp(str + (str_len - suffix_len), suffix, suffix_len) == 0);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_endswith_obj, 2, 3, str_endswith);

enum { LSTRIP, RSTRIP, STRIP };

STATIC mp_obj_t str_uni_strip(int type, size_t n_args, const mp_obj_t *args) {
    mp_check_self(MP_OBJ_IS_STR_OR_BYTES(args[0]));
    const mp_obj_type_t *self_type = mp_obj_get_type(args[0]);

    const byte *chars_to_del;
    uint chars_to_del_len;
    static const byte whitespace[] = " \t\n\r\v\f";

    if (n_args == 1) {
        chars_to_del = whitespace;
        chars_to_del_len = sizeof(whitespace) - 1;
    } else {
        if (mp_obj_get_type(args[1]) != self_type) {
            bad_implicit_conversion(args[1]);
        }
        GET_STR_DATA_LEN(args[1], s, l);
        chars_to_del = s;
        chars_to_del_len = l;
    }

    GET_STR_DATA_LEN(args[0], orig_str, orig_str_len);

    size_t first_good_char_pos = 0;
    bool first_good_char_pos_set = false;
    size_t last_good_char_pos = 0;
    size_t i = 0;
    int delta = 1;
    if (type == RSTRIP) {
        i = orig_str_len - 1;
        delta = -1;
    }
    for (size_t len = orig_str_len; len > 0; len--) {
        if (find_subbytes(chars_to_del, chars_to_del_len, &orig_str[i], 1, 1) == NULL) {
            if (!first_good_char_pos_set) {
                first_good_char_pos_set = true;
                first_good_char_pos = i;
                if (type == LSTRIP) {
                    last_good_char_pos = orig_str_len - 1;
                    break;
                } else if (type == RSTRIP) {
                    first_good_char_pos = 0;
                    last_good_char_pos = i;
                    break;
                }
            }
            last_good_char_pos = i;
        }
        i += delta;
    }

    if (!first_good_char_pos_set) {
        // string is all whitespace, return ''
        if (self_type == &mp_type_str) {
            return MP_OBJ_NEW_QSTR(MP_QSTR_);
        } else {
            return mp_const_empty_bytes;
        }
    }

    assert(last_good_char_pos >= first_good_char_pos);
    //+1 to accommodate the last character
    size_t stripped_len = last_good_char_pos - first_good_char_pos + 1;
    if (stripped_len == orig_str_len) {
        // If nothing was stripped, don't bother to dup original string
        // TODO: watch out for this case when we'll get to bytearray.strip()
        assert(first_good_char_pos == 0);
        return args[0];
    }
    return mp_obj_new_str_of_type(self_type, orig_str + first_good_char_pos, stripped_len);
}

STATIC mp_obj_t str_strip(size_t n_args, const mp_obj_t *args) {
    return str_uni_strip(STRIP, n_args, args);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_strip_obj, 1, 2, str_strip);

STATIC mp_obj_t str_lstrip(size_t n_args, const mp_obj_t *args) {
    return str_uni_strip(LSTRIP, n_args, args);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_lstrip_obj, 1, 2, str_lstrip);

STATIC mp_obj_t str_rstrip(size_t n_args, const mp_obj_t *args) {
    return str_uni_strip(RSTRIP, n_args, args);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_rstrip_obj, 1, 2, str_rstrip);

#if MICROPY_PY_BUILTINS_STR_CENTER
STATIC mp_obj_t str_center(mp_obj_t str_in, mp_obj_t width_in) {
    GET_STR_DATA_LEN(str_in, str, str_len);
    mp_uint_t width = mp_obj_get_int(width_in);
    if (str_len >= width) {
        return str_in;
    }

    vstr_t vstr;
    vstr_init_len(&vstr, width);
    memset(vstr.buf, ' ', width);
    int left = (width - str_len) / 2;
    memcpy(vstr.buf + left, str, str_len);
    return mp_obj_new_str_from_vstr(mp_obj_get_type(str_in), &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_2(str_center_obj, str_center);
#endif

// Takes an int arg, but only parses unsigned numbers, and only changes
// *num if at least one digit was parsed.
STATIC const char *str_to_int(const char *str, const char *top, int *num) {
    if (str < top && '0' <= *str && *str <= '9') {
        *num = 0;
        do {
            *num = *num * 10 + (*str - '0');
            str++;
        }
        while (str < top && '0' <= *str && *str <= '9');
    }
    return str;
}

STATIC bool isalignment(char ch) {
    return ch && strchr("<>=^", ch) != NULL;
}

STATIC bool istype(char ch) {
    return ch && strchr("bcdeEfFgGnosxX%", ch) != NULL;
}

STATIC bool arg_looks_integer(mp_obj_t arg) {
    return MP_OBJ_IS_TYPE(arg, &mp_type_bool) || MP_OBJ_IS_INT(arg);
}

STATIC bool arg_looks_numeric(mp_obj_t arg) {
    return arg_looks_integer(arg)
#if MICROPY_PY_BUILTINS_FLOAT
        || mp_obj_is_float(arg)
#endif
    ;
}

STATIC mp_obj_t arg_as_int(mp_obj_t arg) {
#if MICROPY_PY_BUILTINS_FLOAT
    if (mp_obj_is_float(arg)) {
        return mp_obj_new_int_from_float(mp_obj_float_get(arg));
    }
#endif
    return arg;
}

#if MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE
STATIC NORETURN void terse_str_format_value_error(void) {
    mp_raise_ValueError("bad format string");
}
#else
// define to nothing to improve coverage
#define terse_str_format_value_error()
#endif

STATIC vstr_t mp_obj_str_format_helper(const char *str, const char *top, int *arg_i, size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    vstr_t vstr;
    mp_print_t print;
    vstr_init_print(&vstr, 16, &print);

    for (; str < top; str++) {
        if (*str == '}') {
            str++;
            if (str < top && *str == '}') {
                vstr_add_byte(&vstr, '}');
                continue;
            }
            if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
                terse_str_format_value_error();
            } else {
                mp_raise_ValueError("single '}' encountered in format string");
            }
        }
        if (*str != '{') {
            vstr_add_byte(&vstr, *str);
            continue;
        }

        str++;
        if (str < top && *str == '{') {
            vstr_add_byte(&vstr, '{');
            continue;
        }

        // replacement_field ::=  "{" [field_name] ["!" conversion] [":" format_spec] "}"

        const char *field_name = NULL;
        const char *field_name_top = NULL;
        char conversion = '\0';
        const char *format_spec = NULL;

        if (str < top && *str != '}' && *str != '!' && *str != ':') {
            field_name = (const char *)str;
            while (str < top && *str != '}' && *str != '!' && *str != ':') {
                ++str;
            }
            field_name_top = (const char *)str;
        }

        // conversion ::=  "r" | "s"

        if (str < top && *str == '!') {
            str++;
            if (str < top && (*str == 'r' || *str == 's')) {
                conversion = *str++;
            } else {
                if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
                    terse_str_format_value_error();
                } else if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_NORMAL) {
                    mp_raise_ValueError("bad conversion specifier");
                } else {
                    if (str >= top) {
                        mp_raise_ValueError(
                            "end of format while looking for conversion specifier");
                    } else {
                        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                            "unknown conversion specifier %c", *str));
                    }
                }
            }
        }

        if (str < top && *str == ':') {
            str++;
            // {:} is the same as {}, which is the same as {!s}
            // This makes a difference when passing in a True or False
            // '{}'.format(True) returns 'True'
            // '{:d}'.format(True) returns '1'
            // So we treat {:} as {} and this later gets treated to be {!s}
            if (*str != '}') {
                format_spec = str;
                for (int nest = 1; str < top;) {
                    if (*str == '{') {
                        ++nest;
                    } else if (*str == '}') {
                        if (--nest == 0) {
                            break;
                        }
                    }
                    ++str;
                }
            }
        }
        if (str >= top) {
            if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
                terse_str_format_value_error();
            } else {
                mp_raise_ValueError("unmatched '{' in format");
            }
        }
        if (*str != '}') {
            if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
                terse_str_format_value_error();
            } else {
                mp_raise_ValueError("expected ':' after format specifier");
            }
        }

        mp_obj_t arg = mp_const_none;

        if (field_name) {
            int index = 0;
            if (MP_LIKELY(unichar_isdigit(*field_name))) {
                if (*arg_i > 0) {
                    if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
                        terse_str_format_value_error();
                    } else {
                        mp_raise_ValueError(
                            "can't switch from automatic field numbering to manual field specification");
                    }
                }
                field_name = str_to_int(field_name, field_name_top, &index);
                if ((uint)index >= n_args - 1) {
                    mp_raise_msg(&mp_type_IndexError, "tuple index out of range");
                }
                arg = args[index + 1];
                *arg_i = -1;
            } else {
                const char *lookup;
                for (lookup = field_name; lookup < field_name_top && *lookup != '.' && *lookup != '['; lookup++);
                mp_obj_t field_q = mp_obj_new_str(field_name, lookup - field_name, true/*?*/);
                field_name = lookup;
                mp_map_elem_t *key_elem = mp_map_lookup(kwargs, field_q, MP_MAP_LOOKUP);
                if (key_elem == NULL) {
                    nlr_raise(mp_obj_new_exception_arg1(&mp_type_KeyError, field_q));
                }
                arg = key_elem->value;
            }
            if (field_name < field_name_top) {
                mp_raise_NotImplementedError("attributes not supported yet");
            }
        } else {
            if (*arg_i < 0) {
                if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
                    terse_str_format_value_error();
                } else {
                    mp_raise_ValueError(
                        "can't switch from manual field specification to automatic field numbering");
                }
            }
            if ((uint)*arg_i >= n_args - 1) {
                mp_raise_msg(&mp_type_IndexError, "tuple index out of range");
            }
            arg = args[(*arg_i) + 1];
            (*arg_i)++;
        }
        if (!format_spec && !conversion) {
            conversion = 's';
        }
        if (conversion) {
            mp_print_kind_t print_kind;
            if (conversion == 's') {
                print_kind = PRINT_STR;
            } else {
                assert(conversion == 'r');
                print_kind = PRINT_REPR;
            }
            vstr_t arg_vstr;
            mp_print_t arg_print;
            vstr_init_print(&arg_vstr, 16, &arg_print);
            mp_obj_print_helper(&arg_print, arg, print_kind);
            arg = mp_obj_new_str_from_vstr(&mp_type_str, &arg_vstr);
        }

        char fill = '\0';
        char align = '\0';
        int width = -1;
        int precision = -1;
        char type = '\0';
        int flags = 0;

        if (format_spec) {
            // The format specifier (from http://docs.python.org/2/library/string.html#formatspec)
            //
            // [[fill]align][sign][#][0][width][,][.precision][type]
            // fill        ::=  <any character>
            // align       ::=  "<" | ">" | "=" | "^"
            // sign        ::=  "+" | "-" | " "
            // width       ::=  integer
            // precision   ::=  integer
            // type        ::=  "b" | "c" | "d" | "e" | "E" | "f" | "F" | "g" | "G" | "n" | "o" | "s" | "x" | "X" | "%"

            // recursively call the formatter to format any nested specifiers
            MP_STACK_CHECK();
            vstr_t format_spec_vstr = mp_obj_str_format_helper(format_spec, str, arg_i, n_args, args, kwargs);
            const char *s = vstr_null_terminated_str(&format_spec_vstr);
            const char *stop = s + format_spec_vstr.len;
            if (isalignment(*s)) {
                align = *s++;
            } else if (*s && isalignment(s[1])) {
                fill = *s++;
                align = *s++;
            }
            if (*s == '+' || *s == '-' || *s == ' ') {
                if (*s == '+') {
                    flags |= PF_FLAG_SHOW_SIGN;
                } else if (*s == ' ') {
                    flags |= PF_FLAG_SPACE_SIGN;
                }
                s++;
            }
            if (*s == '#') {
                flags |= PF_FLAG_SHOW_PREFIX;
                s++;
            }
            if (*s == '0') {
                if (!align) {
                    align = '=';
                }
                if (!fill) {
                    fill = '0';
                }
            }
            s = str_to_int(s, stop, &width);
            if (*s == ',') {
                flags |= PF_FLAG_SHOW_COMMA;
                s++;
            }
            if (*s == '.') {
                s++;
                s = str_to_int(s, stop, &precision);
            }
            if (istype(*s)) {
                type = *s++;
            }
            if (*s) {
                if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
                    terse_str_format_value_error();
                } else {
                    mp_raise_ValueError("invalid format specifier");
                }
            }
            vstr_clear(&format_spec_vstr);
        }
        if (!align) {
            if (arg_looks_numeric(arg)) {
                align = '>';
            } else {
                align = '<';
            }
        }
        if (!fill) {
            fill = ' ';
        }

        if (flags & (PF_FLAG_SHOW_SIGN | PF_FLAG_SPACE_SIGN)) {
            if (type == 's') {
                if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
                    terse_str_format_value_error();
                } else {
                    mp_raise_ValueError("sign not allowed in string format specifier");
                }
            }
            if (type == 'c') {
                if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
                    terse_str_format_value_error();
                } else {
                    mp_raise_ValueError(
                        "sign not allowed with integer format specifier 'c'");
                }
            }
        }

        switch (align) {
            case '<': flags |= PF_FLAG_LEFT_ADJUST;     break;
            case '=': flags |= PF_FLAG_PAD_AFTER_SIGN;  break;
            case '^': flags |= PF_FLAG_CENTER_ADJUST;   break;
        }

        if (arg_looks_integer(arg)) {
            switch (type) {
                case 'b':
                    mp_print_mp_int(&print, arg, 2, 'a', flags, fill, width, 0);
                    continue;

                case 'c':
                {
                    char ch = mp_obj_get_int(arg);
                    mp_print_strn(&print, &ch, 1, flags, fill, width);
                    continue;
                }

                case '\0':  // No explicit format type implies 'd'
                case 'n':   // I don't think we support locales in uPy so use 'd'
                case 'd':
                    mp_print_mp_int(&print, arg, 10, 'a', flags, fill, width, 0);
                    continue;

                case 'o':
                    if (flags & PF_FLAG_SHOW_PREFIX) {
                        flags |= PF_FLAG_SHOW_OCTAL_LETTER;
                    }

                    mp_print_mp_int(&print, arg, 8, 'a', flags, fill, width, 0);
                    continue;

                case 'X':
                case 'x':
                    mp_print_mp_int(&print, arg, 16, type - ('X' - 'A'), flags, fill, width, 0);
                    continue;

                case 'e':
                case 'E':
                case 'f':
                case 'F':
                case 'g':
                case 'G':
                case '%':
                    // The floating point formatters all work with anything that
                    // looks like an integer
                    break;

                default:
                    if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
                        terse_str_format_value_error();
                    } else {
                        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                            "unknown format code '%c' for object of type '%s'",
                            type, mp_obj_get_type_str(arg)));
                    }
            }
        }

        // NOTE: no else here. We need the e, f, g etc formats for integer
        //       arguments (from above if) to take this if.
        if (arg_looks_numeric(arg)) {
            if (!type) {

                // Even though the docs say that an unspecified type is the same
                // as 'g', there is one subtle difference, when the exponent
                // is one less than the precision.
                //
                // '{:10.1}'.format(0.0) ==> '0e+00'
                // '{:10.1g}'.format(0.0) ==> '0'
                //
                // TODO: Figure out how to deal with this.
                //
                // A proper solution would involve adding a special flag
                // or something to format_float, and create a format_double
                // to deal with doubles. In order to fix this when using
                // sprintf, we'd need to use the e format and tweak the
                // returned result to strip trailing zeros like the g format
                // does.
                //
                // {:10.3} and {:10.2e} with 1.23e2 both produce 1.23e+02
                // but with 1.e2 you get 1e+02 and 1.00e+02
                //
                // Stripping the trailing 0's (like g) does would make the
                // e format give us the right format.
                //
                // CPython sources say:
                //   Omitted type specifier.  Behaves in the same way as repr(x)
                //   and str(x) if no precision is given, else like 'g', but with
                //   at least one digit after the decimal point. */

                type = 'g';
            }
            if (type == 'n') {
                type = 'g';
            }

            switch (type) {
#if MICROPY_PY_BUILTINS_FLOAT
                case 'e':
                case 'E':
                case 'f':
                case 'F':
                case 'g':
                case 'G':
                    mp_print_float(&print, mp_obj_get_float(arg), type, flags, fill, width, precision);
                    break;

                case '%':
                    flags |= PF_FLAG_ADD_PERCENT;
                    #if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
                    #define F100 100.0F
                    #else
                    #define F100 100.0
                    #endif
                    mp_print_float(&print, mp_obj_get_float(arg) * F100, 'f', flags, fill, width, precision);
                    #undef F100
                    break;
#endif

                default:
                    if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
                        terse_str_format_value_error();
                    } else {
                        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                            "unknown format code '%c' for object of type 'float'",
                            type, mp_obj_get_type_str(arg)));
                    }
            }
        } else {
            // arg doesn't look like a number

            if (align == '=') {
                if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
                    terse_str_format_value_error();
                } else {
                    mp_raise_ValueError(
                        "'=' alignment not allowed in string format specifier");
                }
            }

            switch (type) {
                case '\0': // no explicit format type implies 's'
                case 's': {
                    size_t slen;
                    const char *s = mp_obj_str_get_data(arg, &slen);
                    if (precision < 0) {
                        precision = slen;
                    }
                    if (slen > (size_t)precision) {
                        slen = precision;
                    }
                    mp_print_strn(&print, s, slen, flags, fill, width);
                    break;
                }

                default:
                    if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
                        terse_str_format_value_error();
                    } else {
                        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                            "unknown format code '%c' for object of type 'str'",
                            type, mp_obj_get_type_str(arg)));
                    }
            }
        }
    }

    return vstr;
}

mp_obj_t mp_obj_str_format(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    mp_check_self(MP_OBJ_IS_STR_OR_BYTES(args[0]));

    GET_STR_DATA_LEN(args[0], str, len);
    int arg_i = 0;
    vstr_t vstr = mp_obj_str_format_helper((const char*)str, (const char*)str + len, &arg_i, n_args, args, kwargs);
    return mp_obj_new_str_from_vstr(&mp_type_str, &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_KW(str_format_obj, 1, mp_obj_str_format);

STATIC mp_obj_t str_modulo_format(mp_obj_t pattern, size_t n_args, const mp_obj_t *args, mp_obj_t dict) {
    mp_check_self(MP_OBJ_IS_STR_OR_BYTES(pattern));

    GET_STR_DATA_LEN(pattern, str, len);
    const byte *start_str = str;
    bool is_bytes = MP_OBJ_IS_TYPE(pattern, &mp_type_bytes);
    size_t arg_i = 0;
    vstr_t vstr;
    mp_print_t print;
    vstr_init_print(&vstr, 16, &print);

    for (const byte *top = str + len; str < top; str++) {
        mp_obj_t arg = MP_OBJ_NULL;
        if (*str != '%') {
            vstr_add_byte(&vstr, *str);
            continue;
        }
        if (++str >= top) {
            goto incomplete_format;
        }
        if (*str == '%') {
            vstr_add_byte(&vstr, '%');
            continue;
        }

        // Dictionary value lookup
        if (*str == '(') {
            if (dict == MP_OBJ_NULL) {
                mp_raise_TypeError("format requires a dict");
            }
            arg_i = 1; // we used up the single dict argument
            const byte *key = ++str;
            while (*str != ')') {
                if (str >= top) {
                    if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
                        terse_str_format_value_error();
                    } else {
                        mp_raise_ValueError("incomplete format key");
                    }
                }
                ++str;
            }
            mp_obj_t k_obj = mp_obj_new_str((const char*)key, str - key, true);
            arg = mp_obj_dict_get(dict, k_obj);
            str++;
        }

        int flags = 0;
        char fill = ' ';
        int alt = 0;
        while (str < top) {
            if (*str == '-')      flags |= PF_FLAG_LEFT_ADJUST;
            else if (*str == '+') flags |= PF_FLAG_SHOW_SIGN;
            else if (*str == ' ') flags |= PF_FLAG_SPACE_SIGN;
            else if (*str == '#') alt = PF_FLAG_SHOW_PREFIX;
            else if (*str == '0') {
                flags |= PF_FLAG_PAD_AFTER_SIGN;
                fill = '0';
            } else break;
            str++;
        }
        // parse width, if it exists
        int width = 0;
        if (str < top) {
            if (*str == '*') {
                if (arg_i >= n_args) {
                    goto not_enough_args;
                }
                width = mp_obj_get_int(args[arg_i++]);
                str++;
            } else {
                str = (const byte*)str_to_int((const char*)str, (const char*)top, &width);
            }
        }
        int prec = -1;
        if (str < top && *str == '.') {
            if (++str < top) {
                if (*str == '*') {
                    if (arg_i >= n_args) {
                        goto not_enough_args;
                    }
                    prec = mp_obj_get_int(args[arg_i++]);
                    str++;
                } else {
                    prec = 0;
                    str = (const byte*)str_to_int((const char*)str, (const char*)top, &prec);
                }
            }
        }

        if (str >= top) {
incomplete_format:
            if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
                terse_str_format_value_error();
            } else {
                mp_raise_ValueError("incomplete format");
            }
        }

        // Tuple value lookup
        if (arg == MP_OBJ_NULL) {
            if (arg_i >= n_args) {
not_enough_args:
                mp_raise_TypeError("not enough arguments for format string");
            }
            arg = args[arg_i++];
        }
        switch (*str) {
            case 'c':
                if (MP_OBJ_IS_STR(arg)) {
                    size_t slen;
                    const char *s = mp_obj_str_get_data(arg, &slen);
                    if (slen != 1) {
                        mp_raise_TypeError("%%c requires int or char");
                    }
                    mp_print_strn(&print, s, 1, flags, ' ', width);
                } else if (arg_looks_integer(arg)) {
                    char ch = mp_obj_get_int(arg);
                    mp_print_strn(&print, &ch, 1, flags, ' ', width);
                } else {
                    mp_raise_TypeError("integer required");
                }
                break;

            case 'd':
            case 'i':
            case 'u':
                mp_print_mp_int(&print, arg_as_int(arg), 10, 'a', flags, fill, width, prec);
                break;

#if MICROPY_PY_BUILTINS_FLOAT
            case 'e':
            case 'E':
            case 'f':
            case 'F':
            case 'g':
            case 'G':
                mp_print_float(&print, mp_obj_get_float(arg), *str, flags, fill, width, prec);
                break;
#endif

            case 'o':
                if (alt) {
                    flags |= (PF_FLAG_SHOW_PREFIX | PF_FLAG_SHOW_OCTAL_LETTER);
                }
                mp_print_mp_int(&print, arg, 8, 'a', flags, fill, width, prec);
                break;

            case 'r':
            case 's':
            {
                vstr_t arg_vstr;
                mp_print_t arg_print;
                vstr_init_print(&arg_vstr, 16, &arg_print);
                mp_print_kind_t print_kind = (*str == 'r' ? PRINT_REPR : PRINT_STR);
                if (print_kind == PRINT_STR && is_bytes && MP_OBJ_IS_TYPE(arg, &mp_type_bytes)) {
                    // If we have something like b"%s" % b"1", bytes arg should be
                    // printed undecorated.
                    print_kind = PRINT_RAW;
                }
                mp_obj_print_helper(&arg_print, arg, print_kind);
                uint vlen = arg_vstr.len;
                if (prec < 0) {
                    prec = vlen;
                }
                if (vlen > (uint)prec) {
                    vlen = prec;
                }
                mp_print_strn(&print, arg_vstr.buf, vlen, flags, ' ', width);
                vstr_clear(&arg_vstr);
                break;
            }

            case 'X':
            case 'x':
                mp_print_mp_int(&print, arg, 16, *str - ('X' - 'A'), flags | alt, fill, width, prec);
                break;

            default:
                if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
                    terse_str_format_value_error();
                } else {
                    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                        "unsupported format character '%c' (0x%x) at index %d",
                        *str, *str, str - start_str));
                }
        }
    }

    if (arg_i != n_args) {
        mp_raise_TypeError("not all arguments converted during string formatting");
    }

    return mp_obj_new_str_from_vstr(is_bytes ? &mp_type_bytes : &mp_type_str, &vstr);
}

// The implementation is optimized, returning the original string if there's
// nothing to replace.
STATIC mp_obj_t str_replace(size_t n_args, const mp_obj_t *args) {
    mp_check_self(MP_OBJ_IS_STR_OR_BYTES(args[0]));

    mp_int_t max_rep = -1;
    if (n_args == 4) {
        max_rep = mp_obj_get_int(args[3]);
        if (max_rep == 0) {
            return args[0];
        } else if (max_rep < 0) {
            max_rep = -1;
        }
    }

    // if max_rep is still -1 by this point we will need to do all possible replacements

    // check argument types

    const mp_obj_type_t *self_type = mp_obj_get_type(args[0]);

    if (mp_obj_get_type(args[1]) != self_type) {
        bad_implicit_conversion(args[1]);
    }

    if (mp_obj_get_type(args[2]) != self_type) {
        bad_implicit_conversion(args[2]);
    }

    // extract string data

    GET_STR_DATA_LEN(args[0], str, str_len);
    GET_STR_DATA_LEN(args[1], old, old_len);
    GET_STR_DATA_LEN(args[2], new, new_len);

    // old won't exist in str if it's longer, so nothing to replace
    if (old_len > str_len) {
        return args[0];
    }

    // data for the replaced string
    byte *data = NULL;
    vstr_t vstr;

    // do 2 passes over the string:
    //   first pass computes the required length of the replaced string
    //   second pass does the replacements
    for (;;) {
        size_t replaced_str_index = 0;
        size_t num_replacements_done = 0;
        const byte *old_occurrence;
        const byte *offset_ptr = str;
        size_t str_len_remain = str_len;
        if (old_len == 0) {
            // if old_str is empty, copy new_str to start of replaced string
            // copy the replacement string
            if (data != NULL) {
                memcpy(data, new, new_len);
            }
            replaced_str_index += new_len;
            num_replacements_done++;
        }
        while (num_replacements_done != (size_t)max_rep && str_len_remain > 0 && (old_occurrence = find_subbytes(offset_ptr, str_len_remain, old, old_len, 1)) != NULL) {
            if (old_len == 0) {
                old_occurrence += 1;
            }
            // copy from just after end of last occurrence of to-be-replaced string to right before start of next occurrence
            if (data != NULL) {
                memcpy(data + replaced_str_index, offset_ptr, old_occurrence - offset_ptr);
            }
            replaced_str_index += old_occurrence - offset_ptr;
            // copy the replacement string
            if (data != NULL) {
                memcpy(data + replaced_str_index, new, new_len);
            }
            replaced_str_index += new_len;
            offset_ptr = old_occurrence + old_len;
            str_len_remain = str + str_len - offset_ptr;
            num_replacements_done++;
        }

        // copy from just after end of last occurrence of to-be-replaced string to end of old string
        if (data != NULL) {
            memcpy(data + replaced_str_index, offset_ptr, str_len_remain);
        }
        replaced_str_index += str_len_remain;

        if (data == NULL) {
            // first pass
            if (num_replacements_done == 0) {
                // no substr found, return original string
                return args[0];
            } else {
                // substr found, allocate new string
                vstr_init_len(&vstr, replaced_str_index);
                data = (byte*)vstr.buf;
                assert(data != NULL);
            }
        } else {
            // second pass, we are done
            break;
        }
    }

    return mp_obj_new_str_from_vstr(self_type, &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_replace_obj, 3, 4, str_replace);

STATIC mp_obj_t str_count(size_t n_args, const mp_obj_t *args) {
    const mp_obj_type_t *self_type = mp_obj_get_type(args[0]);
    mp_check_self(MP_OBJ_IS_STR_OR_BYTES(args[0]));

    // check argument type
    if (mp_obj_get_type(args[1]) != self_type) {
        bad_implicit_conversion(args[1]);
    }

    GET_STR_DATA_LEN(args[0], haystack, haystack_len);
    GET_STR_DATA_LEN(args[1], needle, needle_len);

    const byte *start = haystack;
    const byte *end = haystack + haystack_len;
    if (n_args >= 3 && args[2] != mp_const_none) {
        start = str_index_to_ptr(self_type, haystack, haystack_len, args[2], true);
    }
    if (n_args >= 4 && args[3] != mp_const_none) {
        end = str_index_to_ptr(self_type, haystack, haystack_len, args[3], true);
    }

    // if needle_len is zero then we count each gap between characters as an occurrence
    if (needle_len == 0) {
        return MP_OBJ_NEW_SMALL_INT(unichar_charlen((const char*)start, end - start) + 1);
    }

    // count the occurrences
    mp_int_t num_occurrences = 0;
    for (const byte *haystack_ptr = start; haystack_ptr + needle_len <= end;) {
        if (memcmp(haystack_ptr, needle, needle_len) == 0) {
            num_occurrences++;
            haystack_ptr += needle_len;
        } else {
            haystack_ptr = utf8_next_char(haystack_ptr);
        }
    }

    return MP_OBJ_NEW_SMALL_INT(num_occurrences);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_count_obj, 2, 4, str_count);

#if MICROPY_PY_BUILTINS_STR_PARTITION
STATIC mp_obj_t str_partitioner(mp_obj_t self_in, mp_obj_t arg, int direction) {
    mp_check_self(MP_OBJ_IS_STR_OR_BYTES(self_in));
    mp_obj_type_t *self_type = mp_obj_get_type(self_in);
    if (self_type != mp_obj_get_type(arg)) {
        bad_implicit_conversion(arg);
    }

    GET_STR_DATA_LEN(self_in, str, str_len);
    GET_STR_DATA_LEN(arg, sep, sep_len);

    if (sep_len == 0) {
        mp_raise_ValueError("empty separator");
    }

    mp_obj_t result[3];
    if (self_type == &mp_type_str) {
        result[0] = MP_OBJ_NEW_QSTR(MP_QSTR_);
        result[1] = MP_OBJ_NEW_QSTR(MP_QSTR_);
        result[2] = MP_OBJ_NEW_QSTR(MP_QSTR_);
    } else {
        result[0] = mp_const_empty_bytes;
        result[1] = mp_const_empty_bytes;
        result[2] = mp_const_empty_bytes;
    }

    if (direction > 0) {
        result[0] = self_in;
    } else {
        result[2] = self_in;
    }

    const byte *position_ptr = find_subbytes(str, str_len, sep, sep_len, direction);
    if (position_ptr != NULL) {
        size_t position = position_ptr - str;
        result[0] = mp_obj_new_str_of_type(self_type, str, position);
        result[1] = arg;
        result[2] = mp_obj_new_str_of_type(self_type, str + position + sep_len, str_len - position - sep_len);
    }

    return mp_obj_new_tuple(3, result);
}

STATIC mp_obj_t str_partition(mp_obj_t self_in, mp_obj_t arg) {
    return str_partitioner(self_in, arg, 1);
}
MP_DEFINE_CONST_FUN_OBJ_2(str_partition_obj, str_partition);

STATIC mp_obj_t str_rpartition(mp_obj_t self_in, mp_obj_t arg) {
    return str_partitioner(self_in, arg, -1);
}
MP_DEFINE_CONST_FUN_OBJ_2(str_rpartition_obj, str_rpartition);
#endif

// Supposedly not too critical operations, so optimize for code size
STATIC mp_obj_t str_caseconv(unichar (*op)(unichar), mp_obj_t self_in) {
    GET_STR_DATA_LEN(self_in, self_data, self_len);
    vstr_t vstr;
    vstr_init_len(&vstr, self_len);
    byte *data = (byte*)vstr.buf;
    for (size_t i = 0; i < self_len; i++) {
        *data++ = op(*self_data++);
    }
    return mp_obj_new_str_from_vstr(mp_obj_get_type(self_in), &vstr);
}

STATIC mp_obj_t str_lower(mp_obj_t self_in) {
    return str_caseconv(unichar_tolower, self_in);
}
MP_DEFINE_CONST_FUN_OBJ_1(str_lower_obj, str_lower);

STATIC mp_obj_t str_upper(mp_obj_t self_in) {
    return str_caseconv(unichar_toupper, self_in);
}
MP_DEFINE_CONST_FUN_OBJ_1(str_upper_obj, str_upper);

STATIC mp_obj_t str_uni_istype(bool (*f)(unichar), mp_obj_t self_in) {
    GET_STR_DATA_LEN(self_in, self_data, self_len);

    if (self_len == 0) {
        return mp_const_false; // default to False for empty str
    }

    if (f != unichar_isupper && f != unichar_islower) {
        for (size_t i = 0; i < self_len; i++) {
            if (!f(*self_data++)) {
                return mp_const_false;
            }
        }
    } else {
        bool contains_alpha = false;

        for (size_t i = 0; i < self_len; i++) { // only check alphanumeric characters
            if (unichar_isalpha(*self_data++)) {
                contains_alpha = true;
                if (!f(*(self_data - 1))) { // -1 because we already incremented above
                    return mp_const_false;
                }
            }
        }

        if (!contains_alpha) {
            return mp_const_false;
        }
    }

    return mp_const_true;
}

STATIC mp_obj_t str_isspace(mp_obj_t self_in) {
    return str_uni_istype(unichar_isspace, self_in);
}
MP_DEFINE_CONST_FUN_OBJ_1(str_isspace_obj, str_isspace);

STATIC mp_obj_t str_isalpha(mp_obj_t self_in) {
    return str_uni_istype(unichar_isalpha, self_in);
}
MP_DEFINE_CONST_FUN_OBJ_1(str_isalpha_obj, str_isalpha);

STATIC mp_obj_t str_isdigit(mp_obj_t self_in) {
    return str_uni_istype(unichar_isdigit, self_in);
}
MP_DEFINE_CONST_FUN_OBJ_1(str_isdigit_obj, str_isdigit);

STATIC mp_obj_t str_isupper(mp_obj_t self_in) {
    return str_uni_istype(unichar_isupper, self_in);
}
MP_DEFINE_CONST_FUN_OBJ_1(str_isupper_obj, str_isupper);

STATIC mp_obj_t str_islower(mp_obj_t self_in) {
    return str_uni_istype(unichar_islower, self_in);
}
MP_DEFINE_CONST_FUN_OBJ_1(str_islower_obj, str_islower);

#if MICROPY_CPYTHON_COMPAT
// These methods are superfluous in the presence of str() and bytes()
// constructors.
// TODO: should accept kwargs too
STATIC mp_obj_t bytes_decode(size_t n_args, const mp_obj_t *args) {
    mp_obj_t new_args[2];
    if (n_args == 1) {
        new_args[0] = args[0];
        new_args[1] = MP_OBJ_NEW_QSTR(MP_QSTR_utf_hyphen_8);
        args = new_args;
        n_args++;
    }
    return mp_obj_str_make_new(&mp_type_str, n_args, 0, args);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(bytes_decode_obj, 1, 3, bytes_decode);

// TODO: should accept kwargs too
STATIC mp_obj_t str_encode(size_t n_args, const mp_obj_t *args) {
    mp_obj_t new_args[2];
    if (n_args == 1) {
        new_args[0] = args[0];
        new_args[1] = MP_OBJ_NEW_QSTR(MP_QSTR_utf_hyphen_8);
        args = new_args;
        n_args++;
    }
    return bytes_make_new(NULL, n_args, 0, args);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_encode_obj, 1, 3, str_encode);
#endif

mp_int_t mp_obj_str_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    if (flags == MP_BUFFER_READ) {
        GET_STR_DATA_LEN(self_in, str_data, str_len);
        bufinfo->buf = (void*)str_data;
        bufinfo->len = str_len;
        bufinfo->typecode = 'B'; // bytes should be unsigned, so should unicode byte-access
        return 0;
    } else {
        // can't write to a string
        bufinfo->buf = NULL;
        bufinfo->len = 0;
        bufinfo->typecode = -1;
        return 1;
    }
}

STATIC const mp_rom_map_elem_t str8_locals_dict_table[] = {
#if MICROPY_CPYTHON_COMPAT
    { MP_ROM_QSTR(MP_QSTR_decode), MP_ROM_PTR(&bytes_decode_obj) },
    #if !MICROPY_PY_BUILTINS_STR_UNICODE
    // If we have separate unicode type, then here we have methods only
    // for bytes type, and it should not have encode() methods. Otherwise,
    // we have non-compliant-but-practical bytestring type, which shares
    // method table with bytes, so they both have encode() and decode()
    // methods (which should do type checking at runtime).
    { MP_ROM_QSTR(MP_QSTR_encode), MP_ROM_PTR(&str_encode_obj) },
    #endif
#endif
    { MP_ROM_QSTR(MP_QSTR_find), MP_ROM_PTR(&str_find_obj) },
    { MP_ROM_QSTR(MP_QSTR_rfind), MP_ROM_PTR(&str_rfind_obj) },
    { MP_ROM_QSTR(MP_QSTR_index), MP_ROM_PTR(&str_index_obj) },
    { MP_ROM_QSTR(MP_QSTR_rindex), MP_ROM_PTR(&str_rindex_obj) },
    { MP_ROM_QSTR(MP_QSTR_join), MP_ROM_PTR(&str_join_obj) },
    { MP_ROM_QSTR(MP_QSTR_split), MP_ROM_PTR(&str_split_obj) },
    #if MICROPY_PY_BUILTINS_STR_SPLITLINES
    { MP_ROM_QSTR(MP_QSTR_splitlines), MP_ROM_PTR(&str_splitlines_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_rsplit), MP_ROM_PTR(&str_rsplit_obj) },
    { MP_ROM_QSTR(MP_QSTR_startswith), MP_ROM_PTR(&str_startswith_obj) },
    { MP_ROM_QSTR(MP_QSTR_endswith), MP_ROM_PTR(&str_endswith_obj) },
    { MP_ROM_QSTR(MP_QSTR_strip), MP_ROM_PTR(&str_strip_obj) },
    { MP_ROM_QSTR(MP_QSTR_lstrip), MP_ROM_PTR(&str_lstrip_obj) },
    { MP_ROM_QSTR(MP_QSTR_rstrip), MP_ROM_PTR(&str_rstrip_obj) },
    { MP_ROM_QSTR(MP_QSTR_format), MP_ROM_PTR(&str_format_obj) },
    { MP_ROM_QSTR(MP_QSTR_replace), MP_ROM_PTR(&str_replace_obj) },
    { MP_ROM_QSTR(MP_QSTR_count), MP_ROM_PTR(&str_count_obj) },
    #if MICROPY_PY_BUILTINS_STR_PARTITION
    { MP_ROM_QSTR(MP_QSTR_partition), MP_ROM_PTR(&str_partition_obj) },
    { MP_ROM_QSTR(MP_QSTR_rpartition), MP_ROM_PTR(&str_rpartition_obj) },
    #endif
    #if MICROPY_PY_BUILTINS_STR_CENTER
    { MP_ROM_QSTR(MP_QSTR_center), MP_ROM_PTR(&str_center_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_lower), MP_ROM_PTR(&str_lower_obj) },
    { MP_ROM_QSTR(MP_QSTR_upper), MP_ROM_PTR(&str_upper_obj) },
    { MP_ROM_QSTR(MP_QSTR_isspace), MP_ROM_PTR(&str_isspace_obj) },
    { MP_ROM_QSTR(MP_QSTR_isalpha), MP_ROM_PTR(&str_isalpha_obj) },
    { MP_ROM_QSTR(MP_QSTR_isdigit), MP_ROM_PTR(&str_isdigit_obj) },
    { MP_ROM_QSTR(MP_QSTR_isupper), MP_ROM_PTR(&str_isupper_obj) },
    { MP_ROM_QSTR(MP_QSTR_islower), MP_ROM_PTR(&str_islower_obj) },
};

STATIC MP_DEFINE_CONST_DICT(str8_locals_dict, str8_locals_dict_table);

#if !MICROPY_PY_BUILTINS_STR_UNICODE
STATIC mp_obj_t mp_obj_new_str_iterator(mp_obj_t str, mp_obj_iter_buf_t *iter_buf);

const mp_obj_type_t mp_type_str = {
    { &mp_type_type },
    .name = MP_QSTR_str,
    .print = str_print,
    .make_new = mp_obj_str_make_new,
    .binary_op = mp_obj_str_binary_op,
    .subscr = bytes_subscr,
    .getiter = mp_obj_new_str_iterator,
    .buffer_p = { .get_buffer = mp_obj_str_get_buffer },
    .locals_dict = (mp_obj_dict_t*)&str8_locals_dict,
};
#endif

// Reuses most of methods from str
const mp_obj_type_t mp_type_bytes = {
    { &mp_type_type },
    .name = MP_QSTR_bytes,
    .print = str_print,
    .make_new = bytes_make_new,
    .binary_op = mp_obj_str_binary_op,
    .subscr = bytes_subscr,
    .getiter = mp_obj_new_bytes_iterator,
    .buffer_p = { .get_buffer = mp_obj_str_get_buffer },
    .locals_dict = (mp_obj_dict_t*)&str8_locals_dict,
};

// the zero-length bytes
const mp_obj_str_t mp_const_empty_bytes_obj = {{&mp_type_bytes}, 0, 0, NULL};

// Create a str/bytes object using the given data.  New memory is allocated and
// the data is copied across.
mp_obj_t mp_obj_new_str_of_type(const mp_obj_type_t *type, const byte* data, size_t len) {
    mp_obj_str_t *o = m_new_obj(mp_obj_str_t);
    o->base.type = type;
    o->len = len;
    if (data) {
        o->hash = qstr_compute_hash(data, len);
        byte *p = m_new(byte, len + 1);
        o->data = p;
        memcpy(p, data, len * sizeof(byte));
        p[len] = '\0'; // for now we add null for compatibility with C ASCIIZ strings
    }
    return MP_OBJ_FROM_PTR(o);
}

// Create a str/bytes object from the given vstr.  The vstr buffer is resized to
// the exact length required and then reused for the str/bytes object.  The vstr
// is cleared and can safely be passed to vstr_free if it was heap allocated.
mp_obj_t mp_obj_new_str_from_vstr(const mp_obj_type_t *type, vstr_t *vstr) {
    // if not a bytes object, look if a qstr with this data already exists
    if (type == &mp_type_str) {
        qstr q = qstr_find_strn(vstr->buf, vstr->len);
        if (q != MP_QSTR_NULL) {
            vstr_clear(vstr);
            vstr->alloc = 0;
            return MP_OBJ_NEW_QSTR(q);
        }
    }

    // make a new str/bytes object
    mp_obj_str_t *o = m_new_obj(mp_obj_str_t);
    o->base.type = type;
    o->len = vstr->len;
    o->hash = qstr_compute_hash((byte*)vstr->buf, vstr->len);
    if (vstr->len + 1 == vstr->alloc) {
        o->data = (byte*)vstr->buf;
    } else {
        o->data = (byte*)m_renew(char, vstr->buf, vstr->alloc, vstr->len + 1);
    }
    ((byte*)o->data)[o->len] = '\0'; // add null byte
    vstr->buf = NULL;
    vstr->alloc = 0;
    return MP_OBJ_FROM_PTR(o);
}

mp_obj_t mp_obj_new_str(const char* data, size_t len, bool make_qstr_if_not_already) {
    if (make_qstr_if_not_already) {
        // use existing, or make a new qstr
        return MP_OBJ_NEW_QSTR(qstr_from_strn(data, len));
    } else {
        qstr q = qstr_find_strn(data, len);
        if (q != MP_QSTR_NULL) {
            // qstr with this data already exists
            return MP_OBJ_NEW_QSTR(q);
        } else {
            // no existing qstr, don't make one
            return mp_obj_new_str_of_type(&mp_type_str, (const byte*)data, len);
        }
    }
}

mp_obj_t mp_obj_str_intern(mp_obj_t str) {
    GET_STR_DATA_LEN(str, data, len);
    return MP_OBJ_NEW_QSTR(qstr_from_strn((const char*)data, len));
}

mp_obj_t mp_obj_new_bytes(const byte* data, size_t len) {
    return mp_obj_new_str_of_type(&mp_type_bytes, data, len);
}

bool mp_obj_str_equal(mp_obj_t s1, mp_obj_t s2) {
    if (MP_OBJ_IS_QSTR(s1) && MP_OBJ_IS_QSTR(s2)) {
        return s1 == s2;
    } else {
        GET_STR_HASH(s1, h1);
        GET_STR_HASH(s2, h2);
        // If any of hashes is 0, it means it's not valid
        if (h1 != 0 && h2 != 0 && h1 != h2) {
            return false;
        }
        GET_STR_DATA_LEN(s1, d1, l1);
        GET_STR_DATA_LEN(s2, d2, l2);
        if (l1 != l2) {
            return false;
        }
        return memcmp(d1, d2, l1) == 0;
    }
}

STATIC void bad_implicit_conversion(mp_obj_t self_in) {
    if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
        mp_raise_TypeError("can't convert to str implicitly");
    } else {
        const qstr src_name = mp_obj_get_type(self_in)->name;
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError,
            "can't convert '%q' object to %q implicitly",
            src_name, src_name == MP_QSTR_str ? MP_QSTR_bytes : MP_QSTR_str));
    }
}

// use this if you will anyway convert the string to a qstr
// will be more efficient for the case where it's already a qstr
qstr mp_obj_str_get_qstr(mp_obj_t self_in) {
    if (MP_OBJ_IS_QSTR(self_in)) {
        return MP_OBJ_QSTR_VALUE(self_in);
    } else if (MP_OBJ_IS_TYPE(self_in, &mp_type_str)) {
        mp_obj_str_t *self = MP_OBJ_TO_PTR(self_in);
        return qstr_from_strn((char*)self->data, self->len);
    } else {
        bad_implicit_conversion(self_in);
    }
}

// only use this function if you need the str data to be zero terminated
// at the moment all strings are zero terminated to help with C ASCIIZ compatibility
const char *mp_obj_str_get_str(mp_obj_t self_in) {
    if (MP_OBJ_IS_STR_OR_BYTES(self_in)) {
        GET_STR_DATA_LEN(self_in, s, l);
        (void)l; // len unused
        return (const char*)s;
    } else {
        bad_implicit_conversion(self_in);
    }
}

const char *mp_obj_str_get_data(mp_obj_t self_in, size_t *len) {
    if (MP_OBJ_IS_STR_OR_BYTES(self_in)) {
        GET_STR_DATA_LEN(self_in, s, l);
        *len = l;
        return (const char*)s;
    } else {
        bad_implicit_conversion(self_in);
    }
}

#if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_C
const byte *mp_obj_str_get_data_no_check(mp_obj_t self_in, size_t *len) {
    if (MP_OBJ_IS_QSTR(self_in)) {
        return qstr_data(MP_OBJ_QSTR_VALUE(self_in), len);
    } else {
        *len = ((mp_obj_str_t*)self_in)->len;
        return ((mp_obj_str_t*)self_in)->data;
    }
}
#endif

/******************************************************************************/
/* str iterator                                                               */

typedef struct _mp_obj_str8_it_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    mp_obj_t str;
    size_t cur;
} mp_obj_str8_it_t;

#if !MICROPY_PY_BUILTINS_STR_UNICODE
STATIC mp_obj_t str_it_iternext(mp_obj_t self_in) {
    mp_obj_str8_it_t *self = MP_OBJ_TO_PTR(self_in);
    GET_STR_DATA_LEN(self->str, str, len);
    if (self->cur < len) {
        mp_obj_t o_out = mp_obj_new_str((const char*)str + self->cur, 1, true);
        self->cur += 1;
        return o_out;
    } else {
        return MP_OBJ_STOP_ITERATION;
    }
}

STATIC mp_obj_t mp_obj_new_str_iterator(mp_obj_t str, mp_obj_iter_buf_t *iter_buf) {
    assert(sizeof(mp_obj_str8_it_t) <= sizeof(mp_obj_iter_buf_t));
    mp_obj_str8_it_t *o = (mp_obj_str8_it_t*)iter_buf;
    o->base.type = &mp_type_polymorph_iter;
    o->iternext = str_it_iternext;
    o->str = str;
    o->cur = 0;
    return MP_OBJ_FROM_PTR(o);
}
#endif

STATIC mp_obj_t bytes_it_iternext(mp_obj_t self_in) {
    mp_obj_str8_it_t *self = MP_OBJ_TO_PTR(self_in);
    GET_STR_DATA_LEN(self->str, str, len);
    if (self->cur < len) {
        mp_obj_t o_out = MP_OBJ_NEW_SMALL_INT(str[self->cur]);
        self->cur += 1;
        return o_out;
    } else {
        return MP_OBJ_STOP_ITERATION;
    }
}

mp_obj_t mp_obj_new_bytes_iterator(mp_obj_t str, mp_obj_iter_buf_t *iter_buf) {
    assert(sizeof(mp_obj_str8_it_t) <= sizeof(mp_obj_iter_buf_t));
    mp_obj_str8_it_t *o = (mp_obj_str8_it_t*)iter_buf;
    o->base.type = &mp_type_polymorph_iter;
    o->iternext = bytes_it_iternext;
    o->str = str;
    o->cur = 0;
    return MP_OBJ_FROM_PTR(o);
}
