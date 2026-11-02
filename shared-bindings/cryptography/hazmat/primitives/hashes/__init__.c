// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/cryptography/hazmat/primitives/hashes/__init__.h"

//| """Hash algorithms
//|
//| A subset of :py:mod:`cryptography.hazmat.primitives.hashes`: the hash
//| algorithm markers used to parameterize signing/decryption elsewhere, for
//| example
//| :py:meth:`cryptography.hazmat.primitives.asymmetric.rsa.DigitalSignatureKey.sign()`.
//| It does no hashing itself.
//| """
//|

//| SHA256: object
//| """The SHA-256 hash algorithm. Mirrors
//| :py:class:`cryptography.hazmat.primitives.hashes.SHA256`."""
MP_DEFINE_CONST_OBJ_TYPE(
    cryptography_hazmat_primitives_hashes_sha256_type,
    MP_QSTR_SHA256,
    MP_TYPE_FLAG_NONE
    );
const mp_obj_base_t cryptography_hazmat_primitives_hashes_sha256_obj = { &cryptography_hazmat_primitives_hashes_sha256_type };

static mp_rom_map_elem_t cryptography_hazmat_primitives_hashes_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_cryptography_dot_hazmat_dot_primitives_dot_hashes) },
    { MP_ROM_QSTR(MP_QSTR_SHA256), MP_ROM_PTR(&cryptography_hazmat_primitives_hashes_sha256_obj) },
};
static MP_DEFINE_CONST_DICT(cryptography_hazmat_primitives_hashes_module_globals, cryptography_hazmat_primitives_hashes_module_globals_table);

const mp_obj_module_t cryptography_hazmat_primitives_hashes_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&cryptography_hazmat_primitives_hashes_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_cryptography_dot_hazmat_dot_primitives_dot_hashes, cryptography_hazmat_primitives_hashes_module);
