#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime0.h"
#include "runtime.h"
#include "pfenv.h"

typedef struct _mp_obj_str_t {
    mp_obj_base_t base;
    machine_uint_t hash : 16; // XXX here we assume the hash size is 16 bits (it is at the moment; see qstr.c)
    machine_uint_t len : 16; // len == number of bytes used in data, alloc = len + 1 because (at the moment) we also append a null byte
    const byte *data;
} mp_obj_str_t;

STATIC mp_obj_t str_modulo_format(mp_obj_t pattern, uint n_args, const mp_obj_t *args);
const mp_obj_t mp_const_empty_bytes;

// use this macro to extract the string hash
#define GET_STR_HASH(str_obj_in, str_hash) uint str_hash; if (MP_OBJ_IS_QSTR(str_obj_in)) { str_hash = qstr_hash(MP_OBJ_QSTR_VALUE(str_obj_in)); } else { str_hash = ((mp_obj_str_t*)str_obj_in)->hash; }

// use this macro to extract the string length
#define GET_STR_LEN(str_obj_in, str_len) uint str_len; if (MP_OBJ_IS_QSTR(str_obj_in)) { str_len = qstr_len(MP_OBJ_QSTR_VALUE(str_obj_in)); } else { str_len = ((mp_obj_str_t*)str_obj_in)->len; }

// use this macro to extract the string data and length
#define GET_STR_DATA_LEN(str_obj_in, str_data, str_len) const byte *str_data; uint str_len; if (MP_OBJ_IS_QSTR(str_obj_in)) { str_data = qstr_data(MP_OBJ_QSTR_VALUE(str_obj_in), &str_len); } else { str_len = ((mp_obj_str_t*)str_obj_in)->len; str_data = ((mp_obj_str_t*)str_obj_in)->data; }

STATIC mp_obj_t mp_obj_new_str_iterator(mp_obj_t str);
STATIC mp_obj_t mp_obj_new_bytes_iterator(mp_obj_t str);
STATIC mp_obj_t str_new(const mp_obj_type_t *type, const byte* data, uint len);

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
    bool is_bytes = MP_OBJ_IS_TYPE(self_in, &mp_type_bytes);
    if (kind == PRINT_STR && !is_bytes) {
        print(env, "%.*s", str_len, str_data);
    } else {
        if (is_bytes) {
            print(env, "b");
        }
        mp_str_print_quoted(print, env, str_data, str_len);
    }
}

STATIC mp_obj_t str_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    switch (n_args) {
        case 0:
            return MP_OBJ_NEW_QSTR(MP_QSTR_);

        case 1:
        {
            vstr_t *vstr = vstr_new();
            mp_obj_print_helper((void (*)(void*, const char*, ...))vstr_printf, vstr, args[0], PRINT_STR);
            mp_obj_t s = mp_obj_new_str((byte*)vstr->buf, vstr->len, false);
            vstr_free(vstr);
            return s;
        }

        case 2:
        case 3:
        {
            // TODO: validate 2nd/3rd args
            if (!MP_OBJ_IS_TYPE(args[0], &mp_type_bytes)) {
                nlr_jump(mp_obj_new_exception_msg(&mp_type_TypeError, "bytes expected"));
            }
            GET_STR_DATA_LEN(args[0], str_data, str_len);
            GET_STR_HASH(args[0], str_hash);
            mp_obj_str_t *o = str_new(&mp_type_str, NULL, str_len);
            o->data = str_data;
            o->hash = str_hash;
            return o;
        }

        default:
            nlr_jump(mp_obj_new_exception_msg(&mp_type_TypeError, "str takes at most 3 arguments"));
    }
}

STATIC mp_obj_t bytes_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    if (n_args == 0) {
        return mp_const_empty_bytes;
    }

    if (MP_OBJ_IS_STR(args[0])) {
        if (n_args < 2 || n_args > 3) {
            goto wrong_args;
        }
        GET_STR_DATA_LEN(args[0], str_data, str_len);
        GET_STR_HASH(args[0], str_hash);
        mp_obj_str_t *o = str_new(&mp_type_bytes, NULL, str_len);
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
    while ((item = mp_iternext(iterable)) != MP_OBJ_NULL) {
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
        nlr_jump(mp_obj_new_exception_msg(&mp_type_TypeError, "wrong number of arguments"));
}

// like strstr but with specified length and allows \0 bytes
// TODO replace with something more efficient/standard
STATIC const byte *find_subbytes(const byte *haystack, machine_uint_t hlen, const byte *needle, machine_uint_t nlen, machine_int_t direction) {
    if (hlen >= nlen) {
        machine_uint_t str_index, str_index_end;
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

STATIC mp_obj_t str_binary_op(int op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    GET_STR_DATA_LEN(lhs_in, lhs_data, lhs_len);
    switch (op) {
        case MP_BINARY_OP_SUBSCR:
            // TODO: need predicate to check for int-like type (bools are such for example)
            // ["no", "yes"][1 == 2] is common idiom
            if (MP_OBJ_IS_SMALL_INT(rhs_in)) {
                uint index = mp_get_index(mp_obj_get_type(lhs_in), lhs_len, rhs_in, false);
                if (MP_OBJ_IS_TYPE(lhs_in, &mp_type_bytes)) {
                    return MP_OBJ_NEW_SMALL_INT((mp_small_int_t)lhs_data[index]);
                } else {
                    return mp_obj_new_str(lhs_data + index, 1, true);
                }
#if MICROPY_ENABLE_SLICE
            } else if (MP_OBJ_IS_TYPE(rhs_in, &mp_type_slice)) {
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

        case MP_BINARY_OP_ADD:
        case MP_BINARY_OP_INPLACE_ADD:
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

        case MP_BINARY_OP_IN:
            /* NOTE `a in b` is `b.__contains__(a)` */
            if (MP_OBJ_IS_STR(rhs_in)) {
                GET_STR_DATA_LEN(rhs_in, rhs_data, rhs_len);
                return MP_BOOL(find_subbytes(lhs_data, lhs_len, rhs_data, rhs_len, 1) != NULL);
            }
            break;

        case MP_BINARY_OP_MULTIPLY:
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

        case MP_BINARY_OP_MODULO: {
            mp_obj_t *args;
            uint n_args;
            if (MP_OBJ_IS_TYPE(rhs_in, &mp_type_tuple)) {
                // TODO: Support tuple subclasses?
                mp_obj_tuple_get(rhs_in, &n_args, &args);
            } else {
                args = &rhs_in;
                n_args = 1;
            }
            return str_modulo_format(lhs_in, n_args, args);
        }

        // These 2 are never passed here, dealt with as a special case in mp_binary_op().
        //case MP_BINARY_OP_EQUAL:
        //case MP_BINARY_OP_NOT_EQUAL:
        case MP_BINARY_OP_LESS:
        case MP_BINARY_OP_LESS_EQUAL:
        case MP_BINARY_OP_MORE:
        case MP_BINARY_OP_MORE_EQUAL:
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
    if (MP_OBJ_IS_TYPE(arg, &mp_type_tuple)) {
        mp_obj_tuple_get(arg, &seq_len, &seq_items);
    } else if (MP_OBJ_IS_TYPE(arg, &mp_type_list)) {
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
        mp_obj_list_append(res, mp_obj_new_str(start, s - start, false));
        if (s >= top) {
            break;
        }
        while (s < top && is_ws(*s)) s++;
        if (splits > 0) {
            splits--;
        }
    }

    if (s < top) {
        mp_obj_list_append(res, mp_obj_new_str(s, top - s, false));
    }

    return res;
}

STATIC mp_obj_t str_finder(uint n_args, const mp_obj_t *args, machine_int_t direction) {
    assert(2 <= n_args && n_args <= 4);
    assert(MP_OBJ_IS_STR(args[0]));
    assert(MP_OBJ_IS_STR(args[1]));

    GET_STR_DATA_LEN(args[0], haystack, haystack_len);
    GET_STR_DATA_LEN(args[1], needle, needle_len);

    machine_uint_t start = 0;
    machine_uint_t end = haystack_len;
    if (n_args >= 3 && args[2] != mp_const_none) {
        start = mp_get_index(&mp_type_str, haystack_len, args[2], true);
    }
    if (n_args >= 4 && args[3] != mp_const_none) {
        end = mp_get_index(&mp_type_str, haystack_len, args[3], true);
    }

    const byte *p = find_subbytes(haystack + start, end - start, needle, needle_len, direction);
    if (p == NULL) {
        // not found
        return MP_OBJ_NEW_SMALL_INT(-1);
    } else {
        // found
        return MP_OBJ_NEW_SMALL_INT(p - haystack);
    }
}

STATIC mp_obj_t str_find(uint n_args, const mp_obj_t *args) {
    return str_finder(n_args, args, 1);
}

STATIC mp_obj_t str_rfind(uint n_args, const mp_obj_t *args) {
    return str_finder(n_args, args, -1);
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

    machine_uint_t first_good_char_pos = 0;
    bool first_good_char_pos_set = false;
    machine_uint_t last_good_char_pos = 0;
    for (machine_uint_t i = 0; i < orig_str_len; i++) {
        if (find_subbytes(chars_to_del, chars_to_del_len, &orig_str[i], 1, 1) == NULL) {
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
    machine_uint_t stripped_len = last_good_char_pos - first_good_char_pos + 1;
    return mp_obj_new_str(orig_str + first_good_char_pos, stripped_len, false);
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
#if MICROPY_ENABLE_FLOAT
        || MP_OBJ_IS_TYPE(arg, &mp_type_float)
#endif
    ;
}

mp_obj_t str_format(uint n_args, const mp_obj_t *args) {
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
            nlr_jump(mp_obj_new_exception_msg(&mp_type_ValueError, "Single '}' encountered in format string"));
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
                nlr_jump(mp_obj_new_exception_msg(&mp_type_ValueError, "end of format while looking for conversion specifier"));
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
            nlr_jump(mp_obj_new_exception_msg(&mp_type_ValueError, "unmatched '{' in format"));
        }
        if (*str != '}') {
            nlr_jump(mp_obj_new_exception_msg(&mp_type_ValueError, "expected ':' after format specifier"));
        }

        mp_obj_t arg = mp_const_none;

        if (field_name) {
            if (arg_i > 0) {
                nlr_jump(mp_obj_new_exception_msg(&mp_type_ValueError, "cannot switch from automatic field numbering to manual field specification"));
            }
            int index;
            if (str_to_int(vstr_str(field_name), &index) != vstr_len(field_name) - 1) {
                nlr_jump(mp_obj_new_exception_msg(&mp_type_KeyError, "attributes not supported yet"));
            }
            if (index >= n_args - 1) {
                nlr_jump(mp_obj_new_exception_msg(&mp_type_IndexError, "tuple index out of range"));
            }
            arg = args[index + 1];
            arg_i = -1;
            vstr_free(field_name);
            field_name = NULL;
        } else {
            if (arg_i < 0) {
                nlr_jump(mp_obj_new_exception_msg(&mp_type_ValueError, "cannot switch from manual field specification to automatic field numbering"));
            }
            if (arg_i >= n_args - 1) {
                nlr_jump(mp_obj_new_exception_msg(&mp_type_IndexError, "tuple index out of range"));
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
                nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Unknown conversion specifier %c", conversion));
            }
            vstr_t *arg_vstr = vstr_new();
            mp_obj_print_helper((void (*)(void*, const char*, ...))vstr_printf, arg_vstr, arg, print_kind);
            arg = mp_obj_new_str((const byte *)vstr_str(arg_vstr), vstr_len(arg_vstr), false);
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
                nlr_jump(mp_obj_new_exception_msg(&mp_type_KeyError, "Invalid conversion specification"));
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
                nlr_jump(mp_obj_new_exception_msg(&mp_type_ValueError, "Sign not allowed in string format specifier"));
            }
            if (type == 'c') {
                nlr_jump(mp_obj_new_exception_msg(&mp_type_ValueError, "Sign not allowed with integer format specifier 'c'"));
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
                    pfenv_print_int(&pfenv_vstr, mp_obj_get_int(arg), 1, 2, 'a', flags, fill, width);
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
                    pfenv_print_int(&pfenv_vstr, mp_obj_get_int(arg), 1, 10, 'a', flags, fill, width);
                    continue;

                case 'o':
                    pfenv_print_int(&pfenv_vstr, mp_obj_get_int(arg), 1, 8, 'a', flags, fill, width);
                    continue;

                case 'x':
                    pfenv_print_int(&pfenv_vstr, mp_obj_get_int(arg), 1, 16, 'a', flags, fill, width);
                    continue;

                case 'X':
                    pfenv_print_int(&pfenv_vstr, mp_obj_get_int(arg), 1, 16, 'A', flags, fill, width);
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
                    nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                        "Unknown format code '%c' for object of type '%s'", type, mp_obj_get_type_str(arg)));
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
#if MICROPY_ENABLE_FLOAT
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
                    nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                        "Unknown format code '%c' for object of type 'float'",
                        type, mp_obj_get_type_str(arg)));
            }
        } else {
            // arg doesn't look like a number

            if (align == '=') {
                nlr_jump(mp_obj_new_exception_msg(&mp_type_ValueError, "'=' alignment not allowed in string format specifier"));
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
                    nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                        "Unknown format code '%c' for object of type 'str'",
                        type, mp_obj_get_type_str(arg)));
            }
        }
    }

    mp_obj_t s = mp_obj_new_str((byte*)vstr->buf, vstr->len, false);
    vstr_free(vstr);
    return s;
}

STATIC mp_obj_t str_modulo_format(mp_obj_t pattern, uint n_args, const mp_obj_t *args) {
    assert(MP_OBJ_IS_STR(pattern));

    GET_STR_DATA_LEN(pattern, str, len);
    int arg_i = 0;
    vstr_t *vstr = vstr_new();
    for (const byte *top = str + len; str < top; str++) {
        if (*str == '%') {
            if (++str >= top) {
                break;
            }
            if (*str == '%') {
                vstr_add_char(vstr, '%');
            } else {
                if (arg_i >= n_args) {
                    nlr_jump(mp_obj_new_exception_msg(&mp_type_TypeError, "not enough arguments for format string"));
                }
                switch (*str) {
                    case 's':
                        mp_obj_print_helper((void (*)(void*, const char*, ...))vstr_printf, vstr, args[arg_i], PRINT_STR);
                        break;
                    case 'r':
                        mp_obj_print_helper((void (*)(void*, const char*, ...))vstr_printf, vstr, args[arg_i], PRINT_REPR);
                        break;
                }
                arg_i++;
            }
        } else {
            vstr_add_char(vstr, *str);
        }
    }

    if (arg_i != n_args) {
        nlr_jump(mp_obj_new_exception_msg(&mp_type_TypeError, "not all arguments converted during string formatting"));
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
        while ((old_occurrence = find_subbytes(offset_ptr, str_len - offset_num, old, old_len, 1)) != NULL) {
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

STATIC mp_obj_t str_count(uint n_args, const mp_obj_t *args) {
    assert(2 <= n_args && n_args <= 4);
    assert(MP_OBJ_IS_STR(args[0]));
    assert(MP_OBJ_IS_STR(args[1]));

    GET_STR_DATA_LEN(args[0], haystack, haystack_len);
    GET_STR_DATA_LEN(args[1], needle, needle_len);

    machine_uint_t start = 0;
    machine_uint_t end = haystack_len;
    if (n_args >= 3 && args[2] != mp_const_none) {
        start = mp_get_index(&mp_type_str, haystack_len, args[2], true);
    }
    if (n_args >= 4 && args[3] != mp_const_none) {
        end = mp_get_index(&mp_type_str, haystack_len, args[3], true);
    }

    // if needle_len is zero then we count each gap between characters as an occurrence
    if (needle_len == 0) {
        return MP_OBJ_NEW_SMALL_INT(end - start + 1);
    }

    // count the occurrences
    machine_int_t num_occurrences = 0;
    for (machine_uint_t haystack_index = start; haystack_index + needle_len <= end; haystack_index++) {
        if (memcmp(&haystack[haystack_index], needle, needle_len) == 0) {
            num_occurrences++;
            haystack_index += needle_len - 1;
        }
    }

    return MP_OBJ_NEW_SMALL_INT(num_occurrences);
}

STATIC mp_obj_t str_partitioner(mp_obj_t self_in, mp_obj_t arg, machine_int_t direction) {
    assert(MP_OBJ_IS_STR(self_in));
    if (!MP_OBJ_IS_STR(arg)) {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError,
                                               "Can't convert '%s' object to str implicitly", mp_obj_get_type_str(arg)));
    }

    GET_STR_DATA_LEN(self_in, str, str_len);
    GET_STR_DATA_LEN(arg, sep, sep_len);

    if (sep_len == 0) {
        nlr_jump(mp_obj_new_exception_msg(&mp_type_ValueError, "empty separator"));
    }

    mp_obj_t result[] = {MP_OBJ_NEW_QSTR(MP_QSTR_), MP_OBJ_NEW_QSTR(MP_QSTR_), MP_OBJ_NEW_QSTR(MP_QSTR_)};

    if (direction > 0) {
        result[0] = self_in;
    } else {
        result[2] = self_in;
    }

    const byte *position_ptr = find_subbytes(str, str_len, sep, sep_len, direction);
    if (position_ptr != NULL) {
        machine_uint_t position = position_ptr - str;
        result[0] = mp_obj_new_str(str, position, false);
        result[1] = arg;
        result[2] = mp_obj_new_str(str + position + sep_len, str_len - position - sep_len, false);
    }

    return mp_obj_new_tuple(3, result);
}

STATIC mp_obj_t str_partition(mp_obj_t self_in, mp_obj_t arg) {
    return str_partitioner(self_in, arg, 1);
}

STATIC mp_obj_t str_rpartition(mp_obj_t self_in, mp_obj_t arg) {
    return str_partitioner(self_in, arg, -1);
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
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_rfind_obj, 2, 4, str_rfind);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(str_join_obj, str_join);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_split_obj, 1, 3, str_split);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(str_startswith_obj, str_startswith);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_strip_obj, 1, 2, str_strip);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(str_format_obj, 1, str_format);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_replace_obj, 3, 4, str_replace);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(str_count_obj, 2, 4, str_count);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(str_partition_obj, str_partition);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(str_rpartition_obj, str_rpartition);

STATIC const mp_map_elem_t str_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_find), (mp_obj_t)&str_find_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_rfind), (mp_obj_t)&str_rfind_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_join), (mp_obj_t)&str_join_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_split), (mp_obj_t)&str_split_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_startswith), (mp_obj_t)&str_startswith_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_strip), (mp_obj_t)&str_strip_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_format), (mp_obj_t)&str_format_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_replace), (mp_obj_t)&str_replace_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_count), (mp_obj_t)&str_count_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_partition), (mp_obj_t)&str_partition_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_rpartition), (mp_obj_t)&str_rpartition_obj },
};

STATIC MP_DEFINE_CONST_DICT(str_locals_dict, str_locals_dict_table);

const mp_obj_type_t mp_type_str = {
    { &mp_type_type },
    .name = MP_QSTR_str,
    .print = str_print,
    .make_new = str_make_new,
    .binary_op = str_binary_op,
    .getiter = mp_obj_new_str_iterator,
    .buffer_p = { .get_buffer = str_get_buffer },
    .locals_dict = (mp_obj_t)&str_locals_dict,
};

// Reuses most of methods from str
const mp_obj_type_t mp_type_bytes = {
    { &mp_type_type },
    .name = MP_QSTR_bytes,
    .print = str_print,
    .make_new = bytes_make_new,
    .binary_op = str_binary_op,
    .getiter = mp_obj_new_bytes_iterator,
    .locals_dict = (mp_obj_t)&str_locals_dict,
};

// the zero-length bytes
STATIC const mp_obj_str_t empty_bytes_obj = {{&mp_type_bytes}, 0, 0, NULL};
const mp_obj_t mp_const_empty_bytes = (mp_obj_t)&empty_bytes_obj;

mp_obj_t mp_obj_str_builder_start(const mp_obj_type_t *type, uint len, byte **data) {
    mp_obj_str_t *o = m_new_obj(mp_obj_str_t);
    o->base.type = type;
    o->len = len;
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

STATIC mp_obj_t str_new(const mp_obj_type_t *type, const byte* data, uint len) {
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
        return str_new(&mp_type_str, data, len);
    }
}

mp_obj_t mp_obj_new_bytes(const byte* data, uint len) {
    return str_new(&mp_type_bytes, data, len);
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
        return MP_OBJ_NULL;
    }
}

STATIC const mp_obj_type_t mp_type_str_it = {
    { &mp_type_type },
    .name = MP_QSTR_iterator,
    .getiter = mp_identity,
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
        return MP_OBJ_NULL;
    }
}

STATIC const mp_obj_type_t mp_type_bytes_it = {
    { &mp_type_type },
    .name = MP_QSTR_iterator,
    .getiter = mp_identity,
    .iternext = bytes_it_iternext,
};

mp_obj_t mp_obj_new_str_iterator(mp_obj_t str) {
    mp_obj_str_it_t *o = m_new_obj(mp_obj_str_it_t);
    o->base.type = &mp_type_str_it;
    o->str = str;
    o->cur = 0;
    return o;
}

mp_obj_t mp_obj_new_bytes_iterator(mp_obj_t str) {
    mp_obj_str_it_t *o = m_new_obj(mp_obj_str_it_t);
    o->base.type = &mp_type_bytes_it;
    o->str = str;
    o->cur = 0;
    return o;
}
