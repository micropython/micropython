// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mikey Sklar for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Operation context of the RP2350 SHA-256 PSA driver. It is a member of the PSA
// hash operation union (psa/crypto_driver_contexts_primitives.h), so it is
// included from inside psa/crypto.h and must not include it.

#include "mbedtls/private/sha256.h"

typedef struct {
    // Software SHA-256 state, used whenever the operation doesn't own the block.
    mbedtls_sha256_context sha256;
} rp2_sha256_psa_hash_operation_t;
