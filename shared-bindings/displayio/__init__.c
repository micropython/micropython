/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#include <stdint.h>

#include "py/enum.h"
#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/displayio/__init__.h"
#include "shared-bindings/displayio/Bitmap.h"
#include "shared-bindings/displayio/ColorConverter.h"
#include "shared-bindings/displayio/Display.h"
#include "shared-bindings/displayio/EPaperDisplay.h"
#include "shared-bindings/displayio/FourWire.h"
#include "shared-bindings/displayio/Group.h"
#include "shared-bindings/displayio/I2CDisplay.h"
#include "shared-bindings/displayio/OnDiskBitmap.h"
#include "shared-bindings/displayio/Palette.h"
#include "shared-bindings/displayio/ParallelBus.h"
#include "shared-bindings/displayio/Shape.h"
#include "shared-bindings/displayio/TileGrid.h"

//| """Native helpers for driving displays
//|
//| The `displayio` module contains classes to manage display output
//| including synchronizing with refresh rates and partial updating."""
//|

//| def release_displays() -> None:
//|     """Releases any actively used displays so their busses and pins can be used again. This will also
//|     release the builtin display on boards that have one. You will need to reinitialize it yourself
//|     afterwards. This may take seconds to complete if an active EPaperDisplay is refreshing.
//|
//|     Use this once in your code.py if you initialize a display. Place it right before the
//|     initialization so the display is active as long as possible."""
//|     ...
//|
STATIC mp_obj_t displayio_release_displays(void) {
    common_hal_displayio_release_displays();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(displayio_release_displays_obj, displayio_release_displays);

MAKE_ENUM_VALUE(displayio_colorspace_type, displayio_colorspace, RGB888, DISPLAYIO_COLORSPACE_RGB888);
MAKE_ENUM_VALUE(displayio_colorspace_type, displayio_colorspace, RGB565, DISPLAYIO_COLORSPACE_RGB565);
MAKE_ENUM_VALUE(displayio_colorspace_type, displayio_colorspace, RGB565_SWAPPED, DISPLAYIO_COLORSPACE_RGB565_SWAPPED);
MAKE_ENUM_VALUE(displayio_colorspace_type, displayio_colorspace, RGB555, DISPLAYIO_COLORSPACE_RGB555);
MAKE_ENUM_VALUE(displayio_colorspace_type, displayio_colorspace, RGB555_SWAPPED, DISPLAYIO_COLORSPACE_RGB555_SWAPPED);
MAKE_ENUM_VALUE(displayio_colorspace_type, displayio_colorspace, BGR565, DISPLAYIO_COLORSPACE_BGR565);
MAKE_ENUM_VALUE(displayio_colorspace_type, displayio_colorspace, BGR565_SWAPPED, DISPLAYIO_COLORSPACE_BGR565_SWAPPED);
MAKE_ENUM_VALUE(displayio_colorspace_type, displayio_colorspace, BGR555, DISPLAYIO_COLORSPACE_BGR555);
MAKE_ENUM_VALUE(displayio_colorspace_type, displayio_colorspace, BGR555_SWAPPED, DISPLAYIO_COLORSPACE_BGR555_SWAPPED);

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
};
STATIC MP_DEFINE_CONST_DICT(displayio_colorspace_locals_dict, displayio_colorspace_locals_table);

MAKE_PRINTER(displayio, displayio_colorspace);
MAKE_ENUM_TYPE(displayio, ColorSpace, displayio_colorspace);


STATIC const mp_rom_map_elem_t displayio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_displayio) },
    { MP_ROM_QSTR(MP_QSTR_Bitmap), MP_ROM_PTR(&displayio_bitmap_type) },
    { MP_ROM_QSTR(MP_QSTR_ColorConverter), MP_ROM_PTR(&displayio_colorconverter_type) },
    { MP_ROM_QSTR(MP_QSTR_Colorspace), MP_ROM_PTR(&displayio_colorspace_type) },
    { MP_ROM_QSTR(MP_QSTR_Display), MP_ROM_PTR(&displayio_display_type) },
    { MP_ROM_QSTR(MP_QSTR_EPaperDisplay), MP_ROM_PTR(&displayio_epaperdisplay_type) },
    { MP_ROM_QSTR(MP_QSTR_Group), MP_ROM_PTR(&displayio_group_type) },
    { MP_ROM_QSTR(MP_QSTR_OnDiskBitmap), MP_ROM_PTR(&displayio_ondiskbitmap_type) },
    { MP_ROM_QSTR(MP_QSTR_Palette), MP_ROM_PTR(&displayio_palette_type) },
    { MP_ROM_QSTR(MP_QSTR_Shape), MP_ROM_PTR(&displayio_shape_type) },
    { MP_ROM_QSTR(MP_QSTR_TileGrid), MP_ROM_PTR(&displayio_tilegrid_type) },

    { MP_ROM_QSTR(MP_QSTR_FourWire), MP_ROM_PTR(&displayio_fourwire_type) },
    { MP_ROM_QSTR(MP_QSTR_I2CDisplay), MP_ROM_PTR(&displayio_i2cdisplay_type) },
    { MP_ROM_QSTR(MP_QSTR_ParallelBus), MP_ROM_PTR(&displayio_parallelbus_type) },

    { MP_ROM_QSTR(MP_QSTR_release_displays), MP_ROM_PTR(&displayio_release_displays_obj) },
};

STATIC MP_DEFINE_CONST_DICT(displayio_module_globals, displayio_module_globals_table);

const mp_obj_module_t displayio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&displayio_module_globals,
};
