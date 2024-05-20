// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#if CIRCUITPY_HASHLIB_MBEDTLS
#include "shared-module/hashlib/Hash.h"
#else
#include "common-hal/hashlib/Hash.h"
#endif

extern const mp_obj_type_t hashlib_hash_type;

// So that new can call it when given data.
mp_obj_t hashlib_hash_update(mp_obj_t self_in, mp_obj_t buf_in);

void common_hal_hashlib_hash_update(hashlib_hash_obj_t *self, const uint8_t *data, size_t datalen);
void common_hal_hashlib_hash_digest(hashlib_hash_obj_t *self, uint8_t *data, size_t datalen);
size_t common_hal_hashlib_hash_get_digest_size(hashlib_hash_obj_t *self);
