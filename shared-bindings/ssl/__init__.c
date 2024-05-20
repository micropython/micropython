// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/objexcept.h"
#include "py/objstr.h"
#include "py/parsenum.h"
#include "py/runtime.h"
#include "shared-bindings/ssl/__init__.h"
#include "shared-bindings/ssl/SSLContext.h"

//| """
//| The `ssl` module provides SSL contexts to wrap sockets in.
//|
//| |see_cpython_module| :mod:`cpython:ssl`.
//| """
//|

//| def create_default_context() -> ssl.SSLContext:
//|     """Return the default SSLContext."""
//|     ...
//|

static mp_obj_t ssl_create_default_context(void) {
    ssl_sslcontext_obj_t *s = mp_obj_malloc(ssl_sslcontext_obj_t, &ssl_sslcontext_type);

    common_hal_ssl_create_default_context(s);
    return s;
}
MP_DEFINE_CONST_FUN_OBJ_0(ssl_create_default_context_obj, ssl_create_default_context);

static const mp_rom_map_elem_t ssl_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ssl) },

    { MP_ROM_QSTR(MP_QSTR_create_default_context), MP_ROM_PTR(&ssl_create_default_context_obj) },

    { MP_ROM_QSTR(MP_QSTR_SSLContext), MP_ROM_PTR(&ssl_sslcontext_type) },
};

static MP_DEFINE_CONST_DICT(ssl_globals, ssl_globals_table);

const mp_obj_module_t ssl_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&ssl_globals,
};

MP_REGISTER_MODULE(MP_QSTR_ssl, ssl_module);
