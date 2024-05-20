// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/memorymap/AddressRange.h"

extern const mp_obj_type_t memorymap_addressrange_type;

void common_hal_memorymap_addressrange_construct(memorymap_addressrange_obj_t *self, uint8_t *start_address, size_t length);

size_t common_hal_memorymap_addressrange_get_length(const memorymap_addressrange_obj_t *self);

void common_hal_memorymap_addressrange_set_bytes(const memorymap_addressrange_obj_t *self,
    size_t start_index, uint8_t *values, size_t len);

// len and values are intentionally swapped to signify values is an output and
// also leverage the compiler to validate uses are expected.
void common_hal_memorymap_addressrange_get_bytes(const memorymap_addressrange_obj_t *self,
    size_t start_index, size_t len, uint8_t *values);
