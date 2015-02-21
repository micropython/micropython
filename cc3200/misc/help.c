/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Daniel Campora
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

#include "py/mpconfig.h"
#include "py/obj.h"

STATIC const char help_text[] = "Welcome to Micro Python!\n"
                                "For online help please visit http://micropython.org/help/.\n"
                                "For further help on a specific object, type help(obj)\n";

STATIC void pyb_help_print_info_about_object(mp_obj_t name_o, mp_obj_t value) {
    printf("  ");
    mp_obj_print(name_o, PRINT_STR);
    printf(" -- ");
    mp_obj_print(value, PRINT_STR);
    printf("\n");
}

STATIC mp_obj_t pyb_help(uint n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        // print a general help message
        printf("%s", help_text);

    } else {
        // try to print something sensible about the given object

        printf("object ");
        mp_obj_print(args[0], PRINT_STR);
        printf(" is of type %s\n", mp_obj_get_type_str(args[0]));

        mp_map_t *map = NULL;
        if (MP_OBJ_IS_TYPE(args[0], &mp_type_module)) {
            map = mp_obj_dict_get_map(mp_obj_module_get_globals(args[0]));
        } else {
            mp_obj_type_t *type;
            if (MP_OBJ_IS_TYPE(args[0], &mp_type_type)) {
                type = args[0];
            } else {
                type = mp_obj_get_type(args[0]);
            }
            if (type->locals_dict != MP_OBJ_NULL && MP_OBJ_IS_TYPE(type->locals_dict, &mp_type_dict)) {
                map = mp_obj_dict_get_map(type->locals_dict);
            }
        }
        if (map != NULL) {
            for (uint i = 0; i < map->alloc; i++) {
                if (map->table[i].key != MP_OBJ_NULL) {
                    pyb_help_print_info_about_object(map->table[i].key, map->table[i].value);
                }
            }
        }
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_help_obj, 0, 1, pyb_help);
