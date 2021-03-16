/*
 * This file is part of the Circuit Python project, https://github.com/adafruit/circuitpython
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Roy Hooper
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

#include "py/obj.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "py/objproperty.h"

#include "shared-bindings/_pixelbuf/__init__.h"
#include "shared-bindings/_pixelbuf/PixelBuf.h"


//| """A fast RGB(W) pixel buffer library for like NeoPixel and DotStar
//|
//| The `_pixelbuf` module provides the :py:class:`PixelBuf` class to accelerate
//| RGB(W) strip/matrix manipulation, such as DotStar and Neopixel.
//|
//| Byteorders are configured with strings, such as "RGB" or "RGBD"."""
// TODO: Pull in docs from pypixelbuf.

//| def colorwheel(n: float) -> int:
//|     """C implementation of the common wheel() function found in many examples.
//|     Returns the colorwheel RGB value as an integer value for n (usable in :py:class:`PixelBuf`, neopixel, and dotstar)."""
//|     ...
//|
//| def wheel(n: float) -> int:
//|     """Use of wheel() is deprecated. Please use colorwheel()."""
//|

STATIC mp_obj_t pixelbuf_colorwheel(mp_obj_t n) {
    return MP_OBJ_NEW_SMALL_INT(colorwheel(MP_OBJ_IS_SMALL_INT(n) ? MP_OBJ_SMALL_INT_VALUE(n) : mp_obj_get_float(n)));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pixelbuf_colorwheel_obj, pixelbuf_colorwheel);

const int32_t colorwheel(float pos) {
    if (pos > 255) {
        pos = pos - ((uint32_t)(pos / 256) * 256);
    }
    if (pos < 85) {
        return (uint8_t)(255 - (pos * 3)) << 16 | (uint8_t)(pos * 3) << 8;
    } else if (pos < 170) {
        pos -= 85;
        return (uint8_t)(255 - (pos * 3)) << 8 | (uint8_t)(pos * 3);
    } else {
        pos -= 170;
        return (uint8_t)(pos * 3) << 16 | (uint8_t)(255 - (pos * 3));
    }
}

STATIC const mp_rom_map_elem_t pixelbuf_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__pixelbuf) },
    { MP_ROM_QSTR(MP_QSTR_PixelBuf), MP_ROM_PTR(&pixelbuf_pixelbuf_type) },
    { MP_ROM_QSTR(MP_QSTR_wheel), MP_ROM_PTR(&pixelbuf_colorwheel_obj) },
    { MP_ROM_QSTR(MP_QSTR_colorwheel), MP_ROM_PTR(&pixelbuf_colorwheel_obj) },
};

STATIC MP_DEFINE_CONST_DICT(pixelbuf_module_globals, pixelbuf_module_globals_table);

const mp_obj_module_t pixelbuf_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&pixelbuf_module_globals,
};
