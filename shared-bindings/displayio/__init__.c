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
//|

//| AnyDisplayBus = fourwire.FourWire | i2cdisplaybus.I2CDisplayBus | is31fl3741.IS31FL3741
//| """Type-checking shorthand for any kind of display bus. Not actually defined in CircuitPython."""
//|
//| AnyFramebuffer = (
//|     rgbmatrix.RGBMatrix
//|     | is31fl3741.IS31FL3741_FrameBuffer
//|     | sharpdisplay.SharpMemoryFramebuffer
//|     | picodvi.Framebuffer
//|     | aurora_epaper.AuroraMemoryFramebuffer
//| )
//| """Type-checking shorthand for any kind of framebuffer. Not actually defined in CircuitPython."""
//|
//| AnyDisplay = (
//|     busdisplay.BusDisplay | epaperdisplay.EPaperDisplay | framebufferio.FramebufferDisplay
//| )
//| """Type-checking shorthand for any kind of display. Not actually defined in CircuitPython."""
//| CIRCUITPYTHON_TERMINAL: Group
//| """The `displayio.Group` that is the displayed serial terminal (REPL)."""
//|

//| from busdisplay import BusDisplay as Display
//| from epaperdisplay import EPaperDisplay
//| from fourwire import FourWire
//| from i2cdisplaybus import I2CDisplayBus as I2CDisplay
//|
//|

//| def release_displays() -> None:
//|     """Releases any actively used displays so their buses and pins can be used again. This will also
//|     release the builtin display on boards that have one. You will need to reinitialize it yourself
//|     afterwards. This may take seconds to complete if an active EPaperDisplay is refreshing.
//|
//|     Displays that are not `supervisor.runtime.display <supervisor.Runtime.display>` are released automatically when your code
//|     finishes. Use this before initializing a display if one may still be active, such as a builtin
//|     display or a display from an earlier run.
//|
//|     Once you release a builtin display (one that is initialized automatically, not by user code),
//|     the display will remain released even if you restart the CircuitPython VM, for instance by typing ctrl-D.
//|     The display is not re-initialized until you do a hard reset or manually initialize it again.
//|     """
//|     ...
//|
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

    { MP_ROM_QSTR(MP_QSTR_release_displays), MP_ROM_PTR(&displayio_release_displays_obj) },

    { MP_ROM_QSTR(MP_QSTR_CIRCUITPYTHON_TERMINAL), MP_ROM_PTR(&circuitpython_splash) },
};
static MP_DEFINE_CONST_DICT(displayio_module_globals, displayio_module_globals_table);

const mp_obj_module_t displayio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&displayio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_displayio, displayio_module);
