// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/rgbmatrix/RGBMatrix.h"

//| """Low-level routines for bitbanged LED matrices
//|
//| For more information about working with RGB matrix panels in CircuitPython, see
//| `the dedicated learn guide <https://learn.adafruit.com/rgb-led-matrices-matrix-panels-with-circuitpython>`_.
//| """

static const mp_rom_map_elem_t rgbmatrix_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_rgbmatrix) },
    { MP_ROM_QSTR(MP_QSTR_RGBMatrix), MP_ROM_PTR(&rgbmatrix_RGBMatrix_type) },
};

static MP_DEFINE_CONST_DICT(rgbmatrix_module_globals, rgbmatrix_module_globals_table);

const mp_obj_module_t rgbmatrix_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&rgbmatrix_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_rgbmatrix, rgbmatrix_module);
