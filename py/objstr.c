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

#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "mpconfig.h"
#include "nlr.h"
#include "misc.h"
#include "unicode.h"
#include "qstr.h"
#include "obj.h"
#include "runtime0.h"
#include "runtime.h"
#include "pfenv.h"
#include "objstr.h"
#include "objlist.h"

STATIC mp_obj_t str_modulo_format(mp_obj_t pattern, uint n_args, const mp_obj_t *args, mp_obj_t dict);
const mp_obj_t mp_const_empty_bytes;

mp_obj_t mp_obj_new_str_iterator(mp_obj_t str);
STATIC mp_obj_t mp_obj_new_bytes_iterator(mp_obj_t str);
STATIC NORETURN void bad_implicit_conversion(mp_obj_t self_in);
STATIC NORETURN void arg_type_mixup();

STATIC bool is_str_or_bytes(mp_obj_t o) {
    return MP_OBJ_IS_STR(o) || MP_OBJ_IS_TYPE(o, &mp_type_bytes);
}

/******************************************************************************/
/* str                                                                        */

void mp_str_print_quoted(void (*print)(void *env, const char *fmt, ...), void *env,
                         const byte *str_data, uint str_len, bool is_bytes) {
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
    for (const byte *s = str_data, *top = str_data + str_len; s < top; s++) {
        if (*s == quote_char) {
            print(env, "\\%c", quote_char);
        } else if (*s == '\\') {
            print(env, "\\\\");
        } else if (*s >= 0x20 && *s != 0x7f && (!is_bytes || *s < 0x80)) {
            // In strings, anything which is not ascii control character
            // is printed as is, this includes characters in range 0x80-0xff
            // (which can be non-Latin letters, etc.)
            print(env, "%c", *s);
        } else if (*s == '\n') {
            print(env, "\\n");
        } else if (*s == '\r') {
            print(env, "\\r");
        } else if (*s == '\t') {
            print(env, "\\t");
        } else {
            print(env, "\\x%02x", *s);
        }
    }
    print(env, "%c", quote_char);
}

STATIC void str_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    GET_STR_DATA_LEN(self_in, str_data, str_len);
    bool is_bytes = MP_OBJ_IS_TYPE(self_in, &mp_type_bytes);
    if (kind == PRINT_STR && !is_bytes) {
        print(env, "%.*s", str_len, str_data);
    } else {
        if (is_bytes) {
            print(env, "b");
        }
        mp_str_print_quoted(print, env, str_data, str_len, is_bytes);
    }
}

STATIC mp_obj_t str_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
#if MICROPY_CPYTHON_COMPAT
    if (n_kw != 0) {
        mp_arg_error_unimpl_kw();
    }
#endif

    switch (n_args) {
        case 0:
            return MP_OBJ_NEW_QSTR(MP_QSTR_);

        case 1:
        {
            vstr_t *vstr = vstr_new();
            mp_obj_print_helper((void (*)(void*, const char*, ...))vstr_printf, vstr, args[0], PRINT_STR);
            mp_obj_t s = mp_obj_new_str(vstr->buf, vstr->len, false);
            vstr_free(vstr);
            return s;
        }

        case 2:
        case 3:
        {
            // TODO: validate 2nd/3rd args
            if (!MP_OBJ_IS_TYPE(args[0], &mp_type_bytes)) {
                nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "bytes expected"));
            }
            GET_STR_DATA_LEN(args[0], str_data, str_len);
            GET_STR_HASH(args[0], str_hash);
            mp_obj_str_t *o = mp_obj_new_str_of_type(&mp_type_str, NULL, str_len);
            o->data = str_data;
            o->hash = str_hash;
            return o;
        }

        default:
            nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "str takes at most 3 arguments"));
    }
}

STATIC mp_obj_t bytes_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    if (n_args == 0) {
        return mp_const_empty_bytes;
    }

#if MICROPY_CPYTHON_COMPAT
    if (n_kw != 0) {
        mp_arg_error_unimpl_kw();
    }
#endif

    if (MP_OBJ_IS_STR(args[0])) {
        if (n_args < 2 || n_args > 3) {
            goto wrong_args;
        }
        GET_STR_DATA_LEN(args[0], str_data, str_len);
        GET_STR_HASH(args[0], str_hash);
        mp_obj_str_t *o = mp_obj_new_str_of_type(&mp_type_bytes, NULL, str_len);
        o->data = str_data;
        o->hash = str_hash;
        return o;
    }

    if (n_args > 1) {
        goto wrong_args;
    }

    if (MP_OBJ_IS_SMALL_INT(args[0])) {
        uint len = MP_OBJ_SMALL_INT_VALUE(args[0]);
        byte *data;

        mp_obj_t o = mp_obj_str_builder_start(&mp_type_bytes, len, &data);
        memset(data, 0, len);
        return mp_obj_str_builder_end(o);
    }

    int len;
    byte *data;
    vstr_t *vstr = NULL;
    mp_obj_t o = NULL;
    // Try to create array of exact len if initializer len is known
    mp_obj_t len_in = mp_obj_len_maybe(args[0]);
    if (len_in == MP_OBJ_NULL) {
        len = -1;
        vstr = vstr_new();
    } else {
        len = MP_OBJ_SMALL_INT_VALUE(len_in);
        o = mp_obj_str_builder_start(&mp_type_bytes, len, &data);
    }

    mp_obj_t iterable = mp_getiter(args[0]);
    mp_obj_t item;
    while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        if (len == -1) {
            vstr_add_char(vstr, MP_OBJ_SMALL_INT_VALUE(item));
        } else {
            *data++ = MP_OBJ_SMALL_INT_VALUE(item);
        }
    }

    if (len == -1) {
        vstr_shrink(vstr);
        // TODO: Optimize, borrow buffer from vstr
        len = vstr_len(vstr);
        o = mp_obj_str_builder_start(&mp_type_bytes, len, &data);
        memcpy(data, vstr_str(vstr), len);
        vstr_free(vstr);
    }

    return mp_obj_str_builder_end(o);

wrong_args:
        nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "wrong number of arguments"));
}

// like strstr but with specified length and allows \0 bytes
// TODO replace with something more efficient/standard
STATIC const byte *find_subbytes(const byte *haystack, mp_uint_t hlen, const byte *needle, mp_uint_t nlen, mp_int_t direction) {
    if (hlen >= nlen) {
        mp_uint_t str_index, str_index_end;
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

mp_obj_t mp_obj_str_binary_op(int op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    GET_STR_DATA_LEN(lhs_in, lhs_data, lhs_len);
    mp_obj_type_t *lhs_type = mp_obj_get_type(lhs_in);
    mp_obj_type_t *rhs_type = mp_obj_get_type(rhs_in);
    switch (op) {
        case MP_BINARY_OP_ADD:
        case MP_BINARY_OP_INPLACE_ADD:
            if (lhs_type == rhs_type) {
                // add 2 strings or bytes

                GET_STR_DATA_LEN(rhs_in, rhs_data, rhs_len);
                int alloc_len = lhs_len + rhs_len;

                /* code for making qstr
                byte *q_ptr;
                byte *val = qstr_build_start(alloc_len, &q_ptr);
                memcpy(val, lhs_data, lhs_len);
                memcpy(val + lhs_len, rhs_data, rhs_len);
                return MP_OBJ_NEW_QSTR(qstr_build_end(q_ptr));
                */

                // code for non-qstr
                byte *data;
                mp_obj_t s = mp_obj_str_builder_start(lhs_type, alloc_len, &data);
                memcpy(data, lhs_data, lhs_len);
                memcpy(data + lhs_len, rhs_data, rhs_len);
                return mp_obj_str_builder_end(s);
            }
            break;

        case MP_BINARY_OP_IN:
            /* NOTE `a in b` is `b.__contains__(a)` */
            if (lhs_type == rhs_type) {
                GET_STR_DATA_LEN(rhs_in, rhs_data, rhs_len);
                return MP_BOOL(find_subbytes(lhs_data, lhs_len, rhs_data, rhs_len, 1) != NULL);
            }
            break;

        case MP_BINARY_OP_MULTIPLY: {
            mp_int_t n;
            if (!mp_obj_get_int_maybe(rhs_in, &n)) {
                return MP_OBJ_NULL; // op not supported
            }
            if (n <= 0) {
                if (lhs_type == &mp_type_str) {
                    return MP_OBJ_NEW_QSTR(MP_QSTR_); // empty str
                }
                n = 0;
            }
            byte *data;
            mp_obj_t s = mp_obj_str_builder_start(lhs_type, lhs_len * n, &data);
            mp_seq_multiply(lhs_data, sizeof(*lhs_data), lhs_len, n, data);
            return mp_obj_str_builder_end(s);
        }

        case MP_BINARY_OP_MODULO: {
            mp_obj_t *args;
            uint n_args;
            mp_obj_t dict = MP_OBJ_NULL;
            if (MP_OBJ_IS_TYPE(rhs_in, &mp_type_tuple)) {
                // TODO: Support tuple subclasses?
                mp_obj_tuple_get(rhs_in, &n_args, &args);
            } else if (MP_OBJ_IS_TYPE(rhs_in, &mp_type_dict)) {
                args = NULL;
                n_args = 0;
                dict = rhs_in;
            } else {
                args = &rhs_in;
                n_args = 1;
            }
            return str_modulo_format(lhs_in, n_args, args, dict);
        }

        //case MP_BINARY_OP_NOT_EQUAL: // This is never passed here
        case MP_BINARY_OP_EQUAL: // This will be passed only for bytes, str is dealt with in mp_obj_equal()
        case MP_BINARY_OP_LESS:
        case MP_BINARY_OP_LESS_EQUAL:
        case MP_BINARY_OP_MORE:
        case MP_BINARY_OP_MORE_EQUAL:
            if (lhs_type == rhs_type) {
                GET_STR_DATA_LEN(rhs_in, rhs_data, rhs_len);
                return MP_BOOL(mp_seq_cmp_bytes(op, lhs_data, lhs_len, rhs_data, rhs_len));
            }
            if (lhs_type == &mp_type_bytes) {
                mp_buffer_info_t bufinfo;
                if (!mp_get_buffer(rhs_in, &bufinfo, MP_BUFFER_READ)) {
                    goto uncomparable;
                }
                return MP_BOOL(mp_seq_cmp_bytes(op, lhs_data, lhs_len, bufinfo.buf, bufinfo.len));
            }
uncomparable:
            if (op == MP_BINARY_OP_EQUAL) {
                return mp_const_false;
            }
    }

    return MP_OBJ_NULL; // op not supported
}

#if !MICROPY_PY_BUILTINS_STR_UNICODE
// objstrunicode defines own version
const byte *str_index_to_ptr(const mp_obj_type_t *type, const byte *self_data, uint self_len,
                             mp_obj_t index, bool is_slice) {
    mp_uint_t index_val = mp_get_index(type, self_len, index, is_slice);
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
                nlr_raise(mp_obj_new_exception_msg(&mp_type_NotImplementedError,
                    "only slices with step=1 (aka None) are supported"));
            }
            return mp_obj_new_str_of_type(type, self_data + slice.start, slice.stop - slice.start);
        }
#endif
        mp_uint_t index_val = mp_get_index(type, self_len, index, false);
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
    assert(is_str_or_bytes(self_in));
    const mp_obj_type_t *self_type = mp_obj_get_type(self_in);

    // get separation string
    GET_STR_DATA_LEN(self_in, sep_str, sep_len);

    // process args
    uint seq_len;
    mp_obj_t *seq_items;
    if (MP_OBJ_IS_TYPE(arg, &mp_type_tuple)) {
        mp_obj_tuple_get(arg, &seq_len, &seq_items);
    } else {
        if (!MP_OBJ_IS_TYPE(arg, &mp_type_list)) {
            // arg is not a list, try to convert it to one
            // TODO: Try to optimize?
            arg = mp_type_list.make_new((mp_obj_t)&mp_type_list, 1, 0, &arg);
        }
        mp_obj_list_get(arg, &seq_len, &seq_items);
    }

    // count required length
    int required_len = 0;
    for (int i = 0; i < seq_len; i++) {
        if (mp_obj_get_type(seq_items[i]) != self_type) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError,
                "join expects a list of str/bytes objects consistent with self object"));
        }
        if (i > 0) {
            required_len += sep_len;
        }
        GET_STR_LEN(seq_items[i], l);
        required_len += l;
    }

    // make joined string
    byte *data;
    mp_obj_t joined_str = mp_obj_str_builder_start(self_type, required_len, &data);
    for (int i = 0; i < seq_len; i++) {
        if (i > 0) {
            memcpy(data, sep_str, sep_len);
            data += sep_len;
        }
        GET_STR_DATA_LEN(seq_items[i], s, l);
        memcpy(data, s, l);
        data += l;
    }

    // return joined string
    return mp_obj_str_builder_end(joined_str);
}

#define is_ws(c) ((c) == ' ' || (c) == '\t')

STATIC mp_obj_t str_split(uint n_args, const mp_obj_t *args) {
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
        while (s < top && is_ws(*s)) s++;
        while (s < top && splits != 0) {
            const byte *start = s;
            while (s < top && !is_ws(*s)) s++;
            mp_obj_list_append(res, mp_obj_new_str_of_type(self_type, start, s - start));
            if (s >= top) {
                break;
            }
            while (s < top && is_ws(*s)) s++;
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
            arg_type_mixup();
        }

        uint sep_len;
        const char *sep_str = mp_obj_str_get_data(sep, &sep_len);

        if (sep_len == 0) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "empty separator"));
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

STATIC mp_obj_t str_rsplit(uint n_args, const mp_obj_t *args) {
    if (n_args < 3) {
        // If we don't have split limit, it doesn't matter from which side
        // we split.
        return str_split(n_args, args);
    }
    const mp_obj_type_t *self_type = mp_obj_get_type(args[0]);
    mp_obj_t sep = args[1];
    GET_STR_DATA_LEN(args[0], s, len);

    mp_int_t splits = mp_obj_get_int(args[2]);
    mp_int_t org_splits = splits;
    // Preallocate list to the max expected # of elements, as we
    // will fill it from the end.
    mp_obj_list_t *res = mp_obj_new_list(splits + 1, NULL);
    int idx = splits;

    if (sep == mp_const_none) {
        assert(!"TODO: rsplit(None,n) not implemented");
    } else {
        uint sep_len;
        const char *sep_str = mp_obj_str_get_data(sep, &sep_len);

        if (sep_len == 0) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "empty separator"));
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
            int used = org_splits + 1 - idx;
            memcpy(res->items, &res->items[idx], used * sizeof(mp_obj_t));
            mp_seq_clear(res->items, used, res->alloc, sizeof(*res->items));
            res->len = used;
        }
    }

    return res;
}

STATIC mp_obj_t str_finder(uint n_args, const mp_obj_t *args, mp_int_t direction, bool is_index) {
    const mp_obj_type_t *self_type = mp_obj_get_type(args[0]);
    assert(2 <= n_args && n_args <= 4);
    assert(MP_OBJ_IS_STR(args[0]));
    assert(MP_OBJ_IS_STR(args[1]));

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
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "substring not found"));
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

STATIC mp_obj_t str_find(uint n_args, const mp_obj_t *args) {
    return str_finder(n_args, args, 1, false);
}

STATIC mp_obj_t str_rfind(uint n_args, const mp_obj_t *args) {
    return str_finder(n_args, args, -1, false);
}

STATIC mp_obj_t str_index(uint n_args, const mp_obj_t *args) {
    return str_finder(n_args, args, 1, true);
}

STATIC mp_obj_t str_rindex(uint n_args, const mp_obj_t *args) {
    return str_finder(n_args, args, -1, true);
}

// TODO: (Much) more variety in args
STATIC mp_obj_t str_startswith(uint n_args, const mp_obj_t *args) {
    const mp_obj_type_t *self_type = mp_obj_get_type(args[0]);
    GET_STR_DATA_LEN(args[0], str, str_len);
    GET_STR_DATA_LEN(args[1], prefix, prefix_len);
    const byte *start = str;
    if (n_args > 2) {
        start = str_index_to_ptr(self_type, str, str_len, args[2], true);
    }
    if (prefix_len + (start - str) > str_len) {
        return mp_const_false;
    }
    return MP_BOOL(memcmp(start, prefix, prefix_len) == 0);
}

STATIC mp_obj_t str_endswith(uint n_args, const mp_obj_t *args) {
    GET_STR_DATA_LEN(args[0], str, str_len);
    GET_STR_DATA_LEN(args[1], suffix, suffix_len);
    assert(n_args == 2);

    if (suffix_len > str_len) {
        return mp_const_false;
    }
    return MP_BOOL(memcmp(str + (str_len - suffix_len), suffix, suffix_len) == 0);
}

enum { LSTRIP, RSTRIP, STRIP };

STATIC mp_obj_t str_uni_strip(int type, uint n_args, const mp_obj_t *args) {
    assert(1 <= n_args && n_args <= 2);
    assert(is_str_or_bytes(args[0]));
    const mp_obj_type_t *self_type = mp_obj_get_type(args[0]);

    const byte *chars_to_del;
    uint chars_to_del_len;
    static const byte whitespace[] = " \t\n\r\v\f";

    if (n_args == 1) {
        chars_to_del = whitespace;
        chars_to_del_len = sizeof(whitespace);
    } else {
        if (mp_obj_get_type(args[1]) != self_type) {
            arg_type_mixup();
        }
        GET_STR_DATA_LEN(args[1], s, l);
        chars_to_del = s;
        chars_to_del_len = l;
    }

    GET_STR_DATA_LEN(args[0], orig_str, orig_str_len);

    mp_uint_t first_good_char_pos = 0;
    bool first_good_char_pos_set = false;
    mp_uint_t last_good_char_pos = 0;
    mp_uint_t i = 0;
    mp_int_t delta = 1;
    if (type == RSTRIP) {
        i = orig_str_len - 1;
        delta = -1;
    }
    for (mp_uint_t len = orig_str_len; len > 0; len--) {
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
        return MP_OBJ_NEW_QSTR(MP_QSTR_);
    }

    assert(last_good_char_pos >= first_good_char_pos);
    //+1 to accomodate the last character
    mp_uint_t stripped_len = last_good_char_pos - first_good_char_pos + 1;
    if (stripped_len == orig_str_len) {
        // If nothing was stripped, don't bother to dup original string
        // TODO: watch out for this case when we'll get to bytearray.strip()
        assert(first_good_char_pos == 0);
        return args[0];
    }
    return mp_obj_new_str_of_type(self_type, orig_str + first_good_char_pos, stripped_len);
}

STATIC mp_obj_t str_strip(uint n_args, const mp_obj_t *args) {
    return str_uni_strip(STRIP, n_args, args);
}

STATIC mp_obj_t str_lstrip(uint n_args, const mp_obj_t *args) {
    return str_uni_strip(LSTRIP, n_args, args);
}

STATIC mp_obj_t str_rstrip(uint n_args, const mp_obj_t *args) {
    return str_uni_strip(RSTRIP, n_args, args);
}

// Takes an int arg, but only parses unsigned numbers, and only changes
// *num if at least one digit was parsed.
static int str_to_int(const char *str, int *num) {
    const char *s = str;
    if (unichar_isdigit(*s)) {
        *num = 0;
        do {
            *num = *num * 10 + (*s - '0');
            s++;
        }
        while (unichar_isdigit(*s));
    }
    return s - str;
}

static bool isalignment(char ch) {
    return ch && strchr("<>=^", ch) != NULL;
}

static bool istype(char ch) {
    return ch && strchr("bcdeEfFgGnosxX%", ch) != NULL;
}

static bool arg_looks_integer(mp_obj_t arg) {
    return MP_OBJ_IS_TYPE(arg, &mp_type_bool) || MP_OBJ_IS_INT(arg);
}

static bool arg_looks_numeric(mp_obj_t arg) {
    return arg_looks_integer(arg)
#if MICROPY_PY_BUILTINS_FLOAT
        || MP_OBJ_IS_TYPE(arg, &mp_type_float)
#endif
    ;
}

static mp_obj_t arg_as_int(mp_obj_t arg) {
#if MICROPY_PY_BUILTINS_FLOAT
    if (MP_OBJ_IS_TYPE(arg, &mp_type_float)) {

        // TODO: Needs a way to construct an mpz integer from a float

        mp_int_t num = mp_obj_get_float(arg);
        return MP_OBJ_NEW_SMALL_INT(num);
    }
#endif
    return arg;
}

mp_obj_t mp_obj_str_format(uint n_args, const mp_obj_t *args) {
    assert(MP_OBJ_IS_STR(args[0]));

    GET_STR_DATA_LEN(args[0], str, len);
    int arg_i = 0;
    vstr_t *vstr = vstr_new();
    pfenv_t pfenv_vstr;
    pfenv_vstr.data = vstr;
    pfenv_vstr.print_strn = pfenv_vstr_add_strn;

    for (const byte *top = str + len; str < top; str++) {
        if (*str == '}') {
            str++;
            if (str < top && *str == '}') {
                vstr_add_char(vstr, '}');
                continue;
            }
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "single '}' encountered in format string"));
        }
        if (*str != '{') {
            vstr_add_char(vstr, *str);
            continue;
        }

        str++;
        if (str < top && *str == '{') {
            vstr_add_char(vstr, '{');
            continue;
        }

        // replacement_field ::=  "{" [field_name] ["!" conversion] [":" format_spec] "}"

        vstr_t *field_name = NULL;
        char conversion = '\0';
        vstr_t *format_spec = NULL;

        if (str < top && *str != '}' && *str != '!' && *str != ':') {
            field_name = vstr_new();
            while (str < top && *str != '}' && *str != '!' && *str != ':') {
                vstr_add_char(field_name, *str++);
            }
            vstr_add_char(field_name, '\0');
        }

        // conversion ::=  "r" | "s"

        if (str < top && *str == '!') {
            str++;
            if (str < top && (*str == 'r' || *str == 's')) {
                conversion = *str++;
            } else {
                nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "end of format while looking for conversion specifier"));
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
                format_spec = vstr_new();
                while (str < top && *str != '}') {
                    vstr_add_char(format_spec, *str++);
                }
                vstr_add_char(format_spec, '\0');
            }
        }
        if (str >= top) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "unmatched '{' in format"));
        }
        if (*str != '}') {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "expected ':' after format specifier"));
        }

        mp_obj_t arg = mp_const_none;

        if (field_name) {
            if (arg_i > 0) {
                nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "can't switch from automatic field numbering to manual field specification"));
            }
            int index = 0;
            if (str_to_int(vstr_str(field_name), &index) != vstr_len(field_name) - 1) {
                nlr_raise(mp_obj_new_exception_msg(&mp_type_KeyError, "attributes not supported yet"));
            }
            if (index >= n_args - 1) {
                nlr_raise(mp_obj_new_exception_msg(&mp_type_IndexError, "tuple index out of range"));
            }
            arg = args[index + 1];
            arg_i = -1;
            vstr_free(field_name);
            field_name = NULL;
        } else {
            if (arg_i < 0) {
                nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "can't switch from manual field specification to automatic field numbering"));
            }
            if (arg_i >= n_args - 1) {
                nlr_raise(mp_obj_new_exception_msg(&mp_type_IndexError, "tuple index out of range"));
            }
            arg = args[arg_i + 1];
            arg_i++;
        }
        if (!format_spec && !conversion) {
            conversion = 's';
        }
        if (conversion) {
            mp_print_kind_t print_kind;
            if (conversion == 's') {
                print_kind = PRINT_STR;
            } else if (conversion == 'r') {
                print_kind = PRINT_REPR;
            } else {
                nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "unknown conversion specifier %c", conversion));
            }
            vstr_t *arg_vstr = vstr_new();
            mp_obj_print_helper((void (*)(void*, const char*, ...))vstr_printf, arg_vstr, arg, print_kind);
            arg = mp_obj_new_str(vstr_str(arg_vstr), vstr_len(arg_vstr), false);
            vstr_free(arg_vstr);
        }

        char sign = '\0';
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

            const char *s = vstr_str(format_spec);
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
                sign = *s++;
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
            s += str_to_int(s, &width);
            if (*s == ',') {
                flags |= PF_FLAG_SHOW_COMMA;
                s++;
            }
            if (*s == '.') {
                s++;
                s += str_to_int(s, &precision);
            }
            if (istype(*s)) {
                type = *s++;
            }
            if (*s) {
                nlr_raise(mp_obj_new_exception_msg(&mp_type_KeyError, "Invalid conversion specification"));
            }
            vstr_free(format_spec);
            format_spec = NULL;
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

        if (sign) {
            if (type == 's') {
                nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Sign not allowed in string format specifier"));
            }
            if (type == 'c') {
                nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Sign not allowed with integer format specifier 'c'"));
            }
        } else {
            sign = '-';
        }

        switch (align) {
            case '<': flags |= PF_FLAG_LEFT_ADJUST;     break;
            case '=': flags |= PF_FLAG_PAD_AFTER_SIGN;  break;
            case '^': flags |= PF_FLAG_CENTER_ADJUST;   break;
        }

        if (arg_looks_integer(arg)) {
            switch (type) {
                case 'b':
                    pfenv_print_mp_int(&pfenv_vstr, arg, 1, 2, 'a', flags, fill, width, 0);
                    continue;

                case 'c':
                {
                    char ch = mp_obj_get_int(arg);
                    pfenv_print_strn(&pfenv_vstr, &ch, 1, flags, fill, width);
                    continue;
                }

                case '\0':  // No explicit format type implies 'd'
                case 'n':   // I don't think we support locales in uPy so use 'd'
                case 'd':
                    pfenv_print_mp_int(&pfenv_vstr, arg, 1, 10, 'a', flags, fill, width, 0);
                    continue;

                case 'o':
                    if (flags & PF_FLAG_SHOW_PREFIX) {
                        flags |= PF_FLAG_SHOW_OCTAL_LETTER;
                    }

                    pfenv_print_mp_int(&pfenv_vstr, arg, 1, 8, 'a', flags, fill, width, 0);
                    continue;

                case 'X':
                case 'x':
                    pfenv_print_mp_int(&pfenv_vstr, arg, 1, 16, type - ('X' - 'A'), flags, fill, width, 0);
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
                    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                        "unknown format code '%c' for object of type '%s'", type, mp_obj_get_type_str(arg)));
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

            flags |= PF_FLAG_PAD_NAN_INF; // '{:06e}'.format(float('-inf')) should give '-00inf'
            switch (type) {
#if MICROPY_PY_BUILTINS_FLOAT
                case 'e':
                case 'E':
                case 'f':
                case 'F':
                case 'g':
                case 'G':
                    pfenv_print_float(&pfenv_vstr, mp_obj_get_float(arg), type, flags, fill, width, precision);
                    break;

                case '%':
                    flags |= PF_FLAG_ADD_PERCENT;
                    pfenv_print_float(&pfenv_vstr, mp_obj_get_float(arg) * 100.0F, 'f', flags, fill, width, precision);
                    break;
#endif

                default:
                    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                        "unknown format code '%c' for object of type 'float'",
                        type, mp_obj_get_type_str(arg)));
            }
        } else {
            // arg doesn't look like a number

            if (align == '=') {
                nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "'=' alignment not allowed in string format specifier"));
            }

            switch (type) {
                case '\0':
                    mp_obj_print_helper((void (*)(void*, const char*, ...))vstr_printf, vstr, arg, PRINT_STR);
                    break;

                case 's':
                {
                    uint len;
                    const char *s = mp_obj_str_get_data(arg, &len);
                    if (precision < 0) {
                        precision = len;
                    }
                    if (len > precision) {
                        len = precision;
                    }
                    pfenv_print_strn(&pfenv_vstr, s, len, flags, fill, width);
                    break;
                }

                default:
                    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                        "unknown format code '%c' for object of type 'str'",
                        type, mp_obj_get_type_str(arg)));
            }
        }
    }

    mp_obj_t s = mp_obj_new_str(vstr->buf, vstr->len, false);
    vstr_free(vstr);
    return s;
}

STATIC mp_obj_t str_modulo_format(mp_obj_t pattern, uint n_args, const mp_obj_t *args, mp_obj_t dict) {
    assert(MP_OBJ_IS_STR(pattern));

    GET_STR_DATA_LEN(pattern, str, len);
    const byte *start_str = str;
    int arg_i = 0;
    vstr_t *vstr = vstr_new();
    pfenv_t pfenv_vstr;
    pfenv_vstr.data = vstr;
    pfenv_vstr.print_strn = pfenv_vstr_add_strn;

    for (const byte *top = str + len; str < top; str++) {
        mp_obj_t arg = MP_OBJ_NULL;
        if (*str != '%') {
            vstr_add_char(vstr, *str);
            continue;
        }
        if (++str >= top) {
            break;
        }
        if (*str == '%') {
            vstr_add_char(vstr, '%');
            continue;
        }

        // Dictionary value lookup
        if (*str == '(') {
            const byte *key = ++str;
            while (*str != ')') {
                if (str >= top) {
                    nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "incomplete format key"));
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
                for (; str < top && '0' <= *str && *str <= '9'; str++) {
                    width = width * 10 + *str - '0';
                }
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
                    for (; str < top && '0' <= *str && *str <= '9'; str++) {
                        prec = prec * 10 + *str - '0';
                    }
                }
            }
        }

        if (str >= top) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "incomplete format"));
        }

        // Tuple value lookup
        if (arg == MP_OBJ_NULL) {
            if (arg_i >= n_args) {
not_enough_args:
                nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "not enough arguments for format string"));
            }
            arg = args[arg_i++];
        }
        switch (*str) {
            case 'c':
                if (MP_OBJ_IS_STR(arg)) {
                    uint len;
                    const char *s = mp_obj_str_get_data(arg, &len);
                    if (len != 1) {
                        nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "%%c requires int or char"));
                        break;
                    }
                    pfenv_print_strn(&pfenv_vstr, s, 1, flags, ' ', width);
                    break;
                }
                if (arg_looks_integer(arg)) {
                    char ch = mp_obj_get_int(arg);
                    pfenv_print_strn(&pfenv_vstr, &ch, 1, flags, ' ', width);
                    break;
                }
#if MICROPY_PY_BUILTINS_FLOAT
                // This is what CPython reports, so we report the same.
                if (MP_OBJ_IS_TYPE(arg, &mp_type_float)) {
                    nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "integer argument expected, got float"));

                }
#endif
                nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "an integer is required"));
                break;

            case 'd':
            case 'i':
            case 'u':
                pfenv_print_mp_int(&pfenv_vstr, arg_as_int(arg), 1, 10, 'a', flags, fill, width, prec);
                break;

#if MICROPY_PY_BUILTINS_FLOAT
            case 'e':
            case 'E':
            case 'f':
            case 'F':
            case 'g':
            case 'G':
                pfenv_print_float(&pfenv_vstr, mp_obj_get_float(arg), *str, flags, fill, width, prec);
                break;
#endif

            case 'o':
                if (alt) {
                    flags |= (PF_FLAG_SHOW_PREFIX | PF_FLAG_SHOW_OCTAL_LETTER);
                }
                pfenv_print_mp_int(&pfenv_vstr, arg, 1, 8, 'a', flags, fill, width, prec);
                break;

            case 'r':
            case 's':
            {
                vstr_t *arg_vstr = vstr_new();
                mp_obj_print_helper((void (*)(void*, const char*, ...))vstr_printf,
                                    arg_vstr, arg, *str == 'r' ? PRINT_REPR : PRINT_STR);
                uint len = vstr_len(arg_vstr);
                if (prec < 0) {
                    prec = len;
                }
                if (len > prec) {
                    len = prec;
                }
                pfenv_print_strn(&pfenv_vstr, vstr_str(arg_vstr), len, flags, ' ', width);
                vstr_free(arg_vstr);
                break;
            }

            case 'X':
            case 'x':
                pfenv_print_mp_int(&pfenv_vstr, arg, 1, 16, *str - ('X' - 'A'), flags | alt, fill, width, prec);
                break;

            default:
                nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                    "unsupported format character '%c' (0x%x) at index %d",
                    *str, *str, str - start_str));
        }
    }

    if (arg_i != n_args) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "not all arguments converted during string formatting"));
    }

    mp_obj_t s = mp_obj_new_str(vstr->buf, vstr->len, false);
    vstr_free(vstr);
    return s;
}

STATIC mp_obj_t str_replace(uint n_args, const mp_obj_t *args) {
    assert(MP_OBJ_IS_STR(args[0]));

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

    if (!MP_OBJ_IS_STR(args[1])) {
        bad_implicit_conversion(args[1]);
    }

    if (!MP_OBJ_IS_STR(args[2])) {
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
    mp_obj_t replaced_str = MP_OBJ_NULL;

    // do 2 passes over the string:
    //   first pass computes the required length of the replaced string
    //   second pass does the replacements
    for (;;) {
        mp_uint_t replaced_str_index = 0;
        mp_uint_t num_replacements_done = 0;
        const byte *old_occurrence;
        const byte *offset_ptr = str;
        mp_uint_t str_len_remain = str_len;
        if (old_len == 0) {
            // if old_str is empty, copy new_str to start of replaced string
            // copy the replacement string
            if (data != NULL) {
                memcpy(data, new, new_len);
            }
            replaced_str_index += new_len;
            num_replacements_done++;
        }
        while (num_replacements_done != max_rep && str_len_remain > 0 && (old_occurrence = find_subbytes(offset_ptr, str_len_remain, old, old_len, 1)) != NULL) {
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
                replaced_str = mp_obj_str_builder_start(mp_obj_get_type(args[0]), replaced_str_index, &data);
                assert(data != NULL);
            }
        } else {
            // second pass, we are done
            break;
        }
    }

    return mp_obj_str_builder_end(replaced_str);
}

STATIC mp_obj_t str_count(uint n_args, const mp_obj_t *args) {
    const mp_obj_type_t *self_type = mp_obj_get_type(args[0]);
    assert(2 <= n_args && n_args <= 4);
    assert(MP_OBJ_IS_STR(args[0]));
    assert(MP_OBJ_IS_STR(args[1]));

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

STATIC mp_obj_t str_partitioner(mp_obj_t self_in, mp_obj_t arg, mp_int_t direction) {
    if (!is_str_or_bytes(self_in)) {
        assert(0);
    }
    mp_obj_type_t *self_type = mp_obj_get_type(self_in);
    if (self_type != mp_obj_get_type(arg)) {
        arg_type_mixup();
    }

    GET_STR_DATA_LEN(self_in, str, str_len);
    GET_STR_DATA_LEN(arg, sep, sep_len);

    if (sep_len == 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "empty separator"));
    }

    mp_obj_t result[] = {MP_OBJ_NEW_QSTR(MP_QSTR_), MP_OBJ_NEW_QSTR(MP_QSTR_), MP_OBJ_NEW_QSTR(MP_QSTR_)};

    if (direction > 0) {
        result[0] = self_in;
    } else {
        result[2] = self_in;
    }

    const byte *position_ptr = find_subbytes(str, str_len, sep, sep_len, direction);
    if (position_ptr != NULL) {
        mp_uint_t position = position_ptr - str;
        result[0] = mp_obj_new_str_of_type(self_type, str, position);
        result[1] = arg;
        result[2] = mp_obj_new_str_of_type(self_type, str + position + sep_len, str_len - position - sep_len);
    }

    return mp_obj_new_tuple(3, result);
}

STATIC mp_obj_t str_partition(mp_obj_t self_in, mp_obj_t arg) {
    return str_partitioner(self_in, arg, 1);
}

STATIC mp_obj_t str_rpartition(mp_obj_t self_in, mp_obj_t arg) {
    return str_partitioner(self_in, arg, -1);
}

// Supposedly not too critical operations, so optimize for code size
STATIC mp_obj_t str_caseconv(unichar (*op)(unichar), mp_obj_t self_in) {
    GET_STR_DATA_LEN(self_in, self_data, self_len);
    byte *data;
    mp_obj_t s = mp_obj_str_builder_start(mp_obj_get_type(self_in), self_len, &data);
    for (int i = 0; i < self_len; i++) {
        *data++ = op(*self_data++);
    }
    *data = 0;
    return mp_obj_str_builder_end(s);
}

STATIC mp_obj_t str_lower(mp_obj_t self_in) {
    return str_caseconv(unichar_tolower, self_in);
}

STATIC mp_obj_t str_upper(mp_obj_t self_in) {
    return str_caseconv(unichar_toupper, self_in);
}

STATIC mp_obj_t str_uni_istype(bool (*f)(unichar), mp_obj_t self_in) {
    GET_STR_DATA_LEN(self_in, self_data, self_len);

    if (self_len == 0) {
        return mp_const_false; // default to False for empty str
    }

    if (f != unichar_isupper && f != unichar_islower) {
        for (int i = 0; i < self_len; i++) {
            if (!f(*self_data++)) {
                return mp_const_false;
            }
        }
    } else {
        bool contains_alpha = false;

        for (int i = 0; i < self_len; i++) { // only check alphanumeric characters
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

STATIC mp_obj_t str_isalpha(mp_obj_t self_in) {
    return str_uni_istype(unichar_isalpha, self_in);
}

STATIC mp_obj_t str_isdigit(mp_obj_t self_in) {
    return str_uni_istype(unichar_isdigit, self_in);
}

STATIC mp_obj_t str_isupper(mp_obj_t self_in) {
    return str_uni_istype(unichar_isupper, self_in);
}

STATIC mp_obj_t str_islower(mp_obj_t self_in) {
    return str_uni_istype(unichar_islower, self_in);
}

#if MICROPY_CPYTHON_COMPAT
// These methods are superfluous in the presense of str() and bytes()
// constructors.
// TODO: should accept kwargs too
STATIC mp_obj_t bytes_decode(uint n_args, const mp_obj_t *args) {
    mp_obj_t new_args[2];
    if (n_args == 1) {
        new_args[0] = args[0];
        new_args[1] = MP_OBJ_NEW_QSTR(MP_QSTR_utf_hyphen_8);
        args = new_args;
        n_args++;
    }
    return str_make_new(NULL, n_args, 0, args);
}

// TODO: should accept kwargs too
STATIC mp_obj_t str_encode(uint n_args, const mp_obj_t *args) {
    mp_obj_t new_args[2];
    if (n_args == 1) {
        new_args[0] = args[0];
        new_args[1] = MP_OBJ_NEW_QSTR(MP_QSTR_utf_hyphen_8);
        args = new_args;
        n_args++;
    }
    return bytes_make_new(NULL, n_args, 0, args);
}
#endif

mp_int_t mp_obj_str_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, int flags) {
    if (flags == MP_BUFFER_READ) {
        GET_STR_DATA_LEN(self_in, str_data, str_len);
        bufinfo->buf = (void*)str_data;
        bufinfo->len = str_len;
        bufinfo->typecode = 'b';
        return 0;
    } else {
        // can't write to a string
        bufinfo->buf = NULL;
        bufinfo->len = 0;
        bufinfo->typecode = -1;
        return 1;
    }
}

#if MICROPY_CPYTHON_COMPAT
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(bytes_decode_obj, 1, 3, bytes_decode);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_encode_obj, 1, 3, str_encode);
#endif
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_find_obj, 2, 4, str_find);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_rfind_obj, 2, 4, str_rfind);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_index_obj, 2, 4, str_index);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_rindex_obj, 2, 4, str_rindex);
MP_DEFINE_CONST_FUN_OBJ_2(str_join_obj, str_join);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_split_obj, 1, 3, str_split);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_rsplit_obj, 1, 3, str_rsplit);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_startswith_obj, 2, 3, str_startswith);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_endswith_obj, 2, 3, str_endswith);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_strip_obj, 1, 2, str_strip);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_lstrip_obj, 1, 2, str_lstrip);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_rstrip_obj, 1, 2, str_rstrip);
MP_DEFINE_CONST_FUN_OBJ_VAR(str_format_obj, 1, mp_obj_str_format);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_replace_obj, 3, 4, str_replace);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_count_obj, 2, 4, str_count);
MP_DEFINE_CONST_FUN_OBJ_2(str_partition_obj, str_partition);
MP_DEFINE_CONST_FUN_OBJ_2(str_rpartition_obj, str_rpartition);
MP_DEFINE_CONST_FUN_OBJ_1(str_lower_obj, str_lower);
MP_DEFINE_CONST_FUN_OBJ_1(str_upper_obj, str_upper);
MP_DEFINE_CONST_FUN_OBJ_1(str_isspace_obj, str_isspace);
MP_DEFINE_CONST_FUN_OBJ_1(str_isalpha_obj, str_isalpha);
MP_DEFINE_CONST_FUN_OBJ_1(str_isdigit_obj, str_isdigit);
MP_DEFINE_CONST_FUN_OBJ_1(str_isupper_obj, str_isupper);
MP_DEFINE_CONST_FUN_OBJ_1(str_islower_obj, str_islower);

STATIC const mp_map_elem_t str_locals_dict_table[] = {
#if MICROPY_CPYTHON_COMPAT
    { MP_OBJ_NEW_QSTR(MP_QSTR_decode), (mp_obj_t)&bytes_decode_obj },
    #if !MICROPY_PY_BUILTINS_STR_UNICODE
    // If we have separate unicode type, then here we have methods only
    // for bytes type, and it should not have encode() methods. Otherwise,
    // we have non-compliant-but-practical bytestring type, which shares
    // method table with bytes, so they both have encode() and decode()
    // methods (which should do type checking at runtime).
    { MP_OBJ_NEW_QSTR(MP_QSTR_encode), (mp_obj_t)&str_encode_obj },
    #endif
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

STATIC MP_DEFINE_CONST_DICT(str_locals_dict, str_locals_dict_table);

#if !MICROPY_PY_BUILTINS_STR_UNICODE
const mp_obj_type_t mp_type_str = {
    { &mp_type_type },
    .name = MP_QSTR_str,
    .print = str_print,
    .make_new = str_make_new,
    .binary_op = mp_obj_str_binary_op,
    .subscr = bytes_subscr,
    .getiter = mp_obj_new_str_iterator,
    .buffer_p = { .get_buffer = mp_obj_str_get_buffer },
    .locals_dict = (mp_obj_t)&str_locals_dict,
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
    .locals_dict = (mp_obj_t)&str_locals_dict,
};

// the zero-length bytes
STATIC const mp_obj_str_t empty_bytes_obj = {{&mp_type_bytes}, 0, 0, NULL};
const mp_obj_t mp_const_empty_bytes = (mp_obj_t)&empty_bytes_obj;

mp_obj_t mp_obj_str_builder_start(const mp_obj_type_t *type, uint len, byte **data) {
    mp_obj_str_t *o = m_new_obj(mp_obj_str_t);
    o->base.type = type;
    o->len = len;
    o->hash = 0;
    byte *p = m_new(byte, len + 1);
    o->data = p;
    *data = p;
    return o;
}

mp_obj_t mp_obj_str_builder_end(mp_obj_t o_in) {
    mp_obj_str_t *o = o_in;
    o->hash = qstr_compute_hash(o->data, o->len);
    byte *p = (byte*)o->data;
    p[o->len] = '\0'; // for now we add null for compatibility with C ASCIIZ strings
    return o;
}

mp_obj_t mp_obj_str_builder_end_with_len(mp_obj_t o_in, mp_uint_t len) {
    mp_obj_str_t *o = o_in;
    o->data = m_renew(byte, (byte*)o->data, o->len + 1, len + 1);
    o->len = len;
    o->hash = qstr_compute_hash(o->data, o->len);
    byte *p = (byte*)o->data;
    p[o->len] = '\0'; // for now we add null for compatibility with C ASCIIZ strings
    return o;
}

mp_obj_t mp_obj_new_str_of_type(const mp_obj_type_t *type, const byte* data, uint len) {
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
    return o;
}

mp_obj_t mp_obj_new_str(const char* data, uint len, bool make_qstr_if_not_already) {
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

mp_obj_t mp_obj_new_bytes(const byte* data, uint len) {
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
    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "Can't convert '%s' object to str implicitly", mp_obj_get_type_str(self_in)));
}

STATIC void arg_type_mixup() {
    nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "Can't mix str and bytes arguments"));
}

uint mp_obj_str_get_hash(mp_obj_t self_in) {
    // TODO: This has too big overhead for hash accessor
    if (MP_OBJ_IS_STR(self_in) || MP_OBJ_IS_TYPE(self_in, &mp_type_bytes)) {
        GET_STR_HASH(self_in, h);
        return h;
    } else {
        bad_implicit_conversion(self_in);
    }
}

uint mp_obj_str_get_len(mp_obj_t self_in) {
    // TODO This has a double check for the type, one in obj.c and one here
    if (MP_OBJ_IS_STR(self_in) || MP_OBJ_IS_TYPE(self_in, &mp_type_bytes)) {
        GET_STR_LEN(self_in, l);
        return l;
    } else {
        bad_implicit_conversion(self_in);
    }
}

// use this if you will anyway convert the string to a qstr
// will be more efficient for the case where it's already a qstr
qstr mp_obj_str_get_qstr(mp_obj_t self_in) {
    if (MP_OBJ_IS_QSTR(self_in)) {
        return MP_OBJ_QSTR_VALUE(self_in);
    } else if (MP_OBJ_IS_TYPE(self_in, &mp_type_str)) {
        mp_obj_str_t *self = self_in;
        return qstr_from_strn((char*)self->data, self->len);
    } else {
        bad_implicit_conversion(self_in);
    }
}

// only use this function if you need the str data to be zero terminated
// at the moment all strings are zero terminated to help with C ASCIIZ compatibility
const char *mp_obj_str_get_str(mp_obj_t self_in) {
    if (MP_OBJ_IS_STR(self_in)) {
        GET_STR_DATA_LEN(self_in, s, l);
        (void)l; // len unused
        return (const char*)s;
    } else {
        bad_implicit_conversion(self_in);
    }
}

const char *mp_obj_str_get_data(mp_obj_t self_in, uint *len) {
    if (is_str_or_bytes(self_in)) {
        GET_STR_DATA_LEN(self_in, s, l);
        *len = l;
        return (const char*)s;
    } else {
        bad_implicit_conversion(self_in);
    }
}

/******************************************************************************/
/* str iterator                                                               */

typedef struct _mp_obj_str_it_t {
    mp_obj_base_t base;
    mp_obj_t str;
    mp_uint_t cur;
} mp_obj_str_it_t;

#if !MICROPY_PY_BUILTINS_STR_UNICODE
STATIC mp_obj_t str_it_iternext(mp_obj_t self_in) {
    mp_obj_str_it_t *self = self_in;
    GET_STR_DATA_LEN(self->str, str, len);
    if (self->cur < len) {
        mp_obj_t o_out = mp_obj_new_str((const char*)str + self->cur, 1, true);
        self->cur += 1;
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
#endif

STATIC mp_obj_t bytes_it_iternext(mp_obj_t self_in) {
    mp_obj_str_it_t *self = self_in;
    GET_STR_DATA_LEN(self->str, str, len);
    if (self->cur < len) {
        mp_obj_t o_out = MP_OBJ_NEW_SMALL_INT(str[self->cur]);
        self->cur += 1;
        return o_out;
    } else {
        return MP_OBJ_STOP_ITERATION;
    }
}

STATIC const mp_obj_type_t mp_type_bytes_it = {
    { &mp_type_type },
    .name = MP_QSTR_iterator,
    .getiter = mp_identity,
    .iternext = bytes_it_iternext,
};

mp_obj_t mp_obj_new_bytes_iterator(mp_obj_t str) {
    mp_obj_str_it_t *o = m_new_obj(mp_obj_str_it_t);
    o->base.type = &mp_type_bytes_it;
    o->str = str;
    o->cur = 0;
    return o;
}
