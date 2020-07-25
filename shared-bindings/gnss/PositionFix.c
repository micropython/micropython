// SPDX-FileCopyrightText: Sony Semiconductor Solutions Corporation
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/gnss/PositionFix.h"

//| class PositionFix:
//|     """Position fix mode"""
//|
//|     def __init__(self) -> None:
//|         """Enum-like class to define the position fix mode."""
//|
//|     INVALID: PositionFix
//|     """No measurement."""
//|
//|     FIX_2D: PositionFix
//|     """2D fix."""
//|
//|     FIX_3D: PositionFix
//|     """3D fix."""
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
