// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/mpstate.h"
#include "py/obj.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "shared-bindings/warnings/__init__.h"

//| """Warn about potential code issues.
//|
//| This is a slimmed down version of the full CPython module. It defaults to
//| the "always" action instead of "default", which prints once per occurrence.
//| Only "error" and "ignore" are also supported. No filtering on category is
//| available.
//|
//| |see_cpython_module| :mod:`cpython:warnings`.
//|
//| """
//|
//| import typing
//|

//| def warn(message: str, category: type = Warning) -> None:
//|     """Issue a warning with an optional category. Use `simplefilter()` to
//|     set if warnings are ignored, printed or raise an exception.
//|
//|     """
//|     ...
//|
static mp_obj_t warnings_warn(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_message, ARG_category };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_message, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_category,  MP_ARG_OBJ, {.u_obj = MP_OBJ_FROM_PTR(&mp_type_Warning)} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t category_obj = args[ARG_category].u_obj;
    if (!mp_obj_is_type(category_obj, &mp_type_type) ||
        !mp_obj_is_subclass_fast(category_obj, MP_OBJ_FROM_PTR(&mp_type_Warning))) {
        mp_raise_TypeError_varg(MP_ERROR_TEXT("%q must be a subclass of %q"), MP_QSTR_category, MP_QSTR_Warning);
    }

    common_hal_warnings_warn(mp_obj_str_get_str(args[ARG_message].u_obj), MP_OBJ_TO_PTR(category_obj));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(warnings_warn_obj, 1, warnings_warn);

//| def simplefilter(action: str) -> None:
//|     """Set the action to take on all warnings. This is a subset of the CPython
//|     behavior because it allows for per-category changes."""
//|     ...
//|
static mp_obj_t warnings_simplefilter(mp_obj_t action_in) {
    const char *action_str = mp_obj_str_get_str(action_in);
    warnings_action_t action;
    if (strcmp(action_str, "error") == 0) {
        action = WARNINGS_ERROR;
    } else if (strcmp(action_str, "ignore") == 0) {
        action = WARNINGS_IGNORE;
    } else if (strcmp(action_str, "always") == 0) {
        action = WARNINGS_ALWAYS;
    } else {
        mp_raise_ValueError(NULL);
    }
    common_hal_warnings_simplefilter(action);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(warnings_simplefilter_obj, warnings_simplefilter);

static const mp_rom_map_elem_t warnings_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_warnings) },

    { MP_ROM_QSTR(MP_QSTR_warn), MP_ROM_PTR(&warnings_warn_obj) },
    { MP_ROM_QSTR(MP_QSTR_simplefilter), MP_ROM_PTR(&warnings_simplefilter_obj) },
};

static MP_DEFINE_CONST_DICT(warnings_module_globals, warnings_module_globals_table);

const mp_obj_module_t warnings_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&warnings_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_warnings, warnings_module);
