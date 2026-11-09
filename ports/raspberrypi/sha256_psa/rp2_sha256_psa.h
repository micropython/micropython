// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mikey Sklar for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// PSA transparent driver for SHA-256 on the RP2350 SHA-256 block. mbedtls's hash
// dispatch tries it before the builtin driver; lib/mbedtls_config/psa_dispatch.patch
// adds it there and adds its context to the PSA hash operation union.

#include "psa/crypto.h"
#include "rp2_sha256_psa_ctx.h"

// Returns PSA_ERROR_NOT_SUPPORTED for anything but PSA_ALG_SHA_256, so the
// dispatch falls through to the builtin driver.
psa_status_t rp2_sha256_psa_hash_setup(rp2_sha256_psa_hash_operation_t *op, psa_algorithm_t alg);
psa_status_t rp2_sha256_psa_hash_update(rp2_sha256_psa_hash_operation_t *op,
    const uint8_t *input, size_t input_length);
psa_status_t rp2_sha256_psa_hash_finish(rp2_sha256_psa_hash_operation_t *op,
    uint8_t *hash, size_t hash_size, size_t *hash_length);
psa_status_t rp2_sha256_psa_hash_abort(rp2_sha256_psa_hash_operation_t *op);
psa_status_t rp2_sha256_psa_hash_clone(const rp2_sha256_psa_hash_operation_t *source,
    rp2_sha256_psa_hash_operation_t *target);
psa_status_t rp2_sha256_psa_hash_compute(psa_algorithm_t alg,
    const uint8_t *input, size_t input_length,
    uint8_t *hash, size_t hash_size, size_t *hash_length);

// Forget which operation owns the block and release its lock. Call when the VM
// ends, since no operation outlives it. Doesn't allocate or raise.
void rp2_sha256_psa_reset(void);
