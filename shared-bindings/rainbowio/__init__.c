/*
 * This file is part of the CircuitPython project, https://github.com/adafruit/circuitpython
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Kattni Rembor
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

#include "shared-bindings/rainbowio/__init__.h"
#include "py/mpconfig.h"
#include "py/obj.h"
//| """`rainbowio` module.
//|
//| Provides the `colorwheel()` function."""
//|
//| def colorwheel(n: float) -> int:
//|     """C implementation of the common colorwheel() function found in many examples.
//|     Returns the colorwheel RGB value as an integer value for n (usable in neopixel and dotstar)."""
//|     ...
//|
STATIC mp_obj_t rainbowio_colorwheel(mp_obj_t n) {
    return MP_OBJ_NEW_SMALL_INT(colorwheel(mp_obj_is_small_int(n) ? MP_OBJ_SMALL_INT_VALUE(n) : mp_obj_get_float(n)));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(rainbowio_colorwheel_obj, rainbowio_colorwheel);

STATIC const mp_rom_map_elem_t rainbowio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_rainbowio) },
    { MP_ROM_QSTR(MP_QSTR_colorwheel), MP_ROM_PTR(&rainbowio_colorwheel_obj) },
};

STATIC MP_DEFINE_CONST_DICT(rainbowio_module_globals, rainbowio_module_globals_table);

const mp_obj_module_t rainbowio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&rainbowio_module_globals,
};
