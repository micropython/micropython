// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/fontio/__init__.h"
#include "shared-bindings/fontio/BuiltinFont.h"
#include "shared-bindings/fontio/Glyph.h"

//| """Core font related data structures
//|
//| .. note:: This module is intended only for low-level usage.  For working with
//|     fonts in CircuitPython see the `adafruit_bitmap_font library
//|     <https://github.com/adafruit/Adafruit_CircuitPython_Bitmap_Font>`_.
//|     For information on creating custom fonts for use in CircuitPython, see
//|     `this Learn guide <https://learn.adafruit.com/custom-fonts-for-pyportal-circuitpython-display>`_
//|
//| """

static const mp_rom_map_elem_t fontio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_fontio) },
    { MP_ROM_QSTR(MP_QSTR_BuiltinFont), MP_ROM_PTR(&fontio_builtinfont_type) },
    { MP_ROM_QSTR(MP_QSTR_Glyph), MP_ROM_PTR(&fontio_glyph_type) },
};

static MP_DEFINE_CONST_DICT(fontio_module_globals, fontio_module_globals_table);

const mp_obj_module_t fontio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&fontio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_fontio, fontio_module);
