/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Jeff Epler for Adafruit Industries
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

#include "py/enum.h"
#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/displayio/__init__.h"
#include "shared-bindings/displayio/Bitmap.h"

MAKE_ENUM_VALUE(displayio_colorspace_type, displayio_colorspace, RGB888, DISPLAYIO_COLORSPACE_RGB888);
MAKE_ENUM_VALUE(displayio_colorspace_type, displayio_colorspace, RGB565, DISPLAYIO_COLORSPACE_RGB565);
MAKE_ENUM_VALUE(displayio_colorspace_type, displayio_colorspace, RGB565_SWAPPED, DISPLAYIO_COLORSPACE_RGB565_SWAPPED);
MAKE_ENUM_VALUE(displayio_colorspace_type, displayio_colorspace, RGB555, DISPLAYIO_COLORSPACE_RGB555);
MAKE_ENUM_VALUE(displayio_colorspace_type, displayio_colorspace, RGB555_SWAPPED, DISPLAYIO_COLORSPACE_RGB555_SWAPPED);
MAKE_ENUM_VALUE(displayio_colorspace_type, displayio_colorspace, BGR565, DISPLAYIO_COLORSPACE_BGR565);
MAKE_ENUM_VALUE(displayio_colorspace_type, displayio_colorspace, BGR565_SWAPPED, DISPLAYIO_COLORSPACE_BGR565_SWAPPED);
MAKE_ENUM_VALUE(displayio_colorspace_type, displayio_colorspace, BGR555, DISPLAYIO_COLORSPACE_BGR555);
MAKE_ENUM_VALUE(displayio_colorspace_type, displayio_colorspace, BGR555_SWAPPED, DISPLAYIO_COLORSPACE_BGR555_SWAPPED);
MAKE_ENUM_VALUE(displayio_colorspace_type, displayio_colorspace, L8, DISPLAYIO_COLORSPACE_L8);

//| class Colorspace:
//|     """The colorspace for a `ColorConverter` to operate in"""
//|
//|     RGB888: Colorspace
//|     """The standard 24-bit colorspace.  Bits 0-7 are blue, 8-15 are green, and 16-24 are red. (0xRRGGBB)"""
//|
//|     RGB565: Colorspace
//|     """The standard 16-bit colorspace.  Bits 0-4 are blue, bits 5-10 are green, and 11-15 are red (0bRRRRRGGGGGGBBBBB)"""
//|
//|     RGB565_SWAPPED: Colorspace
//|     """The swapped 16-bit colorspace.  First, the high and low 8 bits of the number are swapped, then they are interpreted as for RGB565"""
//|
//|     RGB555: Colorspace
//|     """The standard 15-bit colorspace.  Bits 0-4 are blue, bits 5-9 are green, and 11-14 are red.  The top bit is ignored. (0bxRRRRRGGGGGBBBBB)"""
//|
//|     RGB555_SWAPPED: Colorspace
//|     """The swapped 15-bit colorspace.  First, the high and low 8 bits of the number are swapped, then they are interpreted as for RGB555"""
//|
MAKE_ENUM_MAP(displayio_colorspace) {
    MAKE_ENUM_MAP_ENTRY(displayio_colorspace, RGB888),
    MAKE_ENUM_MAP_ENTRY(displayio_colorspace, RGB565),
    MAKE_ENUM_MAP_ENTRY(displayio_colorspace, RGB565_SWAPPED),
    MAKE_ENUM_MAP_ENTRY(displayio_colorspace, RGB555),
    MAKE_ENUM_MAP_ENTRY(displayio_colorspace, RGB555_SWAPPED),
    MAKE_ENUM_MAP_ENTRY(displayio_colorspace, BGR565),
    MAKE_ENUM_MAP_ENTRY(displayio_colorspace, BGR565_SWAPPED),
    MAKE_ENUM_MAP_ENTRY(displayio_colorspace, BGR555),
    MAKE_ENUM_MAP_ENTRY(displayio_colorspace, BGR555_SWAPPED),
    MAKE_ENUM_MAP_ENTRY(displayio_colorspace, L8),
};
STATIC MP_DEFINE_CONST_DICT(displayio_colorspace_locals_dict, displayio_colorspace_locals_table);

MAKE_PRINTER(displayio, displayio_colorspace);
MAKE_ENUM_TYPE(displayio, ColorSpace, displayio_colorspace);

STATIC const mp_rom_map_elem_t displayio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_displayio) },
    { MP_ROM_QSTR(MP_QSTR_Bitmap), MP_ROM_PTR(&displayio_bitmap_type) },
    { MP_ROM_QSTR(MP_QSTR_Colorspace), MP_ROM_PTR(&displayio_colorspace_type) },
};
STATIC MP_DEFINE_CONST_DICT(displayio_module_globals, displayio_module_globals_table);

const mp_obj_module_t displayio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&displayio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_displayio, displayio_module);
