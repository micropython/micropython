// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/enum.h"
#include "py/obj.h"
#include "py/runtime.h"

#if CIRCUITPY_BUSDISPLAY
#include "shared-bindings/busdisplay/BusDisplay.h"
#endif
#include "shared-bindings/displayio/__init__.h"
#include "shared-bindings/displayio/Bitmap.h"
#include "shared-bindings/displayio/ColorConverter.h"
#include "shared-bindings/displayio/Group.h"
#include "shared-bindings/displayio/OnDiskBitmap.h"
#include "shared-bindings/displayio/Palette.h"
#include "shared-bindings/displayio/TileGrid.h"
#if CIRCUITPY_EPAPERDISPLAY
#include "shared-bindings/epaperdisplay/EPaperDisplay.h"
#endif
#if CIRCUITPY_FOURWIRE
#include "shared-bindings/fourwire/FourWire.h"
#endif
#if CIRCUITPY_I2CDISPLAYBUS
#include "shared-bindings/i2cdisplaybus/I2CDisplayBus.h"
#endif
#include "shared-module/displayio/__init__.h"

//| """High level, display object compositing system
//|
//| The `displayio` module contains classes to define what objects to display.
//| It is optimized for low memory use and, therefore, computes final pixel
//| values for dirty regions as needed.
//|
//| Separate modules manage transmitting the display contents to a display.
//|
//| For more a more thorough explanation and guide for using `displayio`, please
//| refer to `this Learn guide
//| <https://learn.adafruit.com/circuitpython-display-support-using-displayio>`_.
//| """

//| CIRCUITPYTHON_TERMINAL: Group
//| """The `displayio.Group` that is the displayed serial terminal (REPL)."""
//|

//| from busdisplay import BusDisplay as Display
//| from epaperdisplay import EPaperDisplay
//| from fourwire import FourWire
//| from i2cdisplaybus import I2CDisplayBus as I2CDisplay
//|

//| def release_displays() -> None:
//|     """Releases any actively used displays so their buses and pins can be used again. This will also
//|     release the builtin display on boards that have one. You will need to reinitialize it yourself
//|     afterwards. This may take seconds to complete if an active EPaperDisplay is refreshing.
//|
//|     Use this once in your code.py if you initialize a display. Place it right before the
//|     initialization so the display is active as long as possible."""
//|     ...
//|
static mp_obj_t displayio_release_displays(void) {
    common_hal_displayio_release_displays();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(displayio_release_displays_obj, displayio_release_displays);


static const mp_rom_map_elem_t displayio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_displayio) },
    { MP_ROM_QSTR(MP_QSTR_Bitmap), MP_ROM_PTR(&displayio_bitmap_type) },
    { MP_ROM_QSTR(MP_QSTR_ColorConverter), MP_ROM_PTR(&displayio_colorconverter_type) },
    { MP_ROM_QSTR(MP_QSTR_Colorspace), MP_ROM_PTR(&displayio_colorspace_type) },
    { MP_ROM_QSTR(MP_QSTR_Group), MP_ROM_PTR(&displayio_group_type) },
    { MP_ROM_QSTR(MP_QSTR_OnDiskBitmap), MP_ROM_PTR(&displayio_ondiskbitmap_type) },
    { MP_ROM_QSTR(MP_QSTR_Palette), MP_ROM_PTR(&displayio_palette_type) },
    { MP_ROM_QSTR(MP_QSTR_TileGrid), MP_ROM_PTR(&displayio_tilegrid_type) },

    // Remove these in CircuitPython 10
    #if CIRCUITPY_BUSDISPLAY
    { MP_ROM_QSTR(MP_QSTR_Display), MP_ROM_PTR(&busdisplay_busdisplay_type) },
    #endif
    #if CIRCUITPY_EPAPERDISPLAY
    { MP_ROM_QSTR(MP_QSTR_EPaperDisplay), MP_ROM_PTR(&epaperdisplay_epaperdisplay_type) },
    #endif
    #if CIRCUITPY_FOURWIRE
    { MP_ROM_QSTR(MP_QSTR_FourWire), MP_ROM_PTR(&fourwire_fourwire_type) },
    #endif
    #if CIRCUITPY_I2CDISPLAYBUS
    { MP_ROM_QSTR(MP_QSTR_I2CDisplay), MP_ROM_PTR(&i2cdisplaybus_i2cdisplaybus_type) },
    #endif

    { MP_ROM_QSTR(MP_QSTR_release_displays), MP_ROM_PTR(&displayio_release_displays_obj) },
    { MP_ROM_QSTR(MP_QSTR_CIRCUITPYTHON_TERMINAL), MP_ROM_PTR(&circuitpython_splash) },
};
static MP_DEFINE_CONST_DICT(displayio_module_globals, displayio_module_globals_table);

const mp_obj_module_t displayio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&displayio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_displayio, displayio_module);
