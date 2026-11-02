// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>
#include <string.h>

#include "py/obj.h"

void normalize_buffer_bounds(int32_t *start, int32_t end, size_t *length);

// buffer[start:end], with start and end in elements. Returns the slice's address and sets *len
// to its length in bytes.
uint8_t *buffer_slice(mp_obj_t buffer, int32_t start, int32_t end, mp_uint_t flags, size_t *len);
