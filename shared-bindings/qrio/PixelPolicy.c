// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Jeff Epler
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/qrio/__init__.h"
#include "shared-bindings/qrio/PixelPolicy.h"
#include "py/obj.h"
#include "py/enum.h"

//| class PixelPolicy:
//|     EVERY_BYTE: PixelPolicy
//|     """The input buffer to `QRDecoder.decode` consists of greyscale values in every byte"""
//|
//|     EVEN_BYTES: PixelPolicy
//|     """The input buffer to `QRDecoder.decode` consists of greyscale values in positions 0, 2, …, and ignored bytes in positions 1, 3, …. This can decode directly from YUV images where the even bytes hold the Y (luminance) data."""
//|
//|     ODD_BYTES: PixelPolicy
//|     """The input buffer to `QRDecoder.decode` consists of greyscale values in positions 1, 3, …, and ignored bytes in positions 0, 2, …. This can decode directly from YUV images where the odd bytes hold the Y (luminance) data"""
//|
//|     RGB565_SWAPPED: PixelPolicy
//|     """The input buffer to `QRDecoder.decode` consists of RGB565 values in byte-swapped order. Most cameras produce data in byte-swapped order. The green component is used."""
//|
//|     RGB565: PixelPolicy
//|     """The input buffer to `QRDecoder.decode` consists of RGB565 values in native order. The green component is used."""
//|

MAKE_ENUM_VALUE(qrio_pixel_policy_type, qrio_pixel_policy, EVERY_BYTE, QRIO_EVERY_BYTE);
MAKE_ENUM_VALUE(qrio_pixel_policy_type, qrio_pixel_policy, RGB565, QRIO_RGB565);
MAKE_ENUM_VALUE(qrio_pixel_policy_type, qrio_pixel_policy, RGB565_SWAPPED, QRIO_RGB565_SWAPPED);
MAKE_ENUM_VALUE(qrio_pixel_policy_type, qrio_pixel_policy, EVEN_BYTES, QRIO_EVEN_BYTES);
MAKE_ENUM_VALUE(qrio_pixel_policy_type, qrio_pixel_policy, ODD_BYTES, QRIO_EVEN_BYTES);

MAKE_ENUM_MAP(qrio_pixel_policy) {
    MAKE_ENUM_MAP_ENTRY(qrio_pixel_policy, EVERY_BYTE),
    MAKE_ENUM_MAP_ENTRY(qrio_pixel_policy, RGB565),
    MAKE_ENUM_MAP_ENTRY(qrio_pixel_policy, RGB565_SWAPPED),
    MAKE_ENUM_MAP_ENTRY(qrio_pixel_policy, EVEN_BYTES),
    MAKE_ENUM_MAP_ENTRY(qrio_pixel_policy, ODD_BYTES),
};
static MP_DEFINE_CONST_DICT(qrio_pixel_policy_locals_dict, qrio_pixel_policy_locals_table);

MAKE_PRINTER(qrio, qrio_pixel_policy);

MAKE_ENUM_TYPE(qrio, PixelPolicy, qrio_pixel_policy);
