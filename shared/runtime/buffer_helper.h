// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_LIB_UTILS_BUFFER_HELPER_H
#define MICROPY_INCLUDED_LIB_UTILS_BUFFER_HELPER_H

#include <stdint.h>
#include <string.h>

void normalize_buffer_bounds(int32_t *start, int32_t end, size_t *length);

#endif  // MICROPY_INCLUDED_LIB_UTILS_BUFFER_HELPER_H
