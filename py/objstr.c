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
    qstr qstr;
} mp_obj_str_t;

static mp_obj_t mp_obj_new_str_iterator(mp_obj_str_t *str, int cur);

/******************************************************************************/
/* str                                                                        */

void mp_obj_str_print_qstr(void (*print)(void *env, const char *fmt, ...), void *env, qstr q, mp_print_kind_t kind) {
    if (kind == PRINT_STR) {
        print(env, "%s", qstr_str(q));
    } else {
        // TODO need to escape chars etc
        print(env, "'%s'", qstr_str(q));
    }
}

void str_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_obj_str_t *self = self_in;
    mp_obj_str_print_qstr(print, env, self->qstr, kind);
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
    mp_obj_str_t *lhs = lhs_in;
    uint lhs_len;
    const byte *lhs_data = qstr_data(lhs->qstr, &lhs_len);
    switch (op) {
        case RT_BINARY_OP_SUBSCR:
            // TODO: need predicate to check for int-like type (bools are such for example)
            // ["no", "yes"][1 == 2] is common idiom
            if (MP_OBJ_IS_SMALL_INT(rhs_in)) {
                // TODO: This implements byte string access for single index so far
                // TODO: Handle negative indexes.
                return mp_obj_new_int(lhs_data[mp_obj_get_int(rhs_in)]);
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
                return mp_obj_new_str(qstr_from_strn((const char*)lhs_data + start, stop - start));
#endif
            } else {
                // Message doesn't match CPython, but we don't have so much bytes as they
                // to spend them on verbose wording
                nlr_jump(mp_obj_new_exception_msg(MP_QSTR_TypeError, "index must be int"));
            }

        case RT_BINARY_OP_ADD:
        case RT_BINARY_OP_INPLACE_ADD:
            if (MP_OBJ_IS_TYPE(rhs_in, &str_type)) {
                // add 2 strings
                uint rhs_len;
                const byte *rhs_data = qstr_data(((mp_obj_str_t*)rhs_in)->qstr, &rhs_len);
                int alloc_len = lhs_len + rhs_len;
                byte *q_ptr;
                byte *val = qstr_build_start(alloc_len, &q_ptr);
                memcpy(val, lhs_data, lhs_len);
                memcpy(val + lhs_len, rhs_data, rhs_len);
                return mp_obj_new_str(qstr_build_end(q_ptr));
            }
            break;
        case RT_COMPARE_OP_IN:
        case RT_COMPARE_OP_NOT_IN:
            /* NOTE `a in b` is `b.__contains__(a)` */
            if (MP_OBJ_IS_TYPE(rhs_in, &str_type)) {
                uint rhs_len;
                const byte *rhs_data = qstr_data(((mp_obj_str_t*)rhs_in)->qstr, &rhs_len);
                return MP_BOOL((op == RT_COMPARE_OP_IN) ^ (find_subbytes(lhs_data, lhs_len, rhs_data, rhs_len) == NULL));
                return mp_const_false;
            }
            break;
    }

    return MP_OBJ_NULL; // op not supported
}

static mp_obj_t str_getiter(mp_obj_t o_in) {
    return mp_obj_new_str_iterator(o_in, 0);
}

mp_obj_t str_join(mp_obj_t self_in, mp_obj_t arg) {
    assert(MP_OBJ_IS_TYPE(self_in, &str_type));
    mp_obj_str_t *self = self_in;

    // get separation string
    const char *sep_str = qstr_str(self->qstr);
    size_t sep_len = strlen(sep_str);

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
        if (!MP_OBJ_IS_TYPE(seq_items[i], &str_type)) {
            goto bad_arg;
        }
        if (i > 0) {
            required_len += sep_len;
        }
        required_len += strlen(qstr_str(mp_obj_str_get(seq_items[i])));
    }

    // make joined string
    byte *q_ptr;
    byte *s_dest = qstr_build_start(required_len, &q_ptr);
    for (int i = 0; i < seq_len; i++) {
        if (i > 0) {
            memcpy(s_dest, sep_str, sep_len);
            s_dest += sep_len;
        }
        uint s2_len;
        const byte *s2 = qstr_data(mp_obj_str_get(seq_items[i]), &s2_len);
        memcpy(s_dest, s2, s2_len);
        s_dest += s2_len;
    }
    qstr q = qstr_build_end(q_ptr);

    // return joined string
    return mp_obj_new_str(q);

bad_arg:
    nlr_jump(mp_obj_new_exception_msg(MP_QSTR_TypeError, "?str.join expecting a list of str's"));
}

static bool chr_in_str(const char* const str, const size_t str_len, const char c) {
    for (size_t i = 0; i < str_len; i++) {
        if (str[i] == c) {
            return true;
        }
    }
    return false;
}

static mp_obj_t str_find(uint n_args, const mp_obj_t *args) {
    assert(2 <= n_args && n_args <= 4);
    assert(MP_OBJ_IS_TYPE(args[0], &str_type));
    if (!MP_OBJ_IS_TYPE(args[1], &str_type)) {
        nlr_jump(mp_obj_new_exception_msg_1_arg(
                     MP_QSTR_TypeError,
                     "Can't convert '%s' object to str implicitly",
                     mp_obj_get_type_str(args[1])));
    }

    const char* haystack = qstr_str(((mp_obj_str_t*)args[0])->qstr);
    const char* needle = qstr_str(((mp_obj_str_t*)args[1])->qstr);

    size_t haystack_len = strlen(haystack);
    size_t needle_len = strlen(needle);

    size_t start = 0;
    size_t end = haystack_len;
    /* TODO use a non-exception-throwing mp_get_index */
    if (n_args >= 3 && args[2] != mp_const_none) {
        start = mp_get_index(&str_type, haystack_len, args[2]);
    }
    if (n_args >= 4 && args[3] != mp_const_none) {
        end = mp_get_index(&str_type, haystack_len, args[3]);
    }

    char *p = strstr(haystack + start, needle);
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

mp_obj_t str_strip(uint n_args, const mp_obj_t *args) {
    assert(1 <= n_args && n_args <= 2);
    assert(MP_OBJ_IS_TYPE(args[0], &str_type));
    const char *chars_to_del;
    static const char whitespace[] = " \t\n\r\v\f";

    if (n_args == 1) {
        chars_to_del = whitespace;
    } else {
        assert(MP_OBJ_IS_TYPE(args[1], &str_type));
        mp_obj_str_t *chars_to_del_obj = args[1];
        chars_to_del = qstr_str(chars_to_del_obj->qstr);
    }

    const size_t chars_to_del_len = strlen(chars_to_del);
    mp_obj_str_t *self = args[0];
    const char *orig_str = qstr_str(self->qstr);
    const size_t orig_str_len = strlen(orig_str);

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
        //string is all whitespace, return ''
        return mp_obj_new_str(MP_QSTR_);
    }

    assert(last_good_char_pos >= first_good_char_pos);
    //+1 to accomodate the last character
    size_t stripped_len = last_good_char_pos - first_good_char_pos + 1;
    return mp_obj_new_str(qstr_from_strn(orig_str + first_good_char_pos, stripped_len));
}

mp_obj_t str_format(uint n_args, const mp_obj_t *args) {
    assert(MP_OBJ_IS_TYPE(args[0], &str_type));
    mp_obj_str_t *self = args[0];

    const char *str = qstr_str(self->qstr);
    int arg_i = 1;
    vstr_t *vstr = vstr_new();
    for (; *str; str++) {
        if (*str == '{') {
            str++;
            if (*str == '{') {
                vstr_add_char(vstr, '{');
            } else {
                while (*str != '}') str++;
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

    return mp_obj_new_str(qstr_from_strn_take(vstr->buf, vstr->alloc, vstr->len));
}

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_find_obj, 2, 4, str_find);
static MP_DEFINE_CONST_FUN_OBJ_2(str_join_obj, str_join);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_strip_obj, 1, 2, str_strip);
static MP_DEFINE_CONST_FUN_OBJ_VAR(str_format_obj, 1, str_format);

static const mp_method_t str_type_methods[] = {
    { "find", &str_find_obj },
    { "join", &str_join_obj },
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

mp_obj_t mp_obj_new_str(qstr qstr) {
    mp_obj_str_t *o = m_new_obj(mp_obj_str_t);
    o->base.type = &str_type;
    o->qstr = qstr;
    return o;
}

qstr mp_obj_str_get(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &str_type));
    mp_obj_str_t *self = self_in;
    return self->qstr;
}

/******************************************************************************/
/* str iterator                                                               */

typedef struct _mp_obj_str_it_t {
    mp_obj_base_t base;
    mp_obj_str_t *str;
    machine_uint_t cur;
} mp_obj_str_it_t;

mp_obj_t str_it_iternext(mp_obj_t self_in) {
    mp_obj_str_it_t *self = self_in;
    const char *str = qstr_str(self->str->qstr);
    if (self->cur < strlen(str)) {
        mp_obj_t o_out = mp_obj_new_str(qstr_from_strn(str + self->cur, 1));
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

mp_obj_t mp_obj_new_str_iterator(mp_obj_str_t *str, int cur) {
    mp_obj_str_it_t *o = m_new_obj(mp_obj_str_it_t);
    o->base.type = &str_it_type;
    o->str = str;
    o->cur = cur;
    return o;
}
