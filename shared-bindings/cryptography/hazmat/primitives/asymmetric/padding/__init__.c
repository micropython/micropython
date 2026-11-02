// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/cryptography/hazmat/primitives/asymmetric/padding/__init__.h"
#include "shared-bindings/cryptography/hazmat/primitives/hashes/__init__.h"

//| """Asymmetric padding
//|
//| A subset of :py:mod:`cryptography.hazmat.primitives.asymmetric.padding`:
//| the padding markers used to parameterize signing/decryption elsewhere, for
//| example
//| :py:meth:`cryptography.hazmat.primitives.asymmetric.rsa.DigitalSignatureKey.sign()` /
//| :py:meth:`cryptography.hazmat.primitives.asymmetric.rsa.DigitalSignatureKey.decrypt()`.
//| It does no cryptography itself and holds no key material; it exists only
//| so those operations can take explicit ``padding``/``algorithm`` arguments
//| instead of baking one fixed combination into a method name.
//|
//| `PKCS1v15` is a fixed, stateless constant (compare with ``is``; there is
//| nothing to configure, so there is nothing to construct). `OAEP` is a real
//| class since it actually takes a parameter.
//| """
//|

//| PKCS1v15: object
//| """PKCS#1 v1.5 padding, for either signing (RSASSA-PKCS1-v1_5) or
//| decryption (RSAES-PKCS1-v1_5, the older, legacy encryption padding; prefer
//| `OAEP` for new designs). Mirrors
//| :py:class:`cryptography.hazmat.primitives.asymmetric.padding.PKCS1v15`, which is
//| likewise used for both operations."""
MP_DEFINE_CONST_OBJ_TYPE(
    cryptography_hazmat_primitives_asymmetric_padding_pkcs1v15_type,
    MP_QSTR_PKCS1v15,
    MP_TYPE_FLAG_NONE
    );
const mp_obj_base_t cryptography_hazmat_primitives_asymmetric_padding_pkcs1v15_obj = { &cryptography_hazmat_primitives_asymmetric_padding_pkcs1v15_type };

//| class OAEP:
//|     """RSAES-OAEP decryption padding. Only ``algorithm=hashes.SHA256`` and
//|     ``label=None`` are supported today -- an actual driver may use the same hash
//|     for both the OAEP digest and its MGF1 mask, and may not support a label.
//|     Mirrors the one supported combination of
//|     :py:class:`cryptography.hazmat.primitives.asymmetric.padding.OAEP`.
//|
//|     Whether `OAEP` is actually usable depends on the operation it's passed to --
//|     see e.g.
//|     :py:meth:`cryptography.hazmat.primitives.asymmetric.rsa.DigitalSignatureKey.decrypt()`."""
//|
//|     def __init__(self, algorithm: object, *, label: Optional[ReadableBuffer] = None) -> None:
//|         """
//|         :param object algorithm: must be `hashes.SHA256` -- the only hash algorithm supported
//|         :param Optional[~circuitpython_typing.ReadableBuffer] label: must be omitted or
//|           ``None`` -- a label is not supported
//|         """
//|         ...
static mp_obj_t cryptography_hazmat_primitives_asymmetric_padding_oaep_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_algorithm, ARG_label };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_algorithm, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_label, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };
    mp_arg_check_num(n_args, n_kw, 1, 1, true);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (!mp_obj_is_type(args[ARG_algorithm].u_obj, &cryptography_hazmat_primitives_hashes_sha256_type)) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("Only %q supported"), MP_QSTR_SHA256);
    }
    if (args[ARG_label].u_obj != mp_const_none) {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("label is not supported"));
    }

    return MP_OBJ_FROM_PTR(&cryptography_hazmat_primitives_asymmetric_padding_oaep_singleton);
}
MP_DEFINE_CONST_OBJ_TYPE(
    cryptography_hazmat_primitives_asymmetric_padding_oaep_type,
    MP_QSTR_OAEP,
    MP_TYPE_FLAG_NONE,
    make_new, cryptography_hazmat_primitives_asymmetric_padding_oaep_make_new
    );
const mp_obj_base_t cryptography_hazmat_primitives_asymmetric_padding_oaep_singleton = { &cryptography_hazmat_primitives_asymmetric_padding_oaep_type };

static mp_rom_map_elem_t cryptography_hazmat_primitives_asymmetric_padding_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_cryptography_dot_hazmat_dot_primitives_dot_asymmetric_dot_padding) },
    { MP_ROM_QSTR(MP_QSTR_PKCS1v15), MP_ROM_PTR(&cryptography_hazmat_primitives_asymmetric_padding_pkcs1v15_obj) },
    { MP_ROM_QSTR(MP_QSTR_OAEP), MP_ROM_PTR(&cryptography_hazmat_primitives_asymmetric_padding_oaep_type) },
};
static MP_DEFINE_CONST_DICT(cryptography_hazmat_primitives_asymmetric_padding_module_globals, cryptography_hazmat_primitives_asymmetric_padding_module_globals_table);

const mp_obj_module_t cryptography_hazmat_primitives_asymmetric_padding_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&cryptography_hazmat_primitives_asymmetric_padding_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_cryptography_dot_hazmat_dot_primitives_dot_asymmetric_dot_padding, cryptography_hazmat_primitives_asymmetric_padding_module);
