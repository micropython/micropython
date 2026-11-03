// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/cryptography/hazmat/__init__.h"
#include "shared-bindings/cryptography/hazmat/primitives/__init__.h"

//| """Hazardous materials
//|
//| A subset of :py:mod:`cryptography.hazmat`, named for the real module's own
//| warning that misusing what is here can silently produce insecure results.
//| """
//|

static mp_rom_map_elem_t cryptography_hazmat_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_cryptography_dot_hazmat) },
    { MP_ROM_QSTR(MP_QSTR_primitives), MP_OBJ_FROM_PTR(&cryptography_hazmat_primitives_module) },
};
static MP_DEFINE_CONST_DICT(cryptography_hazmat_module_globals, cryptography_hazmat_module_globals_table);

const mp_obj_module_t cryptography_hazmat_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&cryptography_hazmat_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_cryptography_dot_hazmat, cryptography_hazmat_module);
