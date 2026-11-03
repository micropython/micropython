// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/cryptography/hazmat/primitives/asymmetric/rsa/DigitalSignatureKey.h"
#include "shared-bindings/cryptography/hazmat/primitives/asymmetric/rsa/__init__.h"
#include "shared-bindings/hardwarekey/HardwareKey.h"

//| """RSA keys
//|
//| A subset of :py:mod:`cryptography.hazmat.primitives.asymmetric.rsa`: a
//| hardware-backed private-key type for the Digital Signature peripheral.
//| """
//|

//| def load_digital_signature_key(
//|     key: hardwarekey.HardwareKey, ds_params: ReadableBuffer
//| ) -> DigitalSignatureKey:
//|     """Make a Digital Signature hardware key usable for signing and
//|     decryption. Mirrors
//|     :py:func:`cryptography.hazmat.primitives.serialization.load_pem_private_key`:
//|     a blob of key material in, a ready-to-use private-key object out.
//|
//|     :param hardwarekey.HardwareKey key: must have `hardwarekey.HardwareKey.purpose` ==
//|       `hardwarekey.Purpose.HMAC_DOWN_DIGITAL_SIGNATURE`
//|     :param ~circuitpython_typing.ReadableBuffer ds_params: the encrypted Digital
//|       Signature parameter block for this RSA key, produced at provisioning
//|       time by vendor tooling (never by this module). On espressif this is the
//|       raw ``esp_ds_data_t`` structure. Not secret -- it is only usable
//|       together with this slot's eFuse key -- so it is fine to keep in a plain
//|       file, e.g. on the ``CIRCUITPY`` filesystem.
//|     :raises ValueError: if `key`'s purpose is wrong, or ``ds_params`` is malformed
//|     """
//|     ...
static mp_obj_t cryptography_hazmat_primitives_asymmetric_rsa_load_digital_signature_key(mp_obj_t key_in, mp_obj_t ds_params_in) {
    hardwarekey_hardwarekey_obj_t *key =
        mp_arg_validate_type(key_in, &hardwarekey_hardwarekey_type, MP_QSTR_key);
    if (common_hal_hardwarekey_hardwarekey_get_purpose(key) != HARDWAREKEY_PURPOSE_DS) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("key does not have the expected %q purpose"),
            MP_QSTR_HMAC_DOWN_DIGITAL_SIGNATURE);
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(ds_params_in, &bufinfo, MP_BUFFER_READ);

    cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_obj_t *self =
        mp_obj_malloc(cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_obj_t,
        &cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_type);
    self->source_key = key_in;
    common_hal_cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_construct(self, key, bufinfo.buf, bufinfo.len);
    return MP_OBJ_FROM_PTR(self);
}
MP_DEFINE_CONST_FUN_OBJ_2(cryptography_hazmat_primitives_asymmetric_rsa_load_digital_signature_key_obj, cryptography_hazmat_primitives_asymmetric_rsa_load_digital_signature_key);

static mp_rom_map_elem_t cryptography_hazmat_primitives_asymmetric_rsa_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_cryptography_dot_hazmat_dot_primitives_dot_asymmetric_dot_rsa) },
    { MP_ROM_QSTR(MP_QSTR_DigitalSignatureKey), MP_ROM_PTR(&cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_type) },
    { MP_ROM_QSTR(MP_QSTR_load_digital_signature_key), MP_ROM_PTR(&cryptography_hazmat_primitives_asymmetric_rsa_load_digital_signature_key_obj) },
};
static MP_DEFINE_CONST_DICT(cryptography_hazmat_primitives_asymmetric_rsa_module_globals, cryptography_hazmat_primitives_asymmetric_rsa_module_globals_table);

const mp_obj_module_t cryptography_hazmat_primitives_asymmetric_rsa_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&cryptography_hazmat_primitives_asymmetric_rsa_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_cryptography_dot_hazmat_dot_primitives_dot_asymmetric_dot_rsa, cryptography_hazmat_primitives_asymmetric_rsa_module);
