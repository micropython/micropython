// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Rose Hooper
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "py/objproperty.h"

#include "shared-bindings/adafruit_pixelbuf/__init__.h"
#include "shared-bindings/adafruit_pixelbuf/PixelBuf.h"


//| """A fast RGB(W) pixel buffer library for like NeoPixel and DotStar
//|
//| The `adafruit_pixelbuf` module provides the :py:class:`PixelBuf` class to accelerate
//| RGB(W) strip/matrix manipulation, such as DotStar and Neopixel.
//|
//| Byteorders are configured with strings, such as "RGB" or "RGBD"."""
//|
//| # The types accepted when getting a pixel value
//| PixelReturnType = Union[
//|     Tuple[int, int, int], Tuple[int, int, int, int], Tuple[int, int, int, float]
//| ]
//| PixelReturnSequence = Tuple[PixelReturnType]
//| # The types returned when getting a pixel value
//| PixelType = Union[int, PixelReturnType]
//| PixelSequence = Union[Tuple[PixelType], List[PixelType]]
// TODO: Pull in docs from adafruit_pixelbuf.

static const mp_rom_map_elem_t pixelbuf_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_adafruit_pixelbuf) },
    { MP_ROM_QSTR(MP_QSTR_PixelBuf), MP_ROM_PTR(&pixelbuf_pixelbuf_type) },
};

static MP_DEFINE_CONST_DICT(pixelbuf_module_globals, pixelbuf_module_globals_table);

const mp_obj_module_t pixelbuf_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&pixelbuf_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_adafruit_pixelbuf, pixelbuf_module);
