/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2016 Damien P. George
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

#include "py/builtin.h"

#if MICROPY_PY_BUILTINS_HELP

const char *mp_help_default_text =
"Welcome to MicroPython!\n"
"\n"
"For online docs please visit http://docs.micropython.org/\n"
"\n"
"Control commands:\n"
"  CTRL-A        -- on a blank line, enter raw REPL mode\n"
"  CTRL-B        -- on a blank line, enter normal REPL mode\n"
"  CTRL-C        -- interrupt a running program\n"
"  CTRL-D        -- on a blank line, exit or do a soft reset\n"
"  CTRL-E        -- on a blank line, enter paste mode\n"
"\n"
"For further help on a specific object, type help(obj)\n"
;

STATIC void mp_help_print_info_about_object(mp_obj_t name_o, mp_obj_t value) {
    mp_print_str(MP_PYTHON_PRINTER, "  ");
    mp_obj_print(name_o, PRINT_STR);
    mp_print_str(MP_PYTHON_PRINTER, " -- ");
    mp_obj_print(value, PRINT_STR);
    mp_print_str(MP_PYTHON_PRINTER, "\n");
}

STATIC void mp_help_print_obj(const mp_obj_t obj) {
    // try to print something sensible about the given object
    mp_print_str(MP_PYTHON_PRINTER, "object ");
    mp_obj_print(obj, PRINT_STR);
    mp_printf(MP_PYTHON_PRINTER, " is of type %s\n", mp_obj_get_type_str(obj));

    mp_map_t *map = NULL;
    if (MP_OBJ_IS_TYPE(obj, &mp_type_module)) {
        map = mp_obj_dict_get_map(mp_obj_module_get_globals(obj));
    } else {
        mp_obj_type_t *type;
        if (MP_OBJ_IS_TYPE(obj, &mp_type_type)) {
            type = obj;
        } else {
            type = mp_obj_get_type(obj);
        }
        if (type->locals_dict != MP_OBJ_NULL && MP_OBJ_IS_TYPE(type->locals_dict, &mp_type_dict)) {
            map = mp_obj_dict_get_map(type->locals_dict);
        }
    }
    if (map != NULL) {
        for (uint i = 0; i < map->alloc; i++) {
            if (map->table[i].key != MP_OBJ_NULL) {
                mp_help_print_info_about_object(map->table[i].key, map->table[i].value);
            }
        }
    }
}

STATIC mp_obj_t mp_builtin_help(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        // print a general help message
        mp_print_str(MP_PYTHON_PRINTER, MICROPY_PY_BUILTINS_HELP_TEXT);
    } else {
        // try to print something sensible about the given object
        mp_help_print_obj(args[0]);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_help_obj, 0, 1, mp_builtin_help);

#endif // MICROPY_PY_BUILTINS_HELP
