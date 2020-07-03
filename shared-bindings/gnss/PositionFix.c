/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright 2020 Sony Semiconductor Solutions Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "shared-bindings/gnss/PositionFix.h"

//| class PositionFix:
//|     """Position fix mode"""
//|
//|     def __init__(self, ):
//|         """Enum-like class to define the position fix mode."""
//|
//|     INVALID: Any = ...
//|     """No measurement.
//|
//|     :type gnss.PositionFix:"""
//|
//|     FIX_2D: Any = ...
//|     """2D fix.
//|
//|     :type gnss.PositionFix:"""
//|
//|     FIX_3D: Any = ...
//|     """3D fix.
//|
//|     :type gnss.PositionFix:"""
//|
const mp_obj_type_t gnss_positionfix_type;

const gnss_positionfix_obj_t gnss_positionfix_invalid_obj = {
    { &gnss_positionfix_type },
};

const gnss_positionfix_obj_t gnss_positionfix_fix2d_obj = {
    { &gnss_positionfix_type },
};

const gnss_positionfix_obj_t gnss_positionfix_fix3d_obj = {
    { &gnss_positionfix_type },
};

gnss_positionfix_t gnss_positionfix_obj_to_type(mp_obj_t obj) {
    gnss_positionfix_t posfix = POSITIONFIX_INVALID;
    if (obj == MP_ROM_PTR(&gnss_positionfix_fix2d_obj)) {
        posfix = POSITIONFIX_2D;
    } else if (obj == MP_ROM_PTR(&gnss_positionfix_fix3d_obj)) {
        posfix = POSITIONFIX_3D;
    }
    return posfix;
}

mp_obj_t gnss_positionfix_type_to_obj(gnss_positionfix_t posfix) {
    switch (posfix) {
        case POSITIONFIX_2D:
            return (mp_obj_t)MP_ROM_PTR(&gnss_positionfix_fix2d_obj);
        case POSITIONFIX_3D:
            return (mp_obj_t)MP_ROM_PTR(&gnss_positionfix_fix3d_obj);
        case POSITIONFIX_INVALID:
        default:
            return (mp_obj_t)MP_ROM_PTR(&gnss_positionfix_invalid_obj);
    }
}

STATIC const mp_rom_map_elem_t gnss_positionfix_locals_dict_table[] = {
    {MP_ROM_QSTR(MP_QSTR_INVALID), MP_ROM_PTR(&gnss_positionfix_invalid_obj)},
    {MP_ROM_QSTR(MP_QSTR_FIX_2D), MP_ROM_PTR(&gnss_positionfix_fix2d_obj)},
    {MP_ROM_QSTR(MP_QSTR_FIX_3D), MP_ROM_PTR(&gnss_positionfix_fix3d_obj)},
};
STATIC MP_DEFINE_CONST_DICT(gnss_positionfix_locals_dict, gnss_positionfix_locals_dict_table);

STATIC void gnss_positionfix_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    qstr posfix = MP_QSTR_INVALID;
    if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&gnss_positionfix_fix2d_obj)) {
        posfix = MP_QSTR_FIX_2D;
    } else if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&gnss_positionfix_fix3d_obj)) {
        posfix = MP_QSTR_FIX_3D;
    }
    mp_printf(print, "%q.%q.%q", MP_QSTR_gnss, MP_QSTR_PositionFix, posfix);
}

const mp_obj_type_t gnss_positionfix_type = {
    { &mp_type_type },
    .name = MP_QSTR_PositionFix,
    .print = gnss_positionfix_print,
    .locals_dict = (mp_obj_t)&gnss_positionfix_locals_dict,
};
