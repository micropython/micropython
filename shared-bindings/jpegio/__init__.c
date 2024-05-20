// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "shared-bindings/jpegio/JpegDecoder.h"

//|
//| """Support for JPEG image decoding"""
//|

static const mp_rom_map_elem_t jpegio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_jpegio) },
    { MP_ROM_QSTR(MP_QSTR_JpegDecoder), MP_ROM_PTR(&jpegio_jpegdecoder_type) },
};

static MP_DEFINE_CONST_DICT(jpegio_module_globals, jpegio_module_globals_table);

const mp_obj_module_t jpegio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&jpegio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_jpegio, jpegio_module);
