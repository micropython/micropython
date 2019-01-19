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
//| The `_pixelbuf` module provides :py:class:`PixelBuf` and :py:class:`ByteOrder` classes to accelerate
//| RGB(W) strip/matrix manipulation, such as DotStar and Neopixel.
//|

//| Libraries
//|
//| .. toctree::
//|     :maxdepth: 3
//|
//|     PixelBuf

//| .. class:: ByteOrder
//|
//|   Classes representing byteorders for circuitpython


//|   .. attribute:: bpp
//|
//|     The number of bytes per pixel (read-only)
//|

//|   .. attribute:: has_white
//|
//|     Whether the pixel has white (in addition to RGB)
//|

//|   .. attribute:: has_luminosity
//|
//|     Whether the pixel has luminosity (in addition to RGB)
//|

//|   .. attribute:: byteorder
//|
//|     Tuple of byte order (r, g, b) or (r, g, b, w) or (r, g, b, l)
//|


STATIC void pixelbuf_byteorder_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    mp_check_self(MP_OBJ_IS_TYPE(self_in, &pixelbuf_byteorder_type));
    pixelbuf_byteorder_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        mp_obj_t val;
        if (attr == MP_QSTR_bpp) {
            val = MP_OBJ_NEW_SMALL_INT(self->bpp);
        } else if (attr == MP_QSTR_has_white) {
            val = mp_obj_new_bool(self->has_white);
        } else if (attr == MP_QSTR_has_luminosity) {
            val = mp_obj_new_bool(self->has_luminosity);
        } else if (attr == MP_QSTR_byteorder) {
            mp_obj_t items[4];
            uint8_t n = self->bpp;
            if (self->has_luminosity || self->has_white) {
                n = 4;
            }
            uint8_t *values = (uint8_t *)&(self->byteorder);
            for (uint8_t i=0; i<n; i++) {
                items[i] = MP_OBJ_NEW_SMALL_INT(values[i]);
            }
            val = mp_obj_new_tuple(n, items);
        } else {
            mp_raise_AttributeError(translate("no such attribute"));
        }
        dest[0] = val;
    } else {
        // delete/store attribute (ignored)
        dest[0] = MP_OBJ_NULL;
        mp_raise_AttributeError(translate("readonly attribute"));
    }
}

STATIC mp_obj_t pixelbuf_byteorder_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    pixelbuf_byteorder_obj_t *self = MP_OBJ_TO_PTR(self_in);
    switch (op) {
        case MP_UNARY_OP_LEN: return MP_OBJ_NEW_SMALL_INT(self->bpp);
        default: return MP_OBJ_NULL; // op not supported
    }
}

const mp_obj_type_t pixelbuf_byteorder_type = {
    { &mp_type_type },
    .name = MP_QSTR_ByteOrder,
    .print = pixelbuf_byteorder_print,
    .unary_op = pixelbuf_byteorder_unary_op,
    .attr = pixelbuf_byteorder_attr,
};


// This macro is used to simplify RGB subclass definition
#define PIXELBUF_BYTEORDER(p_name, p_bpp, p_r, p_g, p_b, p_w, p_has_white, p_has_luminosity) \
const pixelbuf_byteorder_obj_t byteorder_## p_name = { \
    { &pixelbuf_byteorder_type }, \
    .name = MP_QSTR_## p_name, \
    .bpp = p_bpp, \
    .byteorder = { p_r, p_g, p_b, p_w }, \
    .has_white = p_has_white, \
    .has_luminosity = p_has_luminosity, \
};

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
//| .. class:: RGB
//|
//|    * **order** Red, Green, Blue
//|    * **bpp** 3
PIXELBUF_BYTEORDER(RGB, 3, 0, 1, 2, 3, false, false)
//| .. class:: RBG
//|
//|    * **order** Red, Blue, Green
//|    * **bpp** 3
PIXELBUF_BYTEORDER(RBG, 3, 0, 2, 1, 3, false, false)
//| .. class:: GRB
//|
//|    * **order** Green, Red, Blue
//|    * **bpp** 3
//|
//|    Commonly used by NeoPixel.
PIXELBUF_BYTEORDER(GRB, 3, 1, 0, 2, 3, false, false)
//| .. class:: GBR
//|
//|    * **order** Green, Blue, Red
//|    * **bpp** 3
PIXELBUF_BYTEORDER(GBR, 3, 1, 2, 0, 3, false, false)
//| .. class:: BRG
//|
//|    * **order** Blue, Red, Green
//|    * **bpp** 3
PIXELBUF_BYTEORDER(BRG, 3, 2, 0, 1, 3, false, false)
//| .. class:: BGR
//|
//|    * **order** Blue, Green, Red
//|    * **bpp** 3
//|
//|    Commonly used by Dotstar.
PIXELBUF_BYTEORDER(BGR, 3, 2, 1, 0, 3, false, false)

// RGBW
//| .. class:: RGBW
//|
//|    * **order** Red, Green, Blue, White
//|    * **bpp** 4
//|    * **has_white** True
PIXELBUF_BYTEORDER(RGBW, 4, 0, 1, 2, 3, true, false)
//| .. class:: RBGW
//|
//|    * **order** Red, Blue, Green, White
//|    * **bpp** 4
//|    * **has_white** True
PIXELBUF_BYTEORDER(RBGW, 4, 0, 2, 1, 3, true, false)
//| .. class:: GRBW
//|
//|    * **order** Green, Red, Blue, White
//|    * **bpp** 4
//|    * **has_white** True
//|
//|    Commonly used by RGBW NeoPixels.
PIXELBUF_BYTEORDER(GRBW, 4, 1, 0, 2, 3, true, false)
//| .. class:: GBRW
//|
//|    * **order** Green, Blue, Red, White
//|    * **bpp** 4
//|    * **has_white** True
PIXELBUF_BYTEORDER(GBRW, 4, 1, 2, 0, 3, true, false)
//| .. class:: BRGW
//|
//|    * **order** Blue, Red, Green, White
//|    * **bpp** 4
//|    * **has_white** True
PIXELBUF_BYTEORDER(BRGW, 4, 2, 0, 1, 3, true, false)
//| .. class:: BGRW
//|
//|    * **order** Blue, Green, Red, White
//|    * **bpp** 4
//|    * **has_white** True
PIXELBUF_BYTEORDER(BGRW, 4, 2, 1, 0, 3, true, false)

// Luminosity + RGB (eg for Dotstar)
// Luminosity chosen because the luminosity of a Dotstar at full bright
// burns the eyes like looking at the Sun.
// https://www.thesaurus.com/browse/luminosity?s=t
//| .. class:: LRGB
//|
//|    * **order** *Luminosity*, Red, Green, Blue
//|    * **bpp** 4
//|    * **has_luminosity** True
PIXELBUF_BYTEORDER(LRGB, 4, 1, 2, 3, 0, false, true)
//| .. class:: LRBG
//|
//|    * **order** *Luminosity*, Red, Blue, Green
//|    * **bpp** 4
//|    * **has_luminosity** True
PIXELBUF_BYTEORDER(LRBG, 4, 1, 3, 2, 0, false, true)
//| .. class:: LGRB
//|
//|    * **order** *Luminosity*, Green, Red, Blue
//|    * **bpp** 4
//|    * **has_luminosity** True
PIXELBUF_BYTEORDER(LGRB, 4, 2, 1, 3, 0, false, true)
//| .. class:: LGBR
//|
//|    * **order** *Luminosity*, Green, Blue, Red
//|    * **bpp** 4
//|    * **has_luminosity** True
PIXELBUF_BYTEORDER(LGBR, 4, 2, 3, 1, 0, false, true)
//| .. class:: LBRG
//|
//|    * **order** *Luminosity*, Blue, Red, Green
//|    * **bpp** 4
//|    * **has_luminosity** True
PIXELBUF_BYTEORDER(LBRG, 4, 3, 1, 2, 0, false, true)
//| .. class:: LBGR
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
    { MP_ROM_QSTR(MP_QSTR_ByteOrder), MP_ROM_PTR(&pixelbuf_byteorder_type) },
    { MP_ROM_QSTR(MP_QSTR_RGB),  MP_ROM_PTR(&byteorder_RGB) },
    { MP_ROM_QSTR(MP_QSTR_RBG),  MP_ROM_PTR(&byteorder_RBG) },
    { MP_ROM_QSTR(MP_QSTR_GRB),  MP_ROM_PTR(&byteorder_GRB) },
    { MP_ROM_QSTR(MP_QSTR_GBR),  MP_ROM_PTR(&byteorder_GBR) },
    { MP_ROM_QSTR(MP_QSTR_BRG),  MP_ROM_PTR(&byteorder_BRG) },
    { MP_ROM_QSTR(MP_QSTR_BGR),  MP_ROM_PTR(&byteorder_BGR) },
    { MP_ROM_QSTR(MP_QSTR_RGBW), MP_ROM_PTR(&byteorder_RGBW) },
    { MP_ROM_QSTR(MP_QSTR_RBGW), MP_ROM_PTR(&byteorder_RBGW) },
    { MP_ROM_QSTR(MP_QSTR_GRBW), MP_ROM_PTR(&byteorder_GRBW) },
    { MP_ROM_QSTR(MP_QSTR_GBRW), MP_ROM_PTR(&byteorder_GBRW) },
    { MP_ROM_QSTR(MP_QSTR_BRGW), MP_ROM_PTR(&byteorder_BRGW) },
    { MP_ROM_QSTR(MP_QSTR_BGRW), MP_ROM_PTR(&byteorder_BGRW) },
    { MP_ROM_QSTR(MP_QSTR_LRGB), MP_ROM_PTR(&byteorder_LRGB) },
    { MP_ROM_QSTR(MP_QSTR_LRBG), MP_ROM_PTR(&byteorder_LRBG) },
    { MP_ROM_QSTR(MP_QSTR_LGRB), MP_ROM_PTR(&byteorder_LGRB) },
    { MP_ROM_QSTR(MP_QSTR_LGBR), MP_ROM_PTR(&byteorder_LGBR) },
    { MP_ROM_QSTR(MP_QSTR_LBRG), MP_ROM_PTR(&byteorder_LBRG) },
    { MP_ROM_QSTR(MP_QSTR_LBGR), MP_ROM_PTR(&byteorder_LBGR) },
    { MP_ROM_QSTR(MP_QSTR_wheel), MP_ROM_PTR(&pixelbuf_wheel_obj) },
};

STATIC MP_DEFINE_CONST_DICT(pixelbuf_module_globals, pixelbuf_module_globals_table);

STATIC void pixelbuf_byteorder_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pixelbuf_byteorder_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "%q.%q", MP_QSTR__pixelbuf, self->name);
    return;
}

const mp_obj_module_t pixelbuf_module = {
        .base = { &mp_type_module },
        .globals = (mp_obj_dict_t*)&pixelbuf_module_globals,
};
