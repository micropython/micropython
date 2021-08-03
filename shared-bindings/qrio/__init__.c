/*
 * This file is part of the CircuitPython project, https://github.com/adafruit/circuitpython
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Jeff Epler
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

#include "shared-bindings/qrio/__init__.h"
#include "shared-bindings/qrio/QRDecoder.h"
#include "py/obj.h"
#include "py/enum.h"

//| """`qrio` module.
//|
//| Provides the `QRDecoder` object."""
//|

//| class PixelPolicy:
//|     EVERY_BYTE: object
//|     """The input buffer to `QRDecoder.decode` consists of greyscale values in every byte"""
//|
//|     EVEN_BYTES: object
//|     """The input buffer to `QRDecoder.decode` consists of greyscale values in positions 0, 2, ..., and ignored bytes in positions 1, 3, ..."""

//|     ODD_BYTES: object
//|     """The input buffer to `QRDecoder.decode` consists of greyscale values in positions 1, 3, ..., and ignored bytes in positions 0, 2, ..."""
//|

MAKE_ENUM_VALUE(qrio_pixel_policy_type, qrio_pixel_policy, EVERY_BYTE, QRIO_EVERY_BYTE);
MAKE_ENUM_VALUE(qrio_pixel_policy_type, qrio_pixel_policy, EVEN_BYTES, QRIO_EVEN_BYTES);
MAKE_ENUM_VALUE(qrio_pixel_policy_type, qrio_pixel_policy, ODD_BYTES, QRIO_EVEN_BYTES);

MAKE_ENUM_MAP(qrio_pixel_policy) {
    MAKE_ENUM_MAP_ENTRY(qrio_pixel_policy, EVERY_BYTE),
    MAKE_ENUM_MAP_ENTRY(qrio_pixel_policy, EVEN_BYTES),
    MAKE_ENUM_MAP_ENTRY(qrio_pixel_policy, ODD_BYTES),
};
STATIC MP_DEFINE_CONST_DICT(qrio_pixel_policy_locals_dict, qrio_pixel_policy_locals_table);

MAKE_PRINTER(qrio, qrio_pixel_policy);

MAKE_ENUM_TYPE(qrio, PixelPolicy, qrio_pixel_policy);

//| class qrinfo:
//|     """Information about a decoded QR code"""
//|
//|     payload: bytes
//|     """The content of the QR code"""
//|
//|     data_type: Union[str, int]
//|     """The encoding of the payload as a string (if a standard encoding) or int (if not standard)"""

const mp_obj_namedtuple_type_t qrio_qrinfo_type_obj = {
    .base = {
        .base = {
            .type = &mp_type_type
        },
        .flags = MP_TYPE_FLAG_EXTENDED,
        .name = MP_QSTR_qrinfo,
        .print = namedtuple_print,
        .parent = &mp_type_tuple,
        .make_new = namedtuple_make_new,
        .attr = namedtuple_attr,
        MP_TYPE_EXTENDED_FIELDS(
            .unary_op = mp_obj_tuple_unary_op,
            .binary_op = mp_obj_tuple_binary_op,
            .subscr = mp_obj_tuple_subscr,
            .getiter = mp_obj_tuple_getiter,
            ),
    },
    .n_fields = 2,
    .fields = {
        MP_QSTR_payload,
        MP_QSTR_data_type,
    },
};

STATIC const mp_rom_map_elem_t qrio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_qrio) },
    { MP_ROM_QSTR(MP_QSTR_qrinfo), MP_ROM_PTR(&qrio_qrinfo_type_obj) },
    { MP_ROM_QSTR(MP_QSTR_QRDecoder), MP_ROM_PTR(&qrio_qrdecoder_type_obj) },
    { MP_ROM_QSTR(MP_QSTR_PixelPolicy), MP_ROM_PTR(&qrio_pixel_policy_type) },
};

STATIC MP_DEFINE_CONST_DICT(qrio_module_globals, qrio_module_globals_table);

const mp_obj_module_t qrio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&qrio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_qrio, qrio_module, CIRCUITPY_QRIO);
