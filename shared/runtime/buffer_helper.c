// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared/runtime/buffer_helper.h"

#include "py/binary.h"
#include "py/runtime.h"

void normalize_buffer_bounds(int32_t *start, int32_t end, size_t *length) {
    if (end < 0) {
        end += *length;
    } else if (((size_t)end) > *length) {
        end = *length;
    }
    if (*start < 0) {
        *start += *length;
        if (*start < 0) {
            *start = 0;
        }
    }
    if (end < *start) {
        *length = 0;
    } else {
        *length = end - *start;
    }
}

uint8_t *buffer_slice(mp_obj_t buffer, int32_t start, int32_t end, mp_uint_t flags, size_t *len) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buffer, &bufinfo, flags);
    int stride_in_bytes = mp_binary_get_size('@', bufinfo.typecode, NULL);
    size_t length = bufinfo.len / stride_in_bytes;
    normalize_buffer_bounds(&start, end, &length);
    *len = length * stride_in_bytes;
    return (uint8_t *)bufinfo.buf + start * stride_in_bytes;
}
