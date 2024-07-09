// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/stream.h"
#include "shared-bindings/displayio/__init__.h"

typedef struct gifio_gifwriter {
    mp_obj_base_t base;
    mp_obj_t *file;
    const mp_stream_p_t *file_proto;
    displayio_colorspace_t colorspace;
    int width, height;
    int error;
    uint8_t *data;
    size_t cur, size;
    bool own_file;
    bool byteswap;
    bool dither;
} gifio_gifwriter_t;
