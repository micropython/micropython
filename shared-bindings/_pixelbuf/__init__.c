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

#include "types.h"
#include "__init__.h"

#include "PixelBuf.h"
#include "../../shared-module/_pixelbuf/PixelBuf.h"


//| :mod:`_pixelbuf` --- Fast RGB(W) pixel buffer and helpers
//| ===========================================================

//| .. module:: _pixelbuf
//|   :synopsis: A fast RGB(W) pixel buffer library for like NeoPixel and DotStar.
//|
//| The `_pixelbuf` module provides the :py:class:`PixelBuf` class to accelerate
//| RGB(W) strip/matrix manipulation, such as DotStar and Neopixel.
//|
//| Byteorders are configured with strings, such as "RGB" or "RGBD".
//| TODO: Pull in docs from pypixelbuf.

//| Libraries
//|
//| .. toctree::
//|     :maxdepth: 3
//|
//|     PixelBuf


//| .. function:: wheel(n)
//|
//|     C implementation of the common wheel() function found in many examples.
//|     Returns the colorwheel RGB value as an integer value for n (usable in :py:class:`PixelBuf`, neopixel, and dotstar).
//|

STATIC mp_obj_t pixelbuf_wheel(mp_obj_t n) {
    return MP_OBJ_NEW_SMALL_INT(colorwheel(MP_OBJ_IS_SMALL_INT(n) ? MP_OBJ_SMALL_INT_VALUE(n) : mp_obj_float_get(n)));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pixelbuf_wheel_obj, pixelbuf_wheel);

const int32_t colorwheel(float pos) {
    if (pos > 255) {
        pos = pos - ((uint32_t)(pos / 256) * 256);
    }
    if (pos < 85)
        return (uint8_t)(pos * 3) << 16 | (uint8_t)(255 - (pos * 3)) << 8;
    else if (pos < 170) {
        pos -= 85;
        return (uint8_t)(255 - (pos * 3)) << 16 | (uint8_t)(pos * 3);
    } else {
        pos -= 170;
        return (uint8_t)(pos * 3) << 8 | (uint8_t)(255 - pos * 3);
    }
}


/// RGB
//| .. data:: RGB
//|
//|    * **order** Red, Green, Blue
//|    * **bpp** 3
PIXELBUF_BYTEORDER(RGB, 3, 0, 1, 2, 3, false, false)
//| .. data:: RBG
//|
//|    * **order** Red, Blue, Green
//|    * **bpp** 3
PIXELBUF_BYTEORDER(RBG, 3, 0, 2, 1, 3, false, false)
//| .. data:: GRB
//|
//|    * **order** Green, Red, Blue
//|    * **bpp** 3
//|
//|    Commonly used by NeoPixel.
PIXELBUF_BYTEORDER(GRB, 3, 1, 0, 2, 3, false, false)
//| .. data:: GBR
//|
//|    * **order** Green, Blue, Red
//|    * **bpp** 3
PIXELBUF_BYTEORDER(GBR, 3, 1, 2, 0, 3, false, false)
//| .. data:: BRG
//|
//|    * **order** Blue, Red, Green
//|    * **bpp** 3
PIXELBUF_BYTEORDER(BRG, 3, 2, 0, 1, 3, false, false)
//| .. data:: BGR
//|
//|    * **order** Blue, Green, Red
//|    * **bpp** 3
//|
//|    Commonly used by Dotstar.
PIXELBUF_BYTEORDER(BGR, 3, 2, 1, 0, 3, false, false)

// RGBW
//| .. data:: RGBW
//|
//|    * **order** Red, Green, Blue, White
//|    * **bpp** 4
//|    * **has_white** True
PIXELBUF_BYTEORDER(RGBW, 4, 0, 1, 2, 3, true, false)
//| .. data:: RBGW
//|
//|    * **order** Red, Blue, Green, White
//|    * **bpp** 4
//|    * **has_white** True
PIXELBUF_BYTEORDER(RBGW, 4, 0, 2, 1, 3, true, false)
//| .. data:: GRBW
//|
//|    * **order** Green, Red, Blue, White
//|    * **bpp** 4
//|    * **has_white** True
//|
//|    Commonly used by RGBW NeoPixels.
PIXELBUF_BYTEORDER(GRBW, 4, 1, 0, 2, 3, true, false)
//| .. data:: GBRW
//|
//|    * **order** Green, Blue, Red, White
//|    * **bpp** 4
//|    * **has_white** True
PIXELBUF_BYTEORDER(GBRW, 4, 1, 2, 0, 3, true, false)
//| .. data:: BRGW
//|
//|    * **order** Blue, Red, Green, White
//|    * **bpp** 4
//|    * **has_white** True
PIXELBUF_BYTEORDER(BRGW, 4, 2, 0, 1, 3, true, false)
//| .. data:: BGRW
//|
//|    * **order** Blue, Green, Red, White
//|    * **bpp** 4
//|    * **has_white** True
PIXELBUF_BYTEORDER(BGRW, 4, 2, 1, 0, 3, true, false)

// Luminosity + RGB (eg for Dotstar)
// Luminosity chosen because the luminosity of a Dotstar at full bright
// burns the eyes like looking at the Sun.
// https://www.thesaurus.com/browse/luminosity?s=t
//| .. data:: LRGB
//|
//|    * **order** *Luminosity*, Red, Green, Blue
//|    * **bpp** 4
//|    * **has_luminosity** True
PIXELBUF_BYTEORDER(LRGB, 4, 1, 2, 3, 0, false, true)
//| .. data:: LRBG
//|
//|    * **order** *Luminosity*, Red, Blue, Green
//|    * **bpp** 4
//|    * **has_luminosity** True
PIXELBUF_BYTEORDER(LRBG, 4, 1, 3, 2, 0, false, true)
//| .. data:: LGRB
//|
//|    * **order** *Luminosity*, Green, Red, Blue
//|    * **bpp** 4
//|    * **has_luminosity** True
PIXELBUF_BYTEORDER(LGRB, 4, 2, 1, 3, 0, false, true)
//| .. data:: LGBR
//|
//|    * **order** *Luminosity*, Green, Blue, Red
//|    * **bpp** 4
//|    * **has_luminosity** True
PIXELBUF_BYTEORDER(LGBR, 4, 2, 3, 1, 0, false, true)
//| .. data:: LBRG
//|
//|    * **order** *Luminosity*, Blue, Red, Green
//|    * **bpp** 4
//|    * **has_luminosity** True
PIXELBUF_BYTEORDER(LBRG, 4, 3, 1, 2, 0, false, true)
//| .. data:: LBGR
//|
//|    * **order** *Luminosity*, Blue, Green, Red
//|    * **bpp** 4
//|    * **has_luminosity** True
//|
//|    Actual format commonly used by DotStar (5 bit luninance value)
PIXELBUF_BYTEORDER(LBGR, 4, 3, 2, 1, 0, false, true)

STATIC const mp_rom_map_elem_t pixelbuf_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__pixelbuf) },
    { MP_ROM_QSTR(MP_QSTR_PixelBuf), MP_ROM_PTR(&pixelbuf_pixelbuf_type) },
    { MP_ROM_QSTR(MP_QSTR_RGB),   MP_ROM_QSTR(MP_QSTR_RGB) },
    { MP_ROM_QSTR(MP_QSTR_RBG),   MP_ROM_QSTR(MP_QSTR_RBG) },
    { MP_ROM_QSTR(MP_QSTR_GRB),   MP_ROM_QSTR(MP_QSTR_GRB) },
    { MP_ROM_QSTR(MP_QSTR_GBR),   MP_ROM_QSTR(MP_QSTR_GBR) },
    { MP_ROM_QSTR(MP_QSTR_BRG),   MP_ROM_QSTR(MP_QSTR_BRG) },
    { MP_ROM_QSTR(MP_QSTR_BGR),   MP_ROM_QSTR(MP_QSTR_BGR) },
    { MP_ROM_QSTR(MP_QSTR_RGBW),  MP_ROM_QSTR(MP_QSTR_RGBW) },
    { MP_ROM_QSTR(MP_QSTR_RBGW),  MP_ROM_QSTR(MP_QSTR_RBGW) },
    { MP_ROM_QSTR(MP_QSTR_GRBW),  MP_ROM_QSTR(MP_QSTR_GRBW) },
    { MP_ROM_QSTR(MP_QSTR_GBRW),  MP_ROM_QSTR(MP_QSTR_GBRW) },
    { MP_ROM_QSTR(MP_QSTR_BRGW),  MP_ROM_QSTR(MP_QSTR_BRGW) },
    { MP_ROM_QSTR(MP_QSTR_BGRW),  MP_ROM_QSTR(MP_QSTR_BGRW) },
    { MP_ROM_QSTR(MP_QSTR_RGBD),  MP_ROM_QSTR(MP_QSTR_RGBD) },
    { MP_ROM_QSTR(MP_QSTR_RBGD),  MP_ROM_QSTR(MP_QSTR_RBGD) },
    { MP_ROM_QSTR(MP_QSTR_GRBD),  MP_ROM_QSTR(MP_QSTR_GRBD) },
    { MP_ROM_QSTR(MP_QSTR_GBRD),  MP_ROM_QSTR(MP_QSTR_GBRD) },
    { MP_ROM_QSTR(MP_QSTR_BRGD),  MP_ROM_QSTR(MP_QSTR_BRGD) },
    { MP_ROM_QSTR(MP_QSTR_BGRD),  MP_ROM_QSTR(MP_QSTR_BGRD) },
    { MP_ROM_QSTR(MP_QSTR_wheel), MP_ROM_QSTR(&pixelbuf_wheel_obj) },
};

STATIC MP_DEFINE_CONST_DICT(pixelbuf_module_globals, pixelbuf_module_globals_table);

const mp_obj_module_t pixelbuf_module = {
        .base = { &mp_type_module },
        .globals = (mp_obj_dict_t*)&pixelbuf_module_globals,
};
