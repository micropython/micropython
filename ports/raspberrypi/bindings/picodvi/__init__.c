// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "bindings/picodvi/Framebuffer.h"

//| """Low-level routines for interacting with PicoDVI Output"""

static const mp_rom_map_elem_t picodvi_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_picodvi) },
    { MP_ROM_QSTR(MP_QSTR_Framebuffer), MP_ROM_PTR(&picodvi_framebuffer_type) },
};

static MP_DEFINE_CONST_DICT(picodvi_module_globals, picodvi_module_globals_table);

const mp_obj_module_t picodvi_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&picodvi_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_picodvi, picodvi_module);
