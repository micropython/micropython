// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#include "py/enum.h"
#include "py/obj.h"

#include "shared-bindings/hardwarekey/__init__.h"
#include "shared-bindings/hardwarekey/HardwareKey.h"

//| """Cryptographic operations with keys held in hardware
//|
//| The ``hardwarekey`` module exposes keys that live in a hardware key store --
//| eFuse, a key manager, a secure element -- and can be *used* but never read
//| back. Application code can compute a MAC, sign, or decrypt with the key;
//| there is no API to read the raw key bytes, and no API to write or burn keys.
//| Provisioning a key is a manufacturing-time step done with vendor tools (for
//| example ``espefuse.py`` on Espressif chips).
//|
//| `HardwareKey` objects are not created by application code. Every hardware key
//| slot the board has is exposed as a fixed object in :mod:`board` (for example
//| ``board.EFUSE_KEY0``), in the same way that pins are. Compute a MAC with one
//| by passing it to `hmac.new()`; turn a Digital Signature key into a
//| :py:class:`cryptography.hazmat.primitives.asymmetric.rsa.DigitalSignatureKey`
//| with :py:func:`cryptography.hazmat.primitives.asymmetric.rsa.load_digital_signature_key()`.
//| """

MAKE_ENUM_VALUE(hardwarekey_purpose_type, hardwarekey_purpose, HMAC_UP, HARDWAREKEY_PURPOSE_HMAC);
MAKE_ENUM_VALUE(hardwarekey_purpose_type, hardwarekey_purpose, HMAC_DOWN_DIGITAL_SIGNATURE, HARDWAREKEY_PURPOSE_DS);
MAKE_ENUM_VALUE(hardwarekey_purpose_type, hardwarekey_purpose, UNUSED, HARDWAREKEY_PURPOSE_UNUSED);

//| class Purpose:
//|     """What a hardware key slot is provisioned for. Instances are singletons;
//|     compare with ``is``."""
//|
//|     HMAC_UP: object
//|     """The slot holds an HMAC key. It can be used with `hmac.new()`."""
//|
//|     HMAC_DOWN_DIGITAL_SIGNATURE: object
//|     """The slot holds the key for a Digital Signature (RSA) key. Turn it into a
//|     :py:class:`cryptography.hazmat.primitives.asymmetric.rsa.DigitalSignatureKey`
//|     with :py:func:`cryptography.hazmat.primitives.asymmetric.rsa.load_digital_signature_key()`."""
//|
//|     UNUSED: object
//|     """No key is burned into the slot (or it is burned for something this module
//|     does not expose). The slot's `HardwareKey` still exists but cannot be used."""
//|
MAKE_ENUM_MAP(hardwarekey_purpose) {
    MAKE_ENUM_MAP_ENTRY(hardwarekey_purpose, HMAC_UP),
    MAKE_ENUM_MAP_ENTRY(hardwarekey_purpose, HMAC_DOWN_DIGITAL_SIGNATURE),
    MAKE_ENUM_MAP_ENTRY(hardwarekey_purpose, UNUSED),
};
static MP_DEFINE_CONST_DICT(hardwarekey_purpose_locals_dict, hardwarekey_purpose_locals_table);

MAKE_PRINTER(hardwarekey, hardwarekey_purpose);

MAKE_ENUM_TYPE(hardwarekey, Purpose, hardwarekey_purpose);

static const mp_rom_map_elem_t hardwarekey_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_hardwarekey) },
    { MP_ROM_QSTR(MP_QSTR_HardwareKey), MP_ROM_PTR(&hardwarekey_hardwarekey_type) },
    { MP_ROM_QSTR(MP_QSTR_Purpose), MP_ROM_PTR(&hardwarekey_purpose_type) },
};
static MP_DEFINE_CONST_DICT(hardwarekey_module_globals, hardwarekey_module_globals_table);

const mp_obj_module_t hardwarekey_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&hardwarekey_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_hardwarekey, hardwarekey_module);
