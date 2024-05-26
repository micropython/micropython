// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared/runtime/buffer_helper.h"

void normalize_buffer_bounds(int32_t *start, int32_t end, size_t *length) {
    if (end < 0) {
        end += *length;
    } else if (((size_t)end) > *length) {
        end = *length;
    }
    if (*start < 0) {
        *start += *length;
    }
    if (end < *start) {
        *length = 0;
    } else {
        *length = end - *start;
    }
}
