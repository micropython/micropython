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

#include "shared-module/hardwarekey/HardwareKey.h"

// Produced only by cryptography.hazmat.primitives.asymmetric.rsa.load_digital_signature_key().
// Holds the state a Digital Signature (RSA) key needs that a generic
// HardwareKey has no business carrying: the decrypted-params-derived key
// size, which PSA algorithm the key has committed to, and the PSA key id once
// it's actually been imported.
typedef struct {
    mp_obj_base_t base;
    // Backref to the HardwareKey this was loaded from. Used internally (repr,
    // error messages, indexing the port's per-slot cache via
    // common_hal_hardwarekey_hardwarekey_get_key_slot()) -- not exposed to Python.
    mp_obj_t source_key;
    mp_int_t key_size;
    // The PSA algorithm key_id was imported under, once ensure_algorithm() has
    // run for the first time. PSA_ALG_NONE (0) until then. Reusing an RSA key
    // under more than one algorithm is a real cryptographic risk, not just a
    // PSA API nicety, so this key commits to the first algorithm it's
    // actually used with.
    psa_algorithm_t committed_alg;
    psa_key_id_t key_id;
    // Snapshot, at construction time, of the port's per-slot cache generation
    // counter. A slot's cache is shared C-level state, not owned by any one
    // DigitalSignatureKey object; loading a new key for the same slot bumps the
    // generation, so an older, not-yet-committed object can tell its cached
    // ds_params were replaced out from under it instead of silently importing
    // the wrong key material. See ensure_algorithm() below.
    uint32_t generation;
} cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_obj_t;

mp_int_t common_hal_cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_get_key_size(
    cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_obj_t *self);
// The PSA key id, for ssl's mbedtls_pk_wrap_psa(). 0 until ensure_algorithm()
// has committed and imported a key.
psa_key_id_t common_hal_cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_get_key_id(
    cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_obj_t *self);

// Port-specific: only the port knows how to turn a ds_params blob into a PSA
// key reference. Caches ds_params and computes key_size, but does NOT import a
// PSA key yet -- see ensure_algorithm() below for why. Raises ValueError on a
// malformed blob.
void common_hal_cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_construct(
    cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_obj_t *self,
    hardwarekey_hardwarekey_obj_t *source_key, const uint8_t *ds_params, size_t ds_params_len);

// Port-specific: on the first call, imports self->key_id under exactly `alg`
// and `usage`. On a later call, either confirms `alg` matches what was already
// committed (no-op) or raises ValueError -- this key already committed to a
// different algorithm. Also raises ValueError if this object's cached
// ds_params were superseded by a later load_digital_signature_key() call for
// the same key slot before this object ever committed (see `generation`
// above).
void common_hal_cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_ensure_algorithm(
    cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_obj_t *self,
    psa_algorithm_t alg, psa_key_usage_t usage);

// Portable: psa_sign_message() against self->key_id. sig_out must be
// key_size / 8 bytes.
void common_hal_cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_sign(
    cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_obj_t *self,
    const uint8_t *data, size_t data_len, uint8_t *sig_out, size_t sig_out_len);

// Portable: psa_asymmetric_decrypt() against self->key_id. plaintext_out must
// be at least key_size / 8 bytes; the actual plaintext length (after padding
// removal) is returned via *output_len.
void common_hal_cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_decrypt(
    cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_obj_t *self,
    psa_algorithm_t alg, const uint8_t *ciphertext, size_t ciphertext_len,
    uint8_t *plaintext_out, size_t plaintext_out_size, size_t *output_len);
