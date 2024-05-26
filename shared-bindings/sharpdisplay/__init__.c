// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/sharpdisplay/SharpMemoryFramebuffer.h"

//| """Support for Sharp Memory Display framebuffers
//|
//| For more information about working with Sharp Memory Displays,
//| see `this Learn guide <https://learn.adafruit.com/adafruit-sharp-memory-display-breakout/circuitpython-displayio-setup>`_.
//| """
static const mp_rom_map_elem_t sharpdisplay_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_sharpdisplay) },
    { MP_ROM_QSTR(MP_QSTR_SharpMemoryFramebuffer), MP_ROM_PTR(&sharpdisplay_framebuffer_type) },
};

static MP_DEFINE_CONST_DICT(sharpdisplay_module_globals, sharpdisplay_module_globals_table);

const mp_obj_module_t sharpdisplay_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&sharpdisplay_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_sharpdisplay, sharpdisplay_module);
