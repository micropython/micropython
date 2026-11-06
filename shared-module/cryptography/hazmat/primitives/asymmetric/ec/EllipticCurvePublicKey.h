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

// Produced only by
// cryptography.hazmat.primitives.asymmetric.ec.EllipticCurvePublicKey.from_encoded_point().
// Unlike DigitalSignatureKey, this holds no secret material and needs no
// port-specific driver: importing a public point and verifying against it
// are both plain, portable PSA crypto calls, so this type is entirely
// portable (no common-hal split).
typedef struct {
    mp_obj_base_t base;
    // Backref to the curve singleton this was loaded for, for the `curve`
    // property. Always cryptography_hazmat_primitives_asymmetric_ec_secp256r1_obj
    // today, but kept as a field (not hardcoded in the property getter) so a
    // second curve, if one is ever added, doesn't need a new key type.
    mp_obj_t curve;
    // The uncompressed SEC1 point (0x04 || X || Y) this key was constructed
    // from, kept verbatim for public_bytes(). Always 65 bytes for P-256.
    uint8_t point[65];
    psa_key_id_t key_id;
} cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_obj_t;

// Validates point_len/point (must be a 65-byte 0x04-prefixed SEC1 point for
// P-256) and imports it into PSA as a PSA_KEY_TYPE_ECC_PUBLIC_KEY. Raises
// ValueError on a malformed point.
void common_hal_cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_construct(
    cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_obj_t *self,
    mp_obj_t curve, const uint8_t *point, size_t point_len);

// psa_verify_hash() against self->key_id, after converting `signature` from
// DER to the raw r||s format PSA expects. `digest` must be a 32-byte SHA-256
// digest (the Prehashed contract) -- raises ValueError if it isn't that
// length, or cryptography.exceptions.InvalidSignature if the signature is
// malformed or does not verify.
void common_hal_cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_verify(
    cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_obj_t *self,
    const uint8_t *signature, size_t signature_len, const uint8_t *digest, size_t digest_len);
