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

static mp_obj_t mp_obj_new_str_iterator(mp_obj_t str, int cur);

/******************************************************************************/
/* str                                                                        */

void str_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    GET_STR_DATA_LEN(self_in, str_data, str_len);
    if (kind == PRINT_STR) {
        print(env, "%.*s", str_len, str_data);
    } else {
        // TODO need to escape chars etc
        print(env, "'%.*s'", str_len, str_data);
    }
}

// like strstr but with specified length and allows \0 bytes
// TODO replace with something more efficient/standard
static const byte *find_subbytes(const byte *haystack, uint hlen, const byte *needle, uint nlen) {
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

mp_obj_t str_binary_op(int op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    GET_STR_DATA_LEN(lhs_in, lhs_data, lhs_len);
    switch (op) {
        case RT_BINARY_OP_SUBSCR:
            // TODO: need predicate to check for int-like type (bools are such for example)
            // ["no", "yes"][1 == 2] is common idiom
            if (MP_OBJ_IS_SMALL_INT(rhs_in)) {
                uint index = mp_get_index(mp_obj_get_type(lhs_in), lhs_len, rhs_in);
                return mp_obj_new_str(lhs_data + index, 1, true);
#if MICROPY_ENABLE_SLICE
            } else if (MP_OBJ_IS_TYPE(rhs_in, &slice_type)) {
                machine_int_t start, stop, step;
                mp_obj_slice_get(rhs_in, &start, &stop, &step);
                assert(step == 1);
                if (start < 0) {
                    start = lhs_len + start;
                    if (start < 0) {
                        start = 0;
                    }
                } else if (start > lhs_len) {
                    start = lhs_len;
                }
                if (stop <= 0) {
                    stop = lhs_len + stop;
                    // CPython returns empty string in such case
                    if (stop < 0) {
                        stop = start;
                    }
                } else if (stop > lhs_len) {
                    stop = lhs_len;
                }
                return mp_obj_new_str(lhs_data + start, stop - start, false);
#endif
            } else {
                // Message doesn't match CPython, but we don't have so much bytes as they
                // to spend them on verbose wording
                nlr_jump(mp_obj_new_exception_msg(MP_QSTR_TypeError, "index must be int"));
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
                mp_obj_t s = mp_obj_str_builder_start(alloc_len, &data);
                memcpy(data, lhs_data, lhs_len);
                memcpy(data + lhs_len, rhs_data, rhs_len);
                return mp_obj_str_builder_end(s);
            }
            break;

        case RT_COMPARE_OP_IN:
        case RT_COMPARE_OP_NOT_IN:
            /* NOTE `a in b` is `b.__contains__(a)` */
            if (MP_OBJ_IS_STR(rhs_in)) {
                GET_STR_DATA_LEN(rhs_in, rhs_data, rhs_len);
                return MP_BOOL((op == RT_COMPARE_OP_IN) ^ (find_subbytes(lhs_data, lhs_len, rhs_data, rhs_len) == NULL));
            }
            break;

        case RT_BINARY_OP_MULTIPLY:
        {
            if (!MP_OBJ_IS_SMALL_INT(rhs_in)) {
                return NULL;
            }
            int n = MP_OBJ_SMALL_INT_VALUE(rhs_in);
            byte *data;
            mp_obj_t s = mp_obj_str_builder_start(lhs_len * n, &data);
            mp_seq_multiply(lhs_data, sizeof(*lhs_data), lhs_len, n, data);
            return mp_obj_str_builder_end(s);
        }
    }

    return MP_OBJ_NULL; // op not supported
}

static mp_obj_t str_getiter(mp_obj_t o_in) {
    return mp_obj_new_str_iterator(o_in, 0);
}

mp_obj_t str_join(mp_obj_t self_in, mp_obj_t arg) {
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
    mp_obj_t joined_str = mp_obj_str_builder_start(required_len, &data);
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
    nlr_jump(mp_obj_new_exception_msg(MP_QSTR_TypeError, "?str.join expecting a list of str's"));
}

#define is_ws(c) ((c) == ' ' || (c) == '\t')

static mp_obj_t str_split(uint n_args, const mp_obj_t *args) {
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

static mp_obj_t str_find(uint n_args, const mp_obj_t *args) {
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
static mp_obj_t str_startswith(mp_obj_t self_in, mp_obj_t arg) {
    GET_STR_DATA_LEN(self_in, str, str_len);
    GET_STR_DATA_LEN(arg, prefix, prefix_len);
    if (prefix_len > str_len) {
        return mp_const_false;
    }
    return MP_BOOL(memcmp(str, prefix, prefix_len) == 0);
}

static bool chr_in_str(const byte* const str, const size_t str_len, int c) {
    for (size_t i = 0; i < str_len; i++) {
        if (str[i] == c) {
            return true;
        }
    }
    return false;
}

mp_obj_t str_strip(uint n_args, const mp_obj_t *args) {
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
                    nlr_jump(mp_obj_new_exception_msg(MP_QSTR_IndexError, "tuple index out of range"));
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

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_find_obj, 2, 4, str_find);
static MP_DEFINE_CONST_FUN_OBJ_2(str_join_obj, str_join);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_split_obj, 1, 3, str_split);
static MP_DEFINE_CONST_FUN_OBJ_2(str_startswith_obj, str_startswith);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_strip_obj, 1, 2, str_strip);
static MP_DEFINE_CONST_FUN_OBJ_VAR(str_format_obj, 1, str_format);

static const mp_method_t str_type_methods[] = {
    { "find", &str_find_obj },
    { "join", &str_join_obj },
    { "split", &str_split_obj },
    { "startswith", &str_startswith_obj },
    { "strip", &str_strip_obj },
    { "format", &str_format_obj },
    { NULL, NULL }, // end-of-list sentinel
};

const mp_obj_type_t str_type = {
    { &mp_const_type },
    "str",
    .print = str_print,
    .binary_op = str_binary_op,
    .getiter = str_getiter,
    .methods = str_type_methods,
};

mp_obj_t mp_obj_str_builder_start(uint len, byte **data) {
    mp_obj_str_t *o = m_new_obj_var(mp_obj_str_t, byte, len + 1);
    o->base.type = &str_type;
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
        mp_obj_str_t *o = m_new_obj_var(mp_obj_str_t, byte, len + 1);
        o->base.type = &str_type;
        o->hash = qstr_compute_hash(data, len);
        o->len = len;
        memcpy(o->data, data, len * sizeof(byte));
        o->data[len] = '\0'; // for now we add null for compatibility with C ASCIIZ strings
        return o;
    }
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
        return strncmp((const char*)d1, (const char*)d2, l1) == 0;
    }
}

uint mp_obj_str_get_hash(mp_obj_t self_in) {
    if (MP_OBJ_IS_STR(self_in)) {
        GET_STR_HASH(self_in, h);
        return h;
    } else {
        nlr_jump(mp_obj_new_exception_msg_varg(MP_QSTR_TypeError, "Can't convert '%s' object to str implicitly",
                 mp_obj_get_type_str(self_in)));
    }
}

uint mp_obj_str_get_len(mp_obj_t self_in) {
    if (MP_OBJ_IS_STR(self_in)) {
        GET_STR_LEN(self_in, l);
        return l;
    } else {
        nlr_jump(mp_obj_new_exception_msg_varg(MP_QSTR_TypeError, "Can't convert '%s' object to str implicitly",
                 mp_obj_get_type_str(self_in)));
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
        nlr_jump(mp_obj_new_exception_msg_varg(MP_QSTR_TypeError, "Can't convert '%s' object to str implicitly",
                 mp_obj_get_type_str(self_in)));
    }
}

const byte *mp_obj_str_get_data(mp_obj_t self_in, uint *len) {
    if (MP_OBJ_IS_STR(self_in)) {
        GET_STR_DATA_LEN(self_in, s, l);
        *len = l;
        return s;
    } else {
        nlr_jump(mp_obj_new_exception_msg_varg(MP_QSTR_TypeError, "Can't convert '%s' object to str implicitly",
                 mp_obj_get_type_str(self_in)));
    }
}

/******************************************************************************/
/* str iterator                                                               */

typedef struct _mp_obj_str_it_t {
    mp_obj_base_t base;
    mp_obj_t str;
    machine_uint_t cur;
} mp_obj_str_it_t;

mp_obj_t str_it_iternext(mp_obj_t self_in) {
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

static const mp_obj_type_t str_it_type = {
    { &mp_const_type },
    "str_iterator",
    .iternext = str_it_iternext,
};

mp_obj_t mp_obj_new_str_iterator(mp_obj_t str, int cur) {
    mp_obj_str_it_t *o = m_new_obj(mp_obj_str_it_t);
    o->base.type = &str_it_type;
    o->str = str;
    o->cur = cur;
    return o;
}
