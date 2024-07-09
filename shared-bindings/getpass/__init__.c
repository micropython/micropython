// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
//
// SPDX-License-Identifier: MIT

#include "py/stream.h"
#include "shared-module/getpass/__init__.h"

//| """Getpass Module
//|
//| This module provides a way to get input from user without echoing it.
//|
//| """
//| ...
//|

//| def getpass(prompt: Optional[str] = "Password: ", stream: Optional[io.FileIO] = None) -> str:
//|     """Prompt the user without echoing.
//|
//|     :param str prompt: The user is prompted using the string ``prompt``, which defaults to ``'Password: '``.
//|     :param io.FileIO stream: The ``prompt`` is written to the file-like object ``stream`` if provided.
//|
//|     """
//|     ...
//|
static mp_obj_t getpass_getpass(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_prompt, ARG_stream };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_prompt, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_stream, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const char *prompt = (args[ARG_prompt].u_obj == mp_const_none) ? "Password: " : mp_obj_str_get_str(args[ARG_prompt].u_obj);

    mp_print_t print = {.data = NULL};
    if (args[ARG_stream].u_obj != mp_const_none) {
        #if MICROPY_PY_IO && MICROPY_PY_SYS_STDFILES
        mp_get_stream_raise(args[ARG_stream].u_obj, MP_STREAM_OP_WRITE);
        print.data = MP_OBJ_TO_PTR(args[ARG_stream].u_obj);
        print.print_strn = mp_stream_write_adaptor;
        #else
        mp_raise_NotImplementedError(MP_ERROR_TEXT("stream operation not supported"));
        #endif
    }

    return shared_module_getpass_getpass(prompt, ((print.data) ? &print : NULL));
}
static MP_DEFINE_CONST_FUN_OBJ_KW(getpass_getpass_obj, 0, getpass_getpass);

static const mp_rom_map_elem_t getpass_module_globals_table[] = {
    // module name
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_getpass) },
    // module functions
    { MP_ROM_QSTR(MP_QSTR_getpass), MP_ROM_PTR(&getpass_getpass_obj) },
};
static MP_DEFINE_CONST_DICT(getpass_module_globals, getpass_module_globals_table);

const mp_obj_module_t getpass_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&getpass_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_getpass, getpass_module);
