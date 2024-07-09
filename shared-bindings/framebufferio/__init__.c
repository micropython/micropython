// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "shared-bindings/framebufferio/__init__.h"
#include "shared-bindings/framebufferio/FramebufferDisplay.h"

//| """Native framebuffer display driving
//|
//| The `framebufferio` module contains classes to manage display output
//| including synchronizing with refresh rates and partial updating.
//| It is used in conjunction with classes from `displayio` to actually
//| place items on the display; and classes like `RGBMatrix` to actually
//| drive the display."""

#if CIRCUITPY_FRAMEBUFFERIO
static const mp_rom_map_elem_t framebufferio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_framebufferio) },
    { MP_ROM_QSTR(MP_QSTR_FramebufferDisplay), MP_ROM_PTR(&framebufferio_framebufferdisplay_type) },
};

static MP_DEFINE_CONST_DICT(framebufferio_module_globals, framebufferio_module_globals_table);

const mp_obj_module_t framebufferio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&framebufferio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_framebufferio, framebufferio_module);
#endif
