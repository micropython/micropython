// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "mbedtls/sha1.h"

typedef struct {
    mp_obj_base_t base;
    union {
        mbedtls_sha1_context sha1;
    };
    // Of MBEDTLS_SSL_HASH_*
    uint8_t hash_type;
} hashlib_hash_obj_t;
