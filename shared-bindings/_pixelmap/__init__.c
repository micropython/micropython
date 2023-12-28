/*
 * This file is part of the CircuitPython project, https://github.com/adafruit/circuitpython
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Rose Hooper
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

#include "shared-bindings/_pixelmap/__init__.h"
#include "shared-bindings/_pixelmap/PixelMap.h"


//| """A fast pixel mapping library
//|
//| The `_pixelmap` module provides the :py:class:`PixelMap` class to accelerate
//| RGB(W) strip/matrix manipulation, such as DotStar and Neopixel."""
//|
//| # The types accepted when getting a pixel value
//| PixelReturnType = Union[
//|     Tuple[int, int, int], Tuple[int, int, int, int], Tuple[int, int, int, float]
//| ]
//| PixelReturnSequence = Tuple[PixelReturnType]
//| # The types returned when getting a pixel value
//| PixelType = Union[int, PixelReturnType]
//| PixelSequence = Union[Tuple[PixelType], List[PixelType]]

STATIC const mp_rom_map_elem_t pixelmap_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__pixelmap) },
    { MP_ROM_QSTR(MP_QSTR_PixelMap), MP_ROM_PTR(&pixelmap_pixelmap_type) },
};

STATIC MP_DEFINE_CONST_DICT(pixelmap_module_globals, pixelmap_module_globals_table);

const mp_obj_module_t pixelmap_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&pixelmap_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR__pixelmap, pixelmap_module);
