// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/enum.h"
#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/mipicsi/__init__.h"

//| class BayerOrder:
//|     """The Bayer element order of a sensor's RAW output, in the sequence a 2x2
//|     pixel block transmits (top-left first, reading order)"""
//|
//|     BGGR: BayerOrder
//|     """The first element is Blue, then Green. The next row starts with
//|     Green, then Red"""
//|
//|     GBRG: BayerOrder
//|     """The first element is Green, then Blue. The next row starts with
//|     Red, then Green"""
//|
//|     GRBG: BayerOrder
//|     """The first element is Green, then Red. The next row starts with
//|     Blue, then Green"""
//|
//|     RGGB: BayerOrder
//|     """The first element is Red, then Green. The next row starts with
//|     Green, then Blue"""
//|

MAKE_ENUM_VALUE(mipicsi_bayer_order_type, bayer_order, BGGR, MIPICSI_BAYER_ORDER_BGGR);
MAKE_ENUM_VALUE(mipicsi_bayer_order_type, bayer_order, GBRG, MIPICSI_BAYER_ORDER_GBRG);
MAKE_ENUM_VALUE(mipicsi_bayer_order_type, bayer_order, GRBG, MIPICSI_BAYER_ORDER_GRBG);
MAKE_ENUM_VALUE(mipicsi_bayer_order_type, bayer_order, RGGB, MIPICSI_BAYER_ORDER_RGGB);

MAKE_ENUM_MAP(mipicsi_bayer_order) {
    MAKE_ENUM_MAP_ENTRY(bayer_order, BGGR),
    MAKE_ENUM_MAP_ENTRY(bayer_order, GBRG),
    MAKE_ENUM_MAP_ENTRY(bayer_order, GRBG),
    MAKE_ENUM_MAP_ENTRY(bayer_order, RGGB),
};

static MP_DEFINE_CONST_DICT(mipicsi_bayer_order_locals_dict, mipicsi_bayer_order_locals_table);
MAKE_PRINTER(mipicsi, mipicsi_bayer_order);
MAKE_ENUM_TYPE(mipicsi, BayerOrder, mipicsi_bayer_order);

mipicsi_bayer_order_t validate_bayer_order(mp_obj_t obj, qstr arg_name) {
    return cp_enum_value(&mipicsi_bayer_order_type, obj, arg_name);
}
