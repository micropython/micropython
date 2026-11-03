// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "py/obj.h"

#include "psa/crypto.h"

// What a hardware key slot is provisioned for. UNUSED means no key has been
// burned into the slot (or it is burned for something this module does not
// expose); the slot is present in `board` but not usable.
typedef enum {
    HARDWAREKEY_PURPOSE_UNUSED = 0,
    HARDWAREKEY_PURPOSE_HMAC,
    // A Digital Signature (RSA) key. Unlike HMAC, the key material isn't
    // fully in the eFuse block itself -- the slot only holds the AES key
    // that decrypts an externally-supplied, already-encrypted RSA private
    // key (`ds_params`) inside the DS peripheral. So a DS-purpose slot is
    // usable only after
    // cryptography.hazmat.primitives.asymmetric.rsa.load_digital_signature_key()
    // has turned it into a DigitalSignatureKey.
    HARDWAREKEY_PURPOSE_DS,
} hardwarekey_purpose_t;

// The handle is portable: it holds a PSA key id. How that id gets created --
// which hardware key store, which slot -- is the one port-specific step, done
// when the port populates its per-slot HardwareKey objects at startup.
typedef struct {
    mp_obj_base_t base;
    psa_key_id_t key_id;
    mp_int_t key_slot;
    hardwarekey_purpose_t purpose;
    bool exportable;
    // Name this key is exposed under in `board` (e.g. MP_QSTR_EFUSE_KEY0), for
    // repr(). MP_QSTRnull if the object was not placed in `board`.
    qstr name;
} hardwarekey_hardwarekey_obj_t;

// HardwareKey objects are created by the port at startup, one per hardware key
// slot, and placed in `board`; application code never constructs them. The
// per-port startup code fills in key_id, key_slot, purpose, exportable and name.
// An HMAC key is used by passing the object to hmac.new(); a DS key is used by
// passing the object to
// cryptography.hazmat.primitives.asymmetric.rsa.load_digital_signature_key(),
// which returns a DigitalSignatureKey (see
// shared-module/cryptography/hazmat/primitives/asymmetric/rsa/DigitalSignatureKey.h).

mp_int_t common_hal_hardwarekey_hardwarekey_get_key_slot(hardwarekey_hardwarekey_obj_t *self);
hardwarekey_purpose_t common_hal_hardwarekey_hardwarekey_get_purpose(hardwarekey_hardwarekey_obj_t *self);
bool common_hal_hardwarekey_hardwarekey_get_exportable(hardwarekey_hardwarekey_obj_t *self);
// The PSA key id, for hmac.new(). 0 if the slot is unused.
psa_key_id_t common_hal_hardwarekey_hardwarekey_get_key_id(hardwarekey_hardwarekey_obj_t *self);
