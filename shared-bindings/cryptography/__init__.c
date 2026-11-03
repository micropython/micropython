// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/cryptography/__init__.h"
#include "shared-bindings/cryptography/hazmat/__init__.h"

//| """A subset of PyCA's `cryptography <https://cryptography.io>`_ package
//|
//| ``cryptography``'s nesting is mirrored module-for-module (``cryptography.hazmat.primitives.asymmetric.rsa``,
//| not a flattened equivalent) so that code written against this module is,
//| as far as it goes, portable to CPython unchanged. Only the pieces this
//| board's hardware can actually back are present -- today, just
//| `DigitalSignatureKey` for the Digital Signature peripheral.
//| """
//|

static mp_rom_map_elem_t cryptography_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_cryptography) },
    { MP_ROM_QSTR(MP_QSTR_hazmat), MP_OBJ_FROM_PTR(&cryptography_hazmat_module) },
};
static MP_DEFINE_CONST_DICT(cryptography_module_globals, cryptography_module_globals_table);

const mp_obj_module_t cryptography_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&cryptography_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_cryptography, cryptography_module);
