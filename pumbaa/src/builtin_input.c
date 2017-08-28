/**
 * @file builtin_input.c
 *
 * @section License
 * Copyright (C) 2016, Erik Moqvist
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * This file is part of the Pumbaa project.
 */

#include "pumbaa.h"

/**
 * The builtin input() function. Read a line from standard input.
 *
 * input([prompt])
 */
static mp_obj_t builtin_input(size_t n_args, const mp_obj_t *args_p)
{
    vstr_t line;
    int res;

    if (n_args == 1) {
        mp_obj_print(args_p[0], PRINT_STR);
    }

    vstr_init(&line, 16);
    res = readline(&line, "");

    if ((line.len == 0) && (res == CHAR_CTRL_D)) {
        nlr_raise(mp_obj_new_exception(&mp_type_EOFError));
    }

    return (mp_obj_new_str_from_vstr(&mp_type_str, &line));
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_input_obj, 0, 1, builtin_input);
