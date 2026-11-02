// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/enum.h"

extern const mp_obj_type_t mipicsi_pixel_format_type;
extern const mp_obj_type_t mipicsi_bayer_order_type;
extern const cp_enum_obj_t pixel_format_RGB565_obj;

typedef enum {
    MIPICSI_FORMAT_RAW8,
    MIPICSI_FORMAT_RAW10,
    MIPICSI_FORMAT_RAW12,
    MIPICSI_FORMAT_RGB565,
    MIPICSI_FORMAT_RGB888,
    MIPICSI_FORMAT_YUV422,
    MIPICSI_FORMAT_GRAYSCALE,
} mipicsi_pixel_format_t;

typedef enum {
    MIPICSI_BAYER_ORDER_BGGR,
    MIPICSI_BAYER_ORDER_GBRG,
    MIPICSI_BAYER_ORDER_GRBG,
    MIPICSI_BAYER_ORDER_RGGB,
} mipicsi_bayer_order_t;

mipicsi_pixel_format_t validate_pixel_format(mp_obj_t obj, qstr arg_name);
mipicsi_bayer_order_t validate_bayer_order(mp_obj_t obj, qstr arg_name);
