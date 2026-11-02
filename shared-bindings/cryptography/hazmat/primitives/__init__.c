// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/cryptography/hazmat/primitives/__init__.h"
#include "shared-bindings/cryptography/hazmat/primitives/asymmetric/__init__.h"
#include "shared-bindings/cryptography/hazmat/primitives/hashes/__init__.h"

//| """Cryptographic primitives
//|
//| A subset of :py:mod:`cryptography.hazmat.primitives`.
//| """
//|

static mp_rom_map_elem_t cryptography_hazmat_primitives_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_cryptography_dot_hazmat_dot_primitives) },
    { MP_ROM_QSTR(MP_QSTR_asymmetric), MP_OBJ_FROM_PTR(&cryptography_hazmat_primitives_asymmetric_module) },
    { MP_ROM_QSTR(MP_QSTR_hashes), MP_OBJ_FROM_PTR(&cryptography_hazmat_primitives_hashes_module) },
};
static MP_DEFINE_CONST_DICT(cryptography_hazmat_primitives_module_globals, cryptography_hazmat_primitives_module_globals_table);

const mp_obj_module_t cryptography_hazmat_primitives_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&cryptography_hazmat_primitives_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_cryptography_dot_hazmat_dot_primitives, cryptography_hazmat_primitives_module);
