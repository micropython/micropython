// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#include "py/objint.h"
#include "py/objproperty.h"
#include "py/runtime.h"

#include "mbedtls/psa_util.h"

#include "shared-bindings/cryptography/hazmat/primitives/asymmetric/utils/__init__.h"

// P-256 only: each of r and s is 32 bytes, raw r||s is 64 bytes.
#define P256_KEY_BITS 256
#define P256_COORD_LEN 32
#define P256_RAW_SIG_LEN 64

//| """Signature/key encoding helpers
//|
//| A subset of :py:mod:`cryptography.hazmat.primitives.asymmetric.utils`:
//| `Prehashed`, plus the ECDSA DER<->(r, s) conversion functions, for
//| adapting hardware (e.g. ATECC608-family chips) that natively produces raw
//| ``r || s`` rather than the DER real PyCA's ECDSA uses.
//| """
//|

//| class Prehashed:
//|     """Marks an algorithm as already applied -- `ec.EllipticCurvePublicKey.verify`
//|     never hashes ``data`` itself when its ``signature_algorithm.algorithm``
//|     is one of these. Mirrors
//|     :py:class:`cryptography.hazmat.primitives.asymmetric.utils.Prehashed`."""
//|
//|     def __init__(self, algorithm: object) -> None:
//|         """
//|         :param object algorithm: must be `hashes.SHA256` -- the only hash
//|           algorithm supported
//|         """
//|         ...
static mp_obj_t cryptography_hazmat_primitives_asymmetric_utils_prehashed_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    cryptography_hazmat_primitives_asymmetric_utils_prehashed_obj_t *self =
        mp_obj_malloc(cryptography_hazmat_primitives_asymmetric_utils_prehashed_obj_t, type);
    self->algorithm = all_args[0];
    return MP_OBJ_FROM_PTR(self);
}

//|     algorithm: object
//|     """The algorithm passed to the constructor. Mirrors
//|     :py:attr:`cryptography.hazmat.primitives.asymmetric.utils.Prehashed._algorithm`.
//|     (read-only)"""
static mp_obj_t cryptography_hazmat_primitives_asymmetric_utils_prehashed_get_algorithm(mp_obj_t self_in) {
    cryptography_hazmat_primitives_asymmetric_utils_prehashed_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return self->algorithm;
}
MP_DEFINE_CONST_FUN_OBJ_1(cryptography_hazmat_primitives_asymmetric_utils_prehashed_get_algorithm_obj, cryptography_hazmat_primitives_asymmetric_utils_prehashed_get_algorithm);
MP_PROPERTY_GETTER(cryptography_hazmat_primitives_asymmetric_utils_prehashed_algorithm_obj, (mp_obj_t)&cryptography_hazmat_primitives_asymmetric_utils_prehashed_get_algorithm_obj);

//|     digest_size: int
//|     """``32`` -- the digest size of `hashes.SHA256`, the only algorithm
//|     supported. Mirrors
//|     :py:attr:`cryptography.hazmat.primitives.asymmetric.utils.Prehashed.digest_size`.
//|     (read-only)"""
static mp_obj_t cryptography_hazmat_primitives_asymmetric_utils_prehashed_get_digest_size(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(32);
}
MP_DEFINE_CONST_FUN_OBJ_1(cryptography_hazmat_primitives_asymmetric_utils_prehashed_get_digest_size_obj, cryptography_hazmat_primitives_asymmetric_utils_prehashed_get_digest_size);
MP_PROPERTY_GETTER(cryptography_hazmat_primitives_asymmetric_utils_prehashed_digest_size_obj, (mp_obj_t)&cryptography_hazmat_primitives_asymmetric_utils_prehashed_get_digest_size_obj);

static const mp_rom_map_elem_t cryptography_hazmat_primitives_asymmetric_utils_prehashed_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_algorithm), MP_ROM_PTR(&cryptography_hazmat_primitives_asymmetric_utils_prehashed_algorithm_obj) },
    { MP_ROM_QSTR(MP_QSTR_digest_size), MP_ROM_PTR(&cryptography_hazmat_primitives_asymmetric_utils_prehashed_digest_size_obj) },
};
static MP_DEFINE_CONST_DICT(cryptography_hazmat_primitives_asymmetric_utils_prehashed_locals_dict, cryptography_hazmat_primitives_asymmetric_utils_prehashed_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    cryptography_hazmat_primitives_asymmetric_utils_prehashed_type,
    MP_QSTR_Prehashed,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, cryptography_hazmat_primitives_asymmetric_utils_prehashed_make_new,
    locals_dict, &cryptography_hazmat_primitives_asymmetric_utils_prehashed_locals_dict
    );

//| def encode_dss_signature(r: int, s: int) -> bytes:
//|     """DER-encode a raw ``(r, s)`` ECDSA signature pair -- the format
//|     hardware (e.g. ATECC608-family chips) natively produces, and the
//|     format `ec.EllipticCurvePublicKey.verify` does *not* accept directly.
//|     Mirrors
//|     :py:func:`cryptography.hazmat.primitives.asymmetric.utils.encode_dss_signature`,
//|     restricted to P-256-sized ``r``/``s`` (each at most 32 bytes unsigned).
//|
//|     :param int r: the signature's ``r`` value
//|     :param int s: the signature's ``s`` value
//|     """
//|     ...
static mp_obj_t cryptography_hazmat_primitives_asymmetric_utils_encode_dss_signature(mp_obj_t r_in, mp_obj_t s_in) {
    uint8_t raw_sig[P256_RAW_SIG_LEN];
    mp_obj_int_to_bytes(r_in, P256_COORD_LEN, raw_sig, true, false, true);
    mp_obj_int_to_bytes(s_in, P256_COORD_LEN, raw_sig + P256_COORD_LEN, true, false, true);

    uint8_t der_sig[MBEDTLS_ECDSA_MAX_LEN];
    size_t der_len = 0;
    int ret = mbedtls_ecdsa_raw_to_der(P256_KEY_BITS, raw_sig, sizeof(raw_sig), der_sig, sizeof(der_sig), &der_len);
    if (ret != 0) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q"), MP_QSTR_signature);
    }
    return mp_obj_new_bytes(der_sig, der_len);
}
MP_DEFINE_CONST_FUN_OBJ_2(cryptography_hazmat_primitives_asymmetric_utils_encode_dss_signature_obj, cryptography_hazmat_primitives_asymmetric_utils_encode_dss_signature);

//| def decode_dss_signature(signature: ReadableBuffer) -> Tuple[int, int]:
//|     """The inverse of `encode_dss_signature`: split a DER-encoded ECDSA
//|     signature back into its raw ``(r, s)`` pair. Mirrors
//|     :py:func:`cryptography.hazmat.primitives.asymmetric.utils.decode_dss_signature`,
//|     restricted to P-256-sized signatures.
//|
//|     :param ~circuitpython_typing.ReadableBuffer signature: a DER-encoded ECDSA signature
//|     :raises ValueError: if ``signature`` is not a well-formed DER ECDSA signature
//|     """
//|     ...
static mp_obj_t cryptography_hazmat_primitives_asymmetric_utils_decode_dss_signature(mp_obj_t signature_in) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(signature_in, &bufinfo, MP_BUFFER_READ);

    uint8_t raw_sig[P256_RAW_SIG_LEN];
    size_t raw_len = 0;
    int ret = mbedtls_ecdsa_der_to_raw(P256_KEY_BITS, bufinfo.buf, bufinfo.len, raw_sig, sizeof(raw_sig), &raw_len);
    if (ret != 0 || raw_len != P256_RAW_SIG_LEN) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q"), MP_QSTR_signature);
    }

    mp_obj_t items[2] = {
        mp_obj_int_from_bytes_impl(true, P256_COORD_LEN, raw_sig),
        mp_obj_int_from_bytes_impl(true, P256_COORD_LEN, raw_sig + P256_COORD_LEN),
    };
    return mp_obj_new_tuple(2, items);
}
MP_DEFINE_CONST_FUN_OBJ_1(cryptography_hazmat_primitives_asymmetric_utils_decode_dss_signature_obj, cryptography_hazmat_primitives_asymmetric_utils_decode_dss_signature);

static mp_rom_map_elem_t cryptography_hazmat_primitives_asymmetric_utils_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_cryptography_dot_hazmat_dot_primitives_dot_asymmetric_dot_utils) },
    { MP_ROM_QSTR(MP_QSTR_Prehashed), MP_ROM_PTR(&cryptography_hazmat_primitives_asymmetric_utils_prehashed_type) },
    { MP_ROM_QSTR(MP_QSTR_encode_dss_signature), MP_ROM_PTR(&cryptography_hazmat_primitives_asymmetric_utils_encode_dss_signature_obj) },
    { MP_ROM_QSTR(MP_QSTR_decode_dss_signature), MP_ROM_PTR(&cryptography_hazmat_primitives_asymmetric_utils_decode_dss_signature_obj) },
};
static MP_DEFINE_CONST_DICT(cryptography_hazmat_primitives_asymmetric_utils_module_globals, cryptography_hazmat_primitives_asymmetric_utils_module_globals_table);

const mp_obj_module_t cryptography_hazmat_primitives_asymmetric_utils_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&cryptography_hazmat_primitives_asymmetric_utils_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_cryptography_dot_hazmat_dot_primitives_dot_asymmetric_dot_utils, cryptography_hazmat_primitives_asymmetric_utils_module);
