/**
 * @file builtin_help.c
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
#include "lib/utils/pyhelp.h"

/**
 * The Pumbaa help text.
 */
static const FAR char *help_text_p =
    "Welcome to Pumbaa!\r\n"
    "\r\n"
    "For online docs please visit http://pumbaa.readthedocs.io/en/latest/.\r\n"
    "\r\n"
    "Control commands:\r\n"
    "  CTRL-E        -- on a blank line, enter paste mode\r\n"
    "\r\n"
    "For further help on a specific object, type help(obj)\r\n"
    ;

/**
 * The builtin help() function.
 *
 * help([object])
 */
static mp_obj_t builtin_help(size_t n_args, const mp_obj_t *args_p)
{
    if (n_args == 0) {
        std_printf(help_text_p);
    } else {
        pyhelp_print_obj(args_p[0]);
    }

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_help_obj, 0, 1, builtin_help);
