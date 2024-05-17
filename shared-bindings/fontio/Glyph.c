// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/fontio/Glyph.h"

#include <stdint.h>

//| class Glyph:
//|     """Storage of glyph info"""
//|
//|     def __init__(
//|         self,
//|         bitmap: displayio.Bitmap,
//|         tile_index: int,
//|         width: int,
//|         height: int,
//|         dx: int,
//|         dy: int,
//|         shift_x: int,
//|         shift_y: int,
//|     ) -> None:
//|         """Named tuple used to capture a single glyph and its attributes.
//|
//|         :param bitmap: the bitmap including the glyph
//|         :param tile_index: the tile index within the bitmap
//|         :param width: the width of the glyph's bitmap
//|         :param height: the height of the glyph's bitmap
//|         :param dx: x adjustment to the bitmap's position
//|         :param dy: y adjustment to the bitmap's position
//|         :param shift_x: the x difference to the next glyph
//|         :param shift_y: the y difference to the next glyph"""
//|         ...
//|
const mp_obj_namedtuple_type_t fontio_glyph_type = {
    NAMEDTUPLE_TYPE_BASE_AND_SLOTS(MP_QSTR_Glyph),
    .n_fields = 8,
    .fields = {
        MP_QSTR_bitmap,
        MP_QSTR_tile_index,
        MP_QSTR_width,
        MP_QSTR_height,
        MP_QSTR_dx,
        MP_QSTR_dy,
        MP_QSTR_shift_x,
        MP_QSTR_shift_y
    },
};
