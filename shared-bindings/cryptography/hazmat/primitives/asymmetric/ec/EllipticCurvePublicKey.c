// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#include "py/objproperty.h"
#include "py/runtime.h"

#include "shared-bindings/cryptography/hazmat/primitives/asymmetric/ec/EllipticCurvePublicKey.h"
#include "shared-bindings/cryptography/hazmat/primitives/asymmetric/ec/__init__.h"
#include "shared-bindings/cryptography/hazmat/primitives/asymmetric/utils/__init__.h"
#include "shared-bindings/cryptography/hazmat/primitives/hashes/__init__.h"

//| class EllipticCurvePublicKey:
//|     """An elliptic-curve public key, usable to verify signatures made by
//|     the matching private key. This class cannot be instantiated directly
//|     -- get one from `from_encoded_point()`.
//|
//|     Mirrors the method shape of
//|     :py:class:`cryptography.hazmat.primitives.asymmetric.ec.EllipticCurvePublicKey`
//|     (``verify``, ``public_bytes``, ``curve``, ``key_size``), but not
//|     every method on that class -- there is intentionally no
//|     ``public_numbers()``; only the raw-point `public_bytes` encoding this
//|     module already stores is supported.
//|     """
//|

//|     @classmethod
//|     def from_encoded_point(cls, curve: object, data: ReadableBuffer) -> EllipticCurvePublicKey:
//|         """Build a public key from an uncompressed SEC1 point. Mirrors
//|         :py:meth:`cryptography.hazmat.primitives.asymmetric.ec.EllipticCurvePublicKey.from_encoded_point`.
//|
//|         :param ec.SECP256R1 curve: must be `SECP256R1` -- the only curve supported
//|         :param ~circuitpython_typing.ReadableBuffer data: a 65-byte uncompressed
//|           point, ``0x04 || X || Y``
//|         :raises ValueError: if ``data`` is not a valid point for ``curve``
//|         """
//|         ...
static mp_obj_t cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_from_encoded_point(mp_obj_t cls_in, mp_obj_t curve_in, mp_obj_t data_in) {
    if (!mp_obj_is_type(curve_in, &cryptography_hazmat_primitives_asymmetric_ec_secp256r1_type)) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("Only %q supported"), MP_QSTR_SECP256R1);
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data_in, &bufinfo, MP_BUFFER_READ);

    cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_obj_t *self =
        mp_obj_malloc(cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_obj_t,
            &cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_type);
    common_hal_cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_construct(self,
        curve_in, bufinfo.buf, bufinfo.len);
    return MP_OBJ_FROM_PTR(self);
}
static MP_DEFINE_CONST_FUN_OBJ_3(cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_from_encoded_point_fun_obj, cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_from_encoded_point);
static MP_DEFINE_CONST_CLASSMETHOD_OBJ(cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_from_encoded_point_obj, MP_ROM_PTR(&cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_from_encoded_point_fun_obj));

//|     curve: object
//|     """The curve this key was constructed for -- always `SECP256R1`.
//|     Mirrors
//|     :py:attr:`cryptography.hazmat.primitives.asymmetric.ec.EllipticCurvePublicKey.curve`.
//|     (read-only)"""
static mp_obj_t cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_get_curve(mp_obj_t self_in) {
    cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return self->curve;
}
MP_DEFINE_CONST_FUN_OBJ_1(cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_get_curve_obj, cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_get_curve);
MP_PROPERTY_GETTER(cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_curve_obj, (mp_obj_t)&cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_get_curve_obj);

//|     key_size: int
//|     """``256``. Mirrors
//|     :py:attr:`cryptography.hazmat.primitives.asymmetric.ec.EllipticCurvePublicKey.key_size`.
//|     (read-only)"""
static mp_obj_t cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_get_key_size(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(256);
}
MP_DEFINE_CONST_FUN_OBJ_1(cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_get_key_size_obj, cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_get_key_size);
MP_PROPERTY_GETTER(cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_key_size_obj, (mp_obj_t)&cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_get_key_size_obj);

//|     def verify(
//|         self,
//|         signature: ReadableBuffer,
//|         data: ReadableBuffer,
//|         signature_algorithm: object,
//|     ) -> None:
//|         """Verify ``signature`` over ``data``. Mirrors
//|         :py:meth:`cryptography.hazmat.primitives.asymmetric.ec.EllipticCurvePublicKey.verify`.
//|
//|         :param ~circuitpython_typing.ReadableBuffer signature: a DER-encoded
//|           ECDSA signature (the ``(r, s)`` pair ASN.1-wrapped, as real PyCA
//|           produces/expects -- not the raw ``r || s`` some hardware, e.g.
//|           ATECC608-family chips, outputs natively. Use
//|           `utils.encode_dss_signature` to convert
//|         :param ~circuitpython_typing.ReadableBuffer data: the SHA-256 digest
//|           already computed over the signed message (see ``signature_algorithm``)
//|         :param ec.ECDSA signature_algorithm: must be
//|           ``ECDSA(utils.Prehashed(hashes.SHA256))`` -- the only combination
//|           supported. ``data`` is never hashed by this method; the caller
//|           must already have a digest
//|         :raises cryptography.exceptions.InvalidSignature: if ``signature``
//|           is malformed, or does not verify against ``data``
//|         """
//|         ...
static mp_obj_t cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_verify(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_signature, ARG_data, ARG_signature_algorithm };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_signature, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_data, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_signature_algorithm, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };
    cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (!mp_obj_is_type(args[ARG_signature_algorithm].u_obj, &cryptography_hazmat_primitives_asymmetric_ec_ecdsa_type)) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("Only %q supported"), MP_QSTR_ECDSA);
    }
    cryptography_hazmat_primitives_asymmetric_ec_ecdsa_obj_t *ecdsa = MP_OBJ_TO_PTR(args[ARG_signature_algorithm].u_obj);
    if (!mp_obj_is_type(ecdsa->algorithm, &cryptography_hazmat_primitives_asymmetric_utils_prehashed_type)) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("Only %q supported"), MP_QSTR_Prehashed_space_SHA256);
    }
    cryptography_hazmat_primitives_asymmetric_utils_prehashed_obj_t *prehashed = MP_OBJ_TO_PTR(ecdsa->algorithm);
    if (!mp_obj_is_type(prehashed->algorithm, &cryptography_hazmat_primitives_hashes_sha256_type)) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("Only %q supported"), MP_QSTR_Prehashed_space_SHA256);
    }

    mp_buffer_info_t sig_bufinfo;
    mp_get_buffer_raise(args[ARG_signature].u_obj, &sig_bufinfo, MP_BUFFER_READ);
    mp_buffer_info_t data_bufinfo;
    mp_get_buffer_raise(args[ARG_data].u_obj, &data_bufinfo, MP_BUFFER_READ);

    common_hal_cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_verify(self,
        sig_bufinfo.buf, sig_bufinfo.len, data_bufinfo.buf, data_bufinfo.len);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_verify_obj, 1, cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_verify);

//|     def public_bytes(self, encoding: object, format: object) -> bytes:
//|         """Return the raw point this key was constructed from. Mirrors
//|         :py:meth:`cryptography.hazmat.primitives.asymmetric.ec.EllipticCurvePublicKey.public_bytes`,
//|         restricted to the one encoding this module ever stores -- there is
//|         no ``serialization`` module here to construct any other
//|         ``encoding``/``format`` value with, so both arguments are accepted
//|         for call-shape compatibility and ignored; the result is always the
//|         same as the ``data`` originally passed to `from_encoded_point`
//|         (uncompressed SEC1, ``0x04 || X || Y``).
//|
//|         :param object encoding: ignored
//|         :param object format: ignored
//|         """
//|         ...
static mp_obj_t cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_public_bytes(mp_obj_t self_in, mp_obj_t encoding_in, mp_obj_t format_in) {
    cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bytes(self->point, sizeof(self->point));
}
static MP_DEFINE_CONST_FUN_OBJ_3(cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_public_bytes_obj, cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_public_bytes);

static const mp_rom_map_elem_t cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_from_encoded_point), MP_ROM_PTR(&cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_from_encoded_point_obj) },
    { MP_ROM_QSTR(MP_QSTR_curve), MP_ROM_PTR(&cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_curve_obj) },
    { MP_ROM_QSTR(MP_QSTR_key_size), MP_ROM_PTR(&cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_key_size_obj) },
    { MP_ROM_QSTR(MP_QSTR_verify), MP_ROM_PTR(&cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_verify_obj) },
    { MP_ROM_QSTR(MP_QSTR_public_bytes), MP_ROM_PTR(&cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_public_bytes_obj) },
};
static MP_DEFINE_CONST_DICT(cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_locals_dict, cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_type,
    MP_QSTR_EllipticCurvePublicKey,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    locals_dict, &cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_locals_dict
    );
