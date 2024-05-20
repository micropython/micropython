// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Rose Hooper
//
// SPDX-License-Identifier: MIT

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

static const mp_rom_map_elem_t pixelmap_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__pixelmap) },
    { MP_ROM_QSTR(MP_QSTR_PixelMap), MP_ROM_PTR(&pixelmap_pixelmap_type) },
};

static MP_DEFINE_CONST_DICT(pixelmap_module_globals, pixelmap_module_globals_table);

const mp_obj_module_t pixelmap_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&pixelmap_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR__pixelmap, pixelmap_module);
