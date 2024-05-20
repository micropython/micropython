// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/nvm/ByteArray.h"

extern const mp_obj_type_t nvm_bytearray_type;

uint32_t common_hal_nvm_bytearray_get_length(const nvm_bytearray_obj_t *self);

bool common_hal_nvm_bytearray_set_bytes(const nvm_bytearray_obj_t *self,
    uint32_t start_index, uint8_t *values, uint32_t len);
// len and values are intentionally swapped to signify values is an output and
// also leverage the compiler to validate uses are expected.
void common_hal_nvm_bytearray_get_bytes(const nvm_bytearray_obj_t *self,
    uint32_t start_index, uint32_t len, uint8_t *values);
