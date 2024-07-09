// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Jeff Epler
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/qrio/__init__.h"
#include "shared-bindings/qrio/QRInfo.h"
#include "py/obj.h"
#include "py/enum.h"

//| class QRInfo:
//|     """Information about a decoded QR code"""
//|
//|     payload: bytes
//|     """The content of the QR code"""
//|
//|     data_type: Union[str, int]
//|     """The encoding of the payload as a string (if a standard encoding) or int (if not standard)"""
//|

const mp_obj_namedtuple_type_t qrio_qrinfo_type_obj = {
    NAMEDTUPLE_TYPE_BASE_AND_SLOTS(MP_QSTR_QRInfo),
    .n_fields = 2,
    .fields = {
        MP_QSTR_payload,
        MP_QSTR_data_type,
    },
};

//| class QRPosition:
//|     """Information about a non-decoded QR code"""
//|
//|     top_left_x: int
//|     """X coordinate of the top left corner"""
//|
//|     top_left_y: int
//|     """Y coordinate of the top left corner"""
//|
//|     top_right_x: int
//|     """X coordinate of the top right corner"""
//|
//|     top_right_y: int
//|     """Y coordinate of the top right corner"""
//|
//|     bottom_right_x: int
//|     """X coordinate of the bottom right corner"""
//|
//|     bottom_right_y: int
//|     """Y coordinate of the bottom right corner"""
//|
//|     bottom_left_x: int
//|     """X coordinate of the bottom left corner"""
//|
//|     bottom_left_y: int
//|     """Y coordinate of the bottom left corner"""
//|
//|     size: int
//|     """The number of bits the code contains"""
//|

const mp_obj_namedtuple_type_t qrio_qrposition_type_obj = {
    NAMEDTUPLE_TYPE_BASE_AND_SLOTS(MP_QSTR_QRPosition),
    .n_fields = 9,
    .fields = {
        MP_QSTR_top_left_x,
        MP_QSTR_top_left_y,
        MP_QSTR_top_right_x,
        MP_QSTR_top_right_y,
        MP_QSTR_bottom_right_x,
        MP_QSTR_bottom_right_y,
        MP_QSTR_bottom_left_x,
        MP_QSTR_bottom_left_y,
        MP_QSTR_size,
    },
};
