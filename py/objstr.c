#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime0.h"
#include "runtime.h"

typedef struct _mp_obj_str_t {
    mp_obj_base_t base;
    machine_uint_t hash : 16; // XXX here we assume the hash size is 16 bits (it is at the moment; see qstr.c)
    machine_uint_t len : 16; // len == number of bytes used in data, alloc = len + 1 because (at the moment) we also append a null byte
    byte data[];
} mp_obj_str_t;

// use this macro to extract the string hash
#define GET_STR_HASH(str_obj_in, str_hash) uint str_hash; if (MP_OBJ_IS_QSTR(str_obj_in)) { str_hash = qstr_hash(MP_OBJ_QSTR_VALUE(str_obj_in)); } else { str_hash = ((mp_obj_str_t*)str_obj_in)->hash; }

// use this macro to extract the string length
#define GET_STR_LEN(str_obj_in, str_len) uint str_len; if (MP_OBJ_IS_QSTR(str_obj_in)) { str_len = qstr_len(MP_OBJ_QSTR_VALUE(str_obj_in)); } else { str_len = ((mp_obj_str_t*)str_obj_in)->len; }

// use this macro to extract the string data and length
#define GET_STR_DATA_LEN(str_obj_in, str_data, str_len) const byte *str_data; uint str_len; if (MP_OBJ_IS_QSTR(str_obj_in)) { str_data = qstr_data(MP_OBJ_QSTR_VALUE(str_obj_in), &str_len); } else { str_len = ((mp_obj_str_t*)str_obj_in)->len; str_data = ((mp_obj_str_t*)str_obj_in)->data; }

STATIC mp_obj_t mp_obj_new_str_iterator(mp_obj_t str);
STATIC mp_obj_t mp_obj_new_bytes_iterator(mp_obj_t str);

/******************************************************************************/
/* str                                                                        */

void mp_str_print_quoted(void (*print)(void *env, const char *fmt, ...), void *env, const byte *str_data, uint str_len) {
    // this escapes characters, but it will be very slow to print (calling print many times)
    bool has_single_quote = false;
    bool has_double_quote = false;
    for (const byte *s = str_data, *top = str_data + str_len; (!has_single_quote || !has_double_quote) && s < top; s++) {
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
        } else if (32 <= *s && *s <= 126) {
            print(env, "%c", *s);
        } else if (*s == '\n') {
            print(env, "\\n");
        // TODO add more escape codes here if we want to match CPython
        } else {
            print(env, "\\x%02x", *s);
        }
    }
    print(env, "%c", quote_char);
}

STATIC void str_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    GET_STR_DATA_LEN(self_in, str_data, str_len);
    bool is_bytes = MP_OBJ_IS_TYPE(self_in, &bytes_type);
    if (kind == PRINT_STR && !is_bytes) {
        print(env, "%.*s", str_len, str_data);
    } else {
        if (is_bytes) {
            print(env, "b");
        }
        mp_str_print_quoted(print, env, str_data, str_len);
    }
}

// like strstr but with specified length and allows \0 bytes
// TODO replace with something more efficient/standard
STATIC const byte *find_subbytes(const byte *haystack, uint hlen, const byte *needle, uint nlen) {
    if (hlen >= nlen) {
        for (uint i = 0; i <= hlen - nlen; i++) {
            bool found = true;
            for (uint j = 0; j < nlen; j++) {
                if (haystack[i + j] != needle[j]) {
                    found = false;
                    break;
                }
            }
            if (found) {
                return haystack + i;
            }
        }
    }
    return NULL;
}

STATIC mp_obj_t str_binary_op(int op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    GET_STR_DATA_LEN(lhs_in, lhs_data, lhs_len);
    switch (op) {
        case RT_BINARY_OP_SUBSCR:
            // TODO: need predicate to check for int-like type (bools are such for example)
            // ["no", "yes"][1 == 2] is common idiom
            if (MP_OBJ_IS_SMALL_INT(rhs_in)) {
                uint index = mp_get_index(mp_obj_get_type(lhs_in), lhs_len, rhs_in);
                if (MP_OBJ_IS_TYPE(lhs_in, &bytes_type)) {
                    return MP_OBJ_NEW_SMALL_INT((mp_small_int_t)lhs_data[index]);
                } else {
                    return mp_obj_new_str(lhs_data + index, 1, true);
                }
#if MICROPY_ENABLE_SLICE
            } else if (MP_OBJ_IS_TYPE(rhs_in, &slice_type)) {
                machine_uint_t start, stop;
                if (!m_seq_get_fast_slice_indexes(lhs_len, rhs_in, &start, &stop)) {
                    assert(0);
                }
                return mp_obj_new_str(lhs_data + start, stop - start, false);
#endif
            } else {
                // Message doesn't match CPython, but we don't have so much bytes as they
                // to spend them on verbose wording
                nlr_jump(mp_obj_new_exception_msg(&mp_type_TypeError, "index must be int"));
            }

        case RT_BINARY_OP_ADD:
        case RT_BINARY_OP_INPLACE_ADD:
            if (MP_OBJ_IS_STR(rhs_in)) {
                // add 2 strings

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
                mp_obj_t s = mp_obj_str_builder_start(mp_obj_get_type(lhs_in), alloc_len, &data);
                memcpy(data, lhs_data, lhs_len);
                memcpy(data + lhs_len, rhs_data, rhs_len);
                return mp_obj_str_builder_end(s);
            }
            break;

        case RT_BINARY_OP_IN:
            /* NOTE `a in b` is `b.__contains__(a)` */
            if (MP_OBJ_IS_STR(rhs_in)) {
                GET_STR_DATA_LEN(rhs_in, rhs_data, rhs_len);
                return MP_BOOL(find_subbytes(lhs_data, lhs_len, rhs_data, rhs_len) != NULL);
            }
            break;

        case RT_BINARY_OP_MULTIPLY:
        {
            if (!MP_OBJ_IS_SMALL_INT(rhs_in)) {
                return NULL;
            }
            int n = MP_OBJ_SMALL_INT_VALUE(rhs_in);
            byte *data;
            mp_obj_t s = mp_obj_str_builder_start(mp_obj_get_type(lhs_in), lhs_len * n, &data);
            mp_seq_multiply(lhs_data, sizeof(*lhs_data), lhs_len, n, data);
            return mp_obj_str_builder_end(s);
        }

        // These 2 are never passed here, dealt with as a special case in rt_binary_op().
        //case RT_BINARY_OP_EQUAL:
        //case RT_BINARY_OP_NOT_EQUAL:
        case RT_BINARY_OP_LESS:
        case RT_BINARY_OP_LESS_EQUAL:
        case RT_BINARY_OP_MORE:
        case RT_BINARY_OP_MORE_EQUAL:
            if (MP_OBJ_IS_STR(rhs_in)) {
                GET_STR_DATA_LEN(rhs_in, rhs_data, rhs_len);
                return MP_BOOL(mp_seq_cmp_bytes(op, lhs_data, lhs_len, rhs_data, rhs_len));
            }
    }

    return MP_OBJ_NULL; // op not supported
}

STATIC mp_obj_t str_join(mp_obj_t self_in, mp_obj_t arg) {
    assert(MP_OBJ_IS_STR(self_in));

    // get separation string
    GET_STR_DATA_LEN(self_in, sep_str, sep_len);

    // process args
    uint seq_len;
    mp_obj_t *seq_items;
    if (MP_OBJ_IS_TYPE(arg, &tuple_type)) {
        mp_obj_tuple_get(arg, &seq_len, &seq_items);
    } else if (MP_OBJ_IS_TYPE(arg, &list_type)) {
        mp_obj_list_get(arg, &seq_len, &seq_items);
    } else {
        goto bad_arg;
    }

    // count required length
    int required_len = 0;
    for (int i = 0; i < seq_len; i++) {
        if (!MP_OBJ_IS_STR(seq_items[i])) {
            goto bad_arg;
        }
        if (i > 0) {
            required_len += sep_len;
        }
        GET_STR_LEN(seq_items[i], l);
        required_len += l;
    }

    // make joined string
    byte *data;
    mp_obj_t joined_str = mp_obj_str_builder_start(mp_obj_get_type(self_in), required_len, &data);
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

bad_arg:
    nlr_jump(mp_obj_new_exception_msg(&mp_type_TypeError, "?str.join expecting a list of str's"));
}

#define is_ws(c) ((c) == ' ' || (c) == '\t')

STATIC mp_obj_t str_split(uint n_args, const mp_obj_t *args) {
    int splits = -1;
    mp_obj_t sep = mp_const_none;
    if (n_args > 1) {
        sep = args[1];
        if (n_args > 2) {
            splits = MP_OBJ_SMALL_INT_VALUE(args[2]);
        }
    }
    assert(sep == mp_const_none);
    (void)sep; // unused; to hush compiler warning
    mp_obj_t res = mp_obj_new_list(0, NULL);
    GET_STR_DATA_LEN(args[0], s, len);
    const byte *top = s + len;
    const byte *start;

    // Initial whitespace is not counted as split, so we pre-do it
    while (s < top && is_ws(*s)) s++;
    while (s < top && splits != 0) {
        start = s;
        while (s < top && !is_ws(*s)) s++;
        rt_list_append(res, mp_obj_new_str(start, s - start, false));
        if (s >= top) {
            break;
        }
        while (s < top && is_ws(*s)) s++;
        if (splits > 0) {
            splits--;
        }
    }

    if (s < top) {
        rt_list_append(res, mp_obj_new_str(s, top - s, false));
    }

    return res;
}

STATIC mp_obj_t str_find(uint n_args, const mp_obj_t *args) {
    assert(2 <= n_args && n_args <= 4);
    assert(MP_OBJ_IS_STR(args[0]));
    assert(MP_OBJ_IS_STR(args[1]));

    GET_STR_DATA_LEN(args[0], haystack, haystack_len);
    GET_STR_DATA_LEN(args[1], needle, needle_len);

    size_t start = 0;
    size_t end = haystack_len;
    /* TODO use a non-exception-throwing mp_get_index */
    if (n_args >= 3 && args[2] != mp_const_none) {
        start = mp_get_index(&str_type, haystack_len, args[2]);
    }
    if (n_args >= 4 && args[3] != mp_const_none) {
        end = mp_get_index(&str_type, haystack_len, args[3]);
    }

    const byte *p = find_subbytes(haystack + start, haystack_len - start, needle, needle_len);
    if (p == NULL) {
        // not found
        return MP_OBJ_NEW_SMALL_INT(-1);
    } else {
        // found
        machine_int_t pos = p - haystack;
        if (pos + needle_len > end) {
            pos = -1;
        }
        return MP_OBJ_NEW_SMALL_INT(pos);
    }
}

// TODO: (Much) more variety in args
STATIC mp_obj_t str_startswith(mp_obj_t self_in, mp_obj_t arg) {
    GET_STR_DATA_LEN(self_in, str, str_len);
    GET_STR_DATA_LEN(arg, prefix, prefix_len);
    if (prefix_len > str_len) {
        return mp_const_false;
    }
    return MP_BOOL(memcmp(str, prefix, prefix_len) == 0);
}

STATIC bool chr_in_str(const byte* const str, const size_t str_len, int c) {
    for (size_t i = 0; i < str_len; i++) {
        if (str[i] == c) {
            return true;
        }
    }
    return false;
}

STATIC mp_obj_t str_strip(uint n_args, const mp_obj_t *args) {
    assert(1 <= n_args && n_args <= 2);
    assert(MP_OBJ_IS_STR(args[0]));

    const byte *chars_to_del;
    uint chars_to_del_len;
    static const byte whitespace[] = " \t\n\r\v\f";

    if (n_args == 1) {
        chars_to_del = whitespace;
        chars_to_del_len = sizeof(whitespace);
    } else {
        assert(MP_OBJ_IS_STR(args[1]));
        GET_STR_DATA_LEN(args[1], s, l);
        chars_to_del = s;
        chars_to_del_len = l;
    }

    GET_STR_DATA_LEN(args[0], orig_str, orig_str_len);

    size_t first_good_char_pos = 0;
    bool first_good_char_pos_set = false;
    size_t last_good_char_pos = 0;
    for (size_t i = 0; i < orig_str_len; i++) {
        if (!chr_in_str(chars_to_del, chars_to_del_len, orig_str[i])) {
            last_good_char_pos = i;
            if (!first_good_char_pos_set) {
                first_good_char_pos = i;
                first_good_char_pos_set = true;
            }
        }
    }

    if (first_good_char_pos == 0 && last_good_char_pos == 0) {
        // string is all whitespace, return ''
        return MP_OBJ_NEW_QSTR(MP_QSTR_);
    }

    assert(last_good_char_pos >= first_good_char_pos);
    //+1 to accomodate the last character
    size_t stripped_len = last_good_char_pos - first_good_char_pos + 1;
    return mp_obj_new_str(orig_str + first_good_char_pos, stripped_len, false);
}

mp_obj_t str_format(uint n_args, const mp_obj_t *args) {
    assert(MP_OBJ_IS_STR(args[0]));

    GET_STR_DATA_LEN(args[0], str, len);
    int arg_i = 1;
    vstr_t *vstr = vstr_new();
    for (const byte *top = str + len; str < top; str++) {
        if (*str == '{') {
            str++;
            if (str < top && *str == '{') {
                vstr_add_char(vstr, '{');
            } else {
                while (str < top && *str != '}') str++;
                if (arg_i >= n_args) {
                    nlr_jump(mp_obj_new_exception_msg(&mp_type_IndexError, "tuple index out of range"));
                }
                // TODO: may be PRINT_REPR depending on formatting code
                mp_obj_print_helper((void (*)(void*, const char*, ...))vstr_printf, vstr, args[arg_i], PRINT_STR);
                arg_i++;
            }
        } else {
            vstr_add_char(vstr, *str);
        }
    }

    mp_obj_t s = mp_obj_new_str((byte*)vstr->buf, vstr->len, false);
    vstr_free(vstr);
    return s;
}

STATIC mp_obj_t str_replace(uint n_args, const mp_obj_t *args) {
    assert(MP_OBJ_IS_STR(args[0]));
    assert(MP_OBJ_IS_STR(args[1]));
    assert(MP_OBJ_IS_STR(args[2]));

    machine_int_t max_rep = 0;
    if (n_args == 4) {
        assert(MP_OBJ_IS_SMALL_INT(args[3]));
        max_rep = MP_OBJ_SMALL_INT_VALUE(args[3]);
        if (max_rep == 0) {
            return args[0];
        } else if (max_rep < 0) {
            max_rep = 0;
        }
    }

    // if max_rep is still 0 by this point we will need to do all possible replacements

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
        machine_uint_t replaced_str_index = 0;
        machine_uint_t num_replacements_done = 0;
        const byte *old_occurrence;
        const byte *offset_ptr = str;
        machine_uint_t offset_num = 0;
        while ((old_occurrence = find_subbytes(offset_ptr, str_len - offset_num, old, old_len)) != NULL) {
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
            offset_num = offset_ptr - str;

            num_replacements_done++;
            if (max_rep != 0 && num_replacements_done == max_rep){
                break;
            }
        }

        // copy from just after end of last occurrence of to-be-replaced string to end of old string
        if (data != NULL) {
            memcpy(data + replaced_str_index, offset_ptr, str_len - offset_num);
        }
        replaced_str_index += str_len - offset_num;

        if (data == NULL) {
            // first pass
            if (num_replacements_done == 0) {
                // no substr found, return original string
                return args[0];
            } else {
                // substr found, allocate new string
                replaced_str = mp_obj_str_builder_start(mp_obj_get_type(args[0]), replaced_str_index, &data);
            }
        } else {
            // second pass, we are done
            break;
        }
    }

    return mp_obj_str_builder_end(replaced_str);
}

STATIC machine_int_t str_get_buffer(mp_obj_t self_in, buffer_info_t *bufinfo, int flags) {
    if (flags == BUFFER_READ) {
        GET_STR_DATA_LEN(self_in, str_data, str_len);
        bufinfo->buf = (void*)str_data;
        bufinfo->len = str_len;
        return 0;
    } else {
        // can't write to a string
        bufinfo->buf = NULL;
        bufinfo->len = 0;
        return 1;
    }
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_find_obj, 2, 4, str_find);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(str_join_obj, str_join);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_split_obj, 1, 3, str_split);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(str_startswith_obj, str_startswith);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_strip_obj, 1, 2, str_strip);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(str_format_obj, 1, str_format);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_replace_obj, 3, 4, str_replace);

STATIC const mp_method_t str_type_methods[] = {
    { "find", &str_find_obj },
    { "join", &str_join_obj },
    { "split", &str_split_obj },
    { "startswith", &str_startswith_obj },
    { "strip", &str_strip_obj },
    { "format", &str_format_obj },
    { "replace", &str_replace_obj },
    { NULL, NULL }, // end-of-list sentinel
};

const mp_obj_type_t str_type = {
    { &mp_type_type },
    .name = MP_QSTR_str,
    .print = str_print,
    .binary_op = str_binary_op,
    .getiter = mp_obj_new_str_iterator,
    .methods = str_type_methods,
    .buffer_p = { .get_buffer = str_get_buffer },
};

// Reuses most of methods from str
const mp_obj_type_t bytes_type = {
    { &mp_type_type },
    .name = MP_QSTR_bytes,
    .print = str_print,
    .binary_op = str_binary_op,
    .getiter = mp_obj_new_bytes_iterator,
    .methods = str_type_methods,
};

mp_obj_t mp_obj_str_builder_start(const mp_obj_type_t *type, uint len, byte **data) {
    mp_obj_str_t *o = m_new_obj_var(mp_obj_str_t, byte, len + 1);
    o->base.type = type;
    o->len = len;
    *data = o->data;
    return o;
}

mp_obj_t mp_obj_str_builder_end(mp_obj_t o_in) {
    assert(MP_OBJ_IS_STR(o_in));
    mp_obj_str_t *o = o_in;
    o->hash = qstr_compute_hash(o->data, o->len);
    o->data[o->len] = '\0'; // for now we add null for compatibility with C ASCIIZ strings
    return o;
}

STATIC mp_obj_t str_new(const mp_obj_type_t *type, const byte* data, uint len) {
    mp_obj_str_t *o = m_new_obj_var(mp_obj_str_t, byte, len + 1);
    o->base.type = type;
    o->hash = qstr_compute_hash(data, len);
    o->len = len;
    memcpy(o->data, data, len * sizeof(byte));
    o->data[len] = '\0'; // for now we add null for compatibility with C ASCIIZ strings
    return o;
}

mp_obj_t mp_obj_new_str(const byte* data, uint len, bool make_qstr_if_not_already) {
    qstr q = qstr_find_strn(data, len);
    if (q != MP_QSTR_NULL) {
        // qstr with this data already exists
        return MP_OBJ_NEW_QSTR(q);
    } else if (make_qstr_if_not_already) {
        // no existing qstr, make a new one
        return MP_OBJ_NEW_QSTR(qstr_from_strn((const char*)data, len));
    } else {
        // no existing qstr, don't make one
        return str_new(&str_type, data, len);
    }
}

mp_obj_t mp_obj_new_bytes(const byte* data, uint len) {
    return str_new(&bytes_type, data, len);
}

bool mp_obj_str_equal(mp_obj_t s1, mp_obj_t s2) {
    if (MP_OBJ_IS_QSTR(s1) && MP_OBJ_IS_QSTR(s2)) {
        return s1 == s2;
    } else {
        GET_STR_HASH(s1, h1);
        GET_STR_HASH(s2, h2);
        if (h1 != h2) {
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

void bad_implicit_conversion(mp_obj_t self_in) __attribute__((noreturn));
void bad_implicit_conversion(mp_obj_t self_in) {
    nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "Can't convert '%s' object to str implicitly", mp_obj_get_type_str(self_in)));
}

uint mp_obj_str_get_hash(mp_obj_t self_in) {
    if (MP_OBJ_IS_STR(self_in)) {
        GET_STR_HASH(self_in, h);
        return h;
    } else {
        bad_implicit_conversion(self_in);
    }
}

uint mp_obj_str_get_len(mp_obj_t self_in) {
    if (MP_OBJ_IS_STR(self_in)) {
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
    } else if (MP_OBJ_IS_TYPE(self_in, &str_type)) {
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
    if (MP_OBJ_IS_STR(self_in)) {
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
    machine_uint_t cur;
} mp_obj_str_it_t;

STATIC mp_obj_t str_it_iternext(mp_obj_t self_in) {
    mp_obj_str_it_t *self = self_in;
    GET_STR_DATA_LEN(self->str, str, len);
    if (self->cur < len) {
        mp_obj_t o_out = mp_obj_new_str(str + self->cur, 1, true);
        self->cur += 1;
        return o_out;
    } else {
        return mp_const_stop_iteration;
    }
}

STATIC const mp_obj_type_t str_it_type = {
    { &mp_type_type },
    .name = MP_QSTR_iterator,
    .iternext = str_it_iternext,
};

STATIC mp_obj_t bytes_it_iternext(mp_obj_t self_in) {
    mp_obj_str_it_t *self = self_in;
    GET_STR_DATA_LEN(self->str, str, len);
    if (self->cur < len) {
        mp_obj_t o_out = MP_OBJ_NEW_SMALL_INT((mp_small_int_t)str[self->cur]);
        self->cur += 1;
        return o_out;
    } else {
        return mp_const_stop_iteration;
    }
}

STATIC const mp_obj_type_t bytes_it_type = {
    { &mp_type_type },
    .name = MP_QSTR_iterator,
    .iternext = bytes_it_iternext,
};

mp_obj_t mp_obj_new_str_iterator(mp_obj_t str) {
    mp_obj_str_it_t *o = m_new_obj(mp_obj_str_it_t);
    o->base.type = &str_it_type;
    o->str = str;
    o->cur = 0;
    return o;
}

mp_obj_t mp_obj_new_bytes_iterator(mp_obj_t str) {
    mp_obj_str_it_t *o = m_new_obj(mp_obj_str_it_t);
    o->base.type = &bytes_it_type;
    o->str = str;
    o->cur = 0;
    return o;
}
