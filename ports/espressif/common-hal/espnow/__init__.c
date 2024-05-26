// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 MicroDev
//
// SPDX-License-Identifier: MIT

#include "common-hal/espnow/__init__.h"

#include "py/runtime.h"

// Return C pointer to byte memory string/bytes/bytearray in obj.
// Raise ValueError if the length does not match expected len.
const uint8_t *common_hal_espnow_get_bytes_len(mp_obj_t obj, size_t len) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(obj, &bufinfo, MP_BUFFER_READ);
    mp_arg_validate_length(bufinfo.len, len, MP_QSTR_buffer);
    return (uint8_t *)bufinfo.buf;
}
