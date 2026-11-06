// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mikey Sklar for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// PSA transparent driver for SHA-256 on the RP2350 SHA-256 block. The PSA
// dispatch (generated from CircuitPython's copy of the driver-wrapper template)
// tries it before the builtin driver. It keeps its state in the builtin driver's
// operation struct, so the PSA operation union needs no new member.

#include "psa/crypto.h"
#include "mbedtls/private/crypto_builtin_primitives.h"

// Returns PSA_ERROR_NOT_SUPPORTED for anything but PSA_ALG_SHA_256, so the
// dispatch falls through to the builtin driver.
psa_status_t rp2_sha256_psa_hash_setup(mbedtls_psa_hash_operation_t *op, psa_algorithm_t alg);
psa_status_t rp2_sha256_psa_hash_update(mbedtls_psa_hash_operation_t *op,
    const uint8_t *input, size_t input_length);
psa_status_t rp2_sha256_psa_hash_finish(mbedtls_psa_hash_operation_t *op,
    uint8_t *hash, size_t hash_size, size_t *hash_length);
psa_status_t rp2_sha256_psa_hash_abort(mbedtls_psa_hash_operation_t *op);
psa_status_t rp2_sha256_psa_hash_clone(const mbedtls_psa_hash_operation_t *source,
    mbedtls_psa_hash_operation_t *target);
psa_status_t rp2_sha256_psa_hash_compute(psa_algorithm_t alg,
    const uint8_t *input, size_t input_length,
    uint8_t *hash, size_t hash_size, size_t *hash_length);

// Forget which operation owns the block and release its lock. Call when the VM
// ends, since no operation outlives it. Doesn't allocate or raise.
void rp2_sha256_psa_reset(void);
