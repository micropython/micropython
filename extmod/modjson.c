/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-2019 Damien P. George
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

#include <stdio.h>

#include "py/objlist.h"
#include "py/objstringio.h"
#include "py/parsenum.h"
#include "py/runtime.h"
#include "py/stream.h"

#if MICROPY_PY_JSON

#if MICROPY_PY_JSON_SEPARATORS

enum {
    DUMP_MODE_TO_STRING = 1,
    DUMP_MODE_TO_STREAM = 2,
};

static mp_obj_t mod_json_dump_helper(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args, unsigned int mode) {
    enum { ARG_separators };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_separators, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - mode, pos_args + mode, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_print_ext_t print_ext;

    if (args[ARG_separators].u_obj == mp_const_none) {
        print_ext.item_separator = ", ";
        print_ext.key_separator = ": ";
    } else {
        mp_obj_t *items;
        mp_obj_get_array_fixed_n(args[ARG_separators].u_obj, 2, &items);
        print_ext.item_separator = mp_obj_str_get_str(items[0]);
        print_ext.key_separator = mp_obj_str_get_str(items[1]);
    }

    if (mode == DUMP_MODE_TO_STRING) {
        // dumps(obj)
        vstr_t vstr;
        vstr_init_print(&vstr, 8, &print_ext.base);
        mp_obj_print_helper(&print_ext.base, pos_args[0], PRINT_JSON);
        return mp_obj_new_str_from_utf8_vstr(&vstr);
    } else {
        // dump(obj, stream)
        print_ext.base.data = MP_OBJ_TO_PTR(pos_args[1]);
        print_ext.base.print_strn = mp_stream_write_adaptor;
        mp_get_stream_raise(pos_args[1], MP_STREAM_OP_WRITE);
        mp_obj_print_helper(&print_ext.base, pos_args[0], PRINT_JSON);
        return mp_const_none;
    }
}

static mp_obj_t mod_json_dump(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mod_json_dump_helper(n_args, pos_args, kw_args, DUMP_MODE_TO_STREAM);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(mod_json_dump_obj, 2, mod_json_dump);

static mp_obj_t mod_json_dumps(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mod_json_dump_helper(n_args, pos_args, kw_args, DUMP_MODE_TO_STRING);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(mod_json_dumps_obj, 1, mod_json_dumps);

#else

static mp_obj_t mod_json_dump(mp_obj_t obj, mp_obj_t stream) {
    mp_get_stream_raise(stream, MP_STREAM_OP_WRITE);
    mp_print_t print = {MP_OBJ_TO_PTR(stream), mp_stream_write_adaptor};
    mp_obj_print_helper(&print, obj, PRINT_JSON);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(mod_json_dump_obj, mod_json_dump);

static mp_obj_t mod_json_dumps(mp_obj_t obj) {
    vstr_t vstr;
    mp_print_t print;
    vstr_init_print(&vstr, 8, &print);
    mp_obj_print_helper(&print, obj, PRINT_JSON);
    return mp_obj_new_str_from_utf8_vstr(&vstr);
}
static MP_DEFINE_CONST_FUN_OBJ_1(mod_json_dumps_obj, mod_json_dumps);

#endif

// The function below implements a simple non-recursive JSON parser.
//
// The JSON specification is at http://www.ietf.org/rfc/rfc4627.txt
// The parser here will parse any valid JSON and return the correct
// corresponding Python object.  It allows through a superset of JSON, since
// it treats commas and colons as "whitespace", and doesn't care if
// brackets/braces are correctly paired.  It will raise a ValueError if the
// input is outside it's specs.
//
// Most of the work is parsing the primitives (null, false, true, numbers,
// strings).  It does 1 pass over the input stream.  It tries to be fast and
// small in code size, while not using more RAM than necessary.

typedef struct _json_stream_t {
    mp_obj_t stream_obj;
    mp_uint_t (*read)(mp_obj_t obj, void *buf, mp_uint_t size, int *errcode);
    int errcode;
    byte cur;
} json_stream_t;

#define S_EOF (0) // null is not allowed in json stream so is ok as EOF marker
#define S_END(s) ((s).cur == S_EOF)
#define S_CUR(s) ((s).cur)
#define S_NEXT(s) (json_stream_next(&(s)))

static byte json_stream_next(json_stream_t *s) {
    mp_uint_t ret = s->read(s->stream_obj, &s->cur, 1, &s->errcode);
    if (s->errcode != 0) {
        mp_raise_OSError(s->errcode);
    }
    if (ret == 0) {
        s->cur = S_EOF;
    }
    return s->cur;
}

static mp_obj_t mod_json_load(mp_obj_t stream_obj) {
    const mp_stream_p_t *stream_p = mp_get_stream_raise(stream_obj, MP_STREAM_OP_READ);
    json_stream_t s = {stream_obj, stream_p->read, 0, 0};
    vstr_t vstr;
    vstr_init(&vstr, 8);
    mp_obj_list_t stack; // we use a list as a simple stack for nested JSON
    stack.len = 0;
    stack.items = NULL;
    mp_obj_t stack_top = MP_OBJ_NULL;
    const mp_obj_type_t *stack_top_type = NULL;
    mp_obj_t stack_key = MP_OBJ_NULL;
    S_NEXT(s);
    for (;;) {
    cont:
        if (S_END(s)) {
            break;
        }
        mp_obj_t next = MP_OBJ_NULL;
        bool enter = false;
        byte cur = S_CUR(s);
        S_NEXT(s);
        switch (cur) {
            case ',':
            case ':':
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                goto cont;
            case 'n':
                if (S_CUR(s) == 'u' && S_NEXT(s) == 'l' && S_NEXT(s) == 'l') {
                    S_NEXT(s);
                    next = mp_const_none;
                } else {
                    goto fail;
                }
                break;
            case 'f':
                if (S_CUR(s) == 'a' && S_NEXT(s) == 'l' && S_NEXT(s) == 's' && S_NEXT(s) == 'e') {
                    S_NEXT(s);
                    next = mp_const_false;
                } else {
                    goto fail;
                }
                break;
            case 't':
                if (S_CUR(s) == 'r' && S_NEXT(s) == 'u' && S_NEXT(s) == 'e') {
                    S_NEXT(s);
                    next = mp_const_true;
                } else {
                    goto fail;
                }
                break;
            case '"':
                vstr_reset(&vstr);
                for (; !S_END(s) && S_CUR(s) != '"';) {
                    byte c = S_CUR(s);
                    if (c == '\\') {
                        c = S_NEXT(s);
                        switch (c) {
                            case 'b':
                                c = 0x08;
                                break;
                            case 'f':
                                c = 0x0c;
                                break;
                            case 'n':
                                c = 0x0a;
                                break;
                            case 'r':
                                c = 0x0d;
                                break;
                            case 't':
                                c = 0x09;
                                break;
                            case 'u': {
                                mp_uint_t num = 0;
                                for (int i = 0; i < 4; i++) {
                                    c = (S_NEXT(s) | 0x20) - '0';
                                    if (c > 9) {
                                        c -= ('a' - ('9' + 1));
                                    }
                                    num = (num << 4) | c;
                                }
                                vstr_add_char(&vstr, num);
                                goto str_cont;
                            }
                        }
                    }
                    vstr_add_byte(&vstr, c);
                str_cont:
                    S_NEXT(s);
                }
                if (S_END(s)) {
                    goto fail;
                }
                S_NEXT(s);
                next = mp_obj_new_str(vstr.buf, vstr.len);
                break;
            case '-':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9': {
                bool flt = false;
                vstr_reset(&vstr);
                for (;;) {
                    vstr_add_byte(&vstr, cur);
                    cur = S_CUR(s);
                    if (cur == '.' || cur == 'E' || cur == 'e') {
                        flt = true;
                    } else if (cur == '+' || cur == '-' || unichar_isdigit(cur)) {
                        // pass
                    } else {
                        break;
                    }
                    S_NEXT(s);
                }
                if (flt) {
                    next = mp_parse_num_float(vstr.buf, vstr.len, false, NULL);
                } else {
                    next = mp_parse_num_integer(vstr.buf, vstr.len, 10, NULL);
                }
                break;
            }
            case '[':
                next = mp_obj_new_list(0, NULL);
                enter = true;
                break;
            case '{':
                next = mp_obj_new_dict(0);
                enter = true;
                break;
            case '}':
            case ']': {
                if (stack_top == MP_OBJ_NULL) {
                    // no object at all
                    goto fail;
                }
                if (stack.len == 0) {
                    // finished; compound object
                    goto success;
                }
                stack.len -= 1;
                stack_top = stack.items[stack.len];
                stack_top_type = mp_obj_get_type(stack_top);
                goto cont;
            }
            default:
                goto fail;
        }
        if (stack_top == MP_OBJ_NULL) {
            stack_top = next;
            stack_top_type = mp_obj_get_type(stack_top);
            if (!enter) {
                // finished; single primitive only
                goto success;
            }
        } else {
            // append to list or dict
            if (stack_top_type == &mp_type_list) {
                mp_obj_list_append(stack_top, next);
            } else {
                if (stack_key == MP_OBJ_NULL) {
                    stack_key = next;
                    if (enter) {
                        goto fail;
                    }
                } else {
                    mp_obj_dict_store(stack_top, stack_key, next);
                    stack_key = MP_OBJ_NULL;
                }
            }
            if (enter) {
                if (stack.items == NULL) {
                    mp_obj_list_init(&stack, 1);
                    stack.items[0] = stack_top;
                } else {
                    mp_obj_list_append(MP_OBJ_FROM_PTR(&stack), stack_top);
                }
                stack_top = next;
                stack_top_type = mp_obj_get_type(stack_top);
            }
        }
    }
success:
    // eat trailing whitespace
    while (unichar_isspace(S_CUR(s))) {
        S_NEXT(s);
    }
    if (!S_END(s)) {
        // unexpected chars
        goto fail;
    }
    if (stack_top == MP_OBJ_NULL || stack.len != 0) {
        // not exactly 1 object
        goto fail;
    }
    vstr_clear(&vstr);
    return stack_top;

fail:
    mp_raise_ValueError(MP_ERROR_TEXT("syntax error in JSON"));
}
static MP_DEFINE_CONST_FUN_OBJ_1(mod_json_load_obj, mod_json_load);

static mp_obj_t mod_json_loads(mp_obj_t obj) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(obj, &bufinfo, MP_BUFFER_READ);
    vstr_t vstr = {bufinfo.len, bufinfo.len, (char *)bufinfo.buf, true};
    mp_obj_stringio_t sio = {{&mp_type_stringio}, &vstr, 0, MP_OBJ_NULL};
    return mod_json_load(MP_OBJ_FROM_PTR(&sio));
}
static MP_DEFINE_CONST_FUN_OBJ_1(mod_json_loads_obj, mod_json_loads);

static const mp_rom_map_elem_t mp_module_json_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_json) },
    { MP_ROM_QSTR(MP_QSTR_dump), MP_ROM_PTR(&mod_json_dump_obj) },
    { MP_ROM_QSTR(MP_QSTR_dumps), MP_ROM_PTR(&mod_json_dumps_obj) },
    { MP_ROM_QSTR(MP_QSTR_load), MP_ROM_PTR(&mod_json_load_obj) },
    { MP_ROM_QSTR(MP_QSTR_loads), MP_ROM_PTR(&mod_json_loads_obj) },
};

static MP_DEFINE_CONST_DICT(mp_module_json_globals, mp_module_json_globals_table);

const mp_obj_module_t mp_module_json = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_json_globals,
};

MP_REGISTER_EXTENSIBLE_MODULE(MP_QSTR_json, mp_module_json);

#endif // MICROPY_PY_JSON
