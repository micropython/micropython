// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"

#include "shared-bindings/cryptography/hazmat/primitives/asymmetric/ec/EllipticCurvePublicKey.h"
#include "shared-bindings/cryptography/hazmat/primitives/asymmetric/ec/__init__.h"

//| """Elliptic-curve keys
//|
//| A subset of :py:mod:`cryptography.hazmat.primitives.asymmetric.ec`:
//| P-256 (secp256r1) signature *verification* only -- there is no private-key
//| type here, since verifying a signature against a known public key needs
//| no secret material and no hardware peripheral.
//| """
//|

//| class SECP256R1:
//|     """The NIST P-256 (secp256r1) curve -- the only curve this module
//|     supports. Mirrors the one PyCA curve class this module implements:
//|     :py:class:`cryptography.hazmat.primitives.asymmetric.ec.SECP256R1`."""
//|
//|     def __init__(self) -> None:
//|         ...
//|
static mp_obj_t cryptography_hazmat_primitives_asymmetric_ec_secp256r1_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    return MP_OBJ_FROM_PTR(&cryptography_hazmat_primitives_asymmetric_ec_secp256r1_obj);
}

//|     name: str
//|     """``"secp256r1"``. Mirrors
//|     :py:attr:`cryptography.hazmat.primitives.asymmetric.ec.EllipticCurve.name`.
//|     (read-only)"""
static mp_obj_t cryptography_hazmat_primitives_asymmetric_ec_secp256r1_get_name(mp_obj_t self_in) {
    return MP_OBJ_NEW_QSTR(MP_QSTR_secp256r1);
}
MP_DEFINE_CONST_FUN_OBJ_1(cryptography_hazmat_primitives_asymmetric_ec_secp256r1_get_name_obj, cryptography_hazmat_primitives_asymmetric_ec_secp256r1_get_name);
MP_PROPERTY_GETTER(cryptography_hazmat_primitives_asymmetric_ec_secp256r1_name_obj, (mp_obj_t)&cryptography_hazmat_primitives_asymmetric_ec_secp256r1_get_name_obj);

//|     key_size: int
//|     """``256``. Mirrors
//|     :py:attr:`cryptography.hazmat.primitives.asymmetric.ec.EllipticCurve.key_size`.
//|     (read-only)"""
static mp_obj_t cryptography_hazmat_primitives_asymmetric_ec_secp256r1_get_key_size(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(256);
}
MP_DEFINE_CONST_FUN_OBJ_1(cryptography_hazmat_primitives_asymmetric_ec_secp256r1_get_key_size_obj, cryptography_hazmat_primitives_asymmetric_ec_secp256r1_get_key_size);
MP_PROPERTY_GETTER(cryptography_hazmat_primitives_asymmetric_ec_secp256r1_key_size_obj, (mp_obj_t)&cryptography_hazmat_primitives_asymmetric_ec_secp256r1_get_key_size_obj);

static const mp_rom_map_elem_t cryptography_hazmat_primitives_asymmetric_ec_secp256r1_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_name), MP_ROM_PTR(&cryptography_hazmat_primitives_asymmetric_ec_secp256r1_name_obj) },
    { MP_ROM_QSTR(MP_QSTR_key_size), MP_ROM_PTR(&cryptography_hazmat_primitives_asymmetric_ec_secp256r1_key_size_obj) },
};
static MP_DEFINE_CONST_DICT(cryptography_hazmat_primitives_asymmetric_ec_secp256r1_locals_dict, cryptography_hazmat_primitives_asymmetric_ec_secp256r1_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    cryptography_hazmat_primitives_asymmetric_ec_secp256r1_type,
    MP_QSTR_SECP256R1,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, cryptography_hazmat_primitives_asymmetric_ec_secp256r1_make_new,
    locals_dict, &cryptography_hazmat_primitives_asymmetric_ec_secp256r1_locals_dict
    );
const mp_obj_base_t cryptography_hazmat_primitives_asymmetric_ec_secp256r1_obj = { &cryptography_hazmat_primitives_asymmetric_ec_secp256r1_type };

//| class ECDSA:
//|     """Parameterizes `EllipticCurvePublicKey.verify` with the digest
//|     algorithm a signature was computed over. Mirrors
//|     :py:class:`cryptography.hazmat.primitives.asymmetric.ec.ECDSA`.
//|
//|     Only ``algorithm=utils.Prehashed(hashes.SHA256)`` is supported --
//|     `verify` hashes nothing itself; the caller must already have a
//|     SHA-256 digest in hand."""
//|
//|     def __init__(self, algorithm: object) -> None:
//|         """
//|         :param object algorithm: must be `utils.Prehashed` wrapping
//|           `hashes.SHA256` -- the only combination supported
//|         """
//|         ...
static mp_obj_t cryptography_hazmat_primitives_asymmetric_ec_ecdsa_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    cryptography_hazmat_primitives_asymmetric_ec_ecdsa_obj_t *self =
        mp_obj_malloc(cryptography_hazmat_primitives_asymmetric_ec_ecdsa_obj_t, type);
    self->algorithm = all_args[0];
    return MP_OBJ_FROM_PTR(self);
}

//|     algorithm: object
//|     """The algorithm passed to the constructor. Mirrors
//|     :py:attr:`cryptography.hazmat.primitives.asymmetric.ec.ECDSA.algorithm`.
//|     (read-only)"""
static mp_obj_t cryptography_hazmat_primitives_asymmetric_ec_ecdsa_get_algorithm(mp_obj_t self_in) {
    cryptography_hazmat_primitives_asymmetric_ec_ecdsa_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return self->algorithm;
}
MP_DEFINE_CONST_FUN_OBJ_1(cryptography_hazmat_primitives_asymmetric_ec_ecdsa_get_algorithm_obj, cryptography_hazmat_primitives_asymmetric_ec_ecdsa_get_algorithm);
MP_PROPERTY_GETTER(cryptography_hazmat_primitives_asymmetric_ec_ecdsa_algorithm_obj, (mp_obj_t)&cryptography_hazmat_primitives_asymmetric_ec_ecdsa_get_algorithm_obj);

static const mp_rom_map_elem_t cryptography_hazmat_primitives_asymmetric_ec_ecdsa_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_algorithm), MP_ROM_PTR(&cryptography_hazmat_primitives_asymmetric_ec_ecdsa_algorithm_obj) },
};
static MP_DEFINE_CONST_DICT(cryptography_hazmat_primitives_asymmetric_ec_ecdsa_locals_dict, cryptography_hazmat_primitives_asymmetric_ec_ecdsa_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    cryptography_hazmat_primitives_asymmetric_ec_ecdsa_type,
    MP_QSTR_ECDSA,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, cryptography_hazmat_primitives_asymmetric_ec_ecdsa_make_new,
    locals_dict, &cryptography_hazmat_primitives_asymmetric_ec_ecdsa_locals_dict
    );

static mp_rom_map_elem_t cryptography_hazmat_primitives_asymmetric_ec_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_cryptography_dot_hazmat_dot_primitives_dot_asymmetric_dot_ec) },
    { MP_ROM_QSTR(MP_QSTR_SECP256R1), MP_ROM_PTR(&cryptography_hazmat_primitives_asymmetric_ec_secp256r1_type) },
    { MP_ROM_QSTR(MP_QSTR_ECDSA), MP_ROM_PTR(&cryptography_hazmat_primitives_asymmetric_ec_ecdsa_type) },
    { MP_ROM_QSTR(MP_QSTR_EllipticCurvePublicKey), MP_ROM_PTR(&cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_type) },
};
static MP_DEFINE_CONST_DICT(cryptography_hazmat_primitives_asymmetric_ec_module_globals, cryptography_hazmat_primitives_asymmetric_ec_module_globals_table);

const mp_obj_module_t cryptography_hazmat_primitives_asymmetric_ec_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&cryptography_hazmat_primitives_asymmetric_ec_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_cryptography_dot_hazmat_dot_primitives_dot_asymmetric_dot_ec, cryptography_hazmat_primitives_asymmetric_ec_module);
