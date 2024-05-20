// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Kattni Rembor
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/rainbowio/__init__.h"
#include "py/mpconfig.h"
#include "py/obj.h"
//| """`rainbowio` module.
//|
//| Provides the `colorwheel()` function."""
//|
//| def colorwheel(n: float) -> int:
//|     """C implementation of the common colorwheel() function found in many examples.
//|     Returns the colorwheel RGB value as an integer value for n (usable in neopixel and dotstar).
//|     """
//|     ...
//|
static mp_obj_t rainbowio_colorwheel(mp_obj_t n) {
    mp_float_t f = mp_obj_get_float(n);
    return MP_OBJ_NEW_SMALL_INT(colorwheel(f));
}
static MP_DEFINE_CONST_FUN_OBJ_1(rainbowio_colorwheel_obj, rainbowio_colorwheel);

static const mp_rom_map_elem_t rainbowio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_rainbowio) },
    { MP_ROM_QSTR(MP_QSTR_colorwheel), MP_ROM_PTR(&rainbowio_colorwheel_obj) },
};

static MP_DEFINE_CONST_DICT(rainbowio_module_globals, rainbowio_module_globals_table);

const mp_obj_module_t rainbowio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&rainbowio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_rainbowio, rainbowio_module);
