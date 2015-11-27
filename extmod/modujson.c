/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
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

#include "py/nlr.h"
#include "py/objlist.h"
#include "py/parsenum.h"
#include "py/runtime.h"

#if MICROPY_PY_UJSON

STATIC mp_obj_t mod_ujson_dumps(mp_obj_t obj) {
    vstr_t vstr;
    mp_print_t print;
    vstr_init_print(&vstr, 8, &print);
    mp_obj_print_helper(&print, obj, PRINT_JSON);
    return mp_obj_new_str_from_vstr(&mp_type_str, &vstr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_ujson_dumps_obj, mod_ujson_dumps);

// This function implements a simple non-recursive JSON parser.
//
// The JSON specification is at http://www.ietf.org/rfc/rfc4627.txt
// The parser here will parse any valid JSON and return the correct
// corresponding Python object.  It allows through a superset of JSON, since
// it treats commas and colons as "whitespace", and doesn't care if
// brackets/braces are correctly paired.  It will raise a ValueError if the
// input is outside it's specs.
//
// Most of the work is parsing the primitives (null, false, true, numbers,
// strings).  It does 1 pass over the input string and so is easily extended to
// being able to parse from a non-seekable stream.  It tries to be fast and
// small in code size, while not using more RAM than necessary.
STATIC mp_obj_t mod_ujson_loads(mp_obj_t obj) {
    mp_uint_t len;
    const char *s = mp_obj_str_get_data(obj, &len);
    const char *top = s + len;
    vstr_t vstr;
    vstr_init(&vstr, 8);
    mp_obj_list_t stack; // we use a list as a simple stack for nested JSON
    stack.len = 0;
    stack.items = NULL;
    mp_obj_t stack_top = MP_OBJ_NULL;
    mp_obj_type_t *stack_top_type = NULL;
    mp_obj_t stack_key = MP_OBJ_NULL;
    for (;;) {
        cont:
        if (s == top) {
            break;
        }
        mp_obj_t next = MP_OBJ_NULL;
        bool enter = false;
        switch (*s) {
            case ',':
            case ':':
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                s += 1;
                goto cont;
            case 'n':
                if (s + 3 < top && s[1] == 'u' && s[2] == 'l' && s[3] == 'l') {
                    s += 4;
                    next = mp_const_none;
                } else {
                    goto fail;
                }
                break;
            case 'f':
                if (s + 4 < top && s[1] == 'a' && s[2] == 'l' && s[3] == 's' && s[4] == 'e') {
                    s += 5;
                    next = mp_const_false;
                } else {
                    goto fail;
                }
                break;
            case 't':
                if (s + 3 < top && s[1] == 'r' && s[2] == 'u' && s[3] == 'e') {
                    s += 4;
                    next = mp_const_true;
                } else {
                    goto fail;
                }
                break;
            case '"':
                vstr_reset(&vstr);
                for (s++; s < top && *s != '"';) {
                    byte c = *s;
                    if (c == '\\') {
                        s++;
                        c = *s;
                        switch (c) {
                            case 'b': c = 0x08; break;
                            case 'f': c = 0x0c; break;
                            case 'n': c = 0x0a; break;
                            case 'r': c = 0x0d; break;
                            case 't': c = 0x09; break;
                            case 'u': {
                                if (s + 4 >= top) { goto fail; }
                                mp_uint_t num = 0;
                                for (int i = 0; i < 4; i++) {
                                    c = (*++s | 0x20) - '0';
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
                    s++;
                }
                if (s == top) {
                    goto fail;
                }
                s++;
                next = mp_obj_new_str(vstr.buf, vstr.len, false);
                break;
            case '-':
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
                bool flt = false;
                vstr_reset(&vstr);
                for (; s < top; s++) {
                    if (*s == '.' || *s == 'E' || *s == 'e') {
                        flt = true;
                    } else if (*s == '-' || unichar_isdigit(*s)) {
                        // pass
                    } else {
                        break;
                    }
                    vstr_add_byte(&vstr, *s);
                }
                if (flt) {
                    next = mp_parse_num_decimal(vstr.buf, vstr.len, false, false, NULL);
                } else {
                    next = mp_parse_num_integer(vstr.buf, vstr.len, 10, NULL);
                }
                break;
            }
            case '[':
                next = mp_obj_new_list(0, NULL);
                enter = true;
                s += 1;
                break;
            case '{':
                next = mp_obj_new_dict(0);
                enter = true;
                s += 1;
                break;
            case '}':
            case ']': {
                s += 1;
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
    while (s < top && unichar_isspace(*s)) {
        s++;
    }
    if (s < top) {
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
    nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "syntax error in JSON"));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_ujson_loads_obj, mod_ujson_loads);

STATIC const mp_rom_map_elem_t mp_module_ujson_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ujson) },
    { MP_ROM_QSTR(MP_QSTR_dumps), MP_ROM_PTR(&mod_ujson_dumps_obj) },
    { MP_ROM_QSTR(MP_QSTR_loads), MP_ROM_PTR(&mod_ujson_loads_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_ujson_globals, mp_module_ujson_globals_table);

const mp_obj_module_t mp_module_ujson = {
    .base = { &mp_type_module },
    .name = MP_QSTR_ujson,
    .globals = (mp_obj_dict_t*)&mp_module_ujson_globals,
};

#endif //MICROPY_PY_UJSON
