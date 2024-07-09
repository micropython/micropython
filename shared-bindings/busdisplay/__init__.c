// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/enum.h"
#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/busdisplay/__init__.h"
#include "shared-bindings/busdisplay/BusDisplay.h"

//| """Displays a `displayio` object tree on an external device with a built-in
//| framebuffer
//|
//| """

static const mp_rom_map_elem_t busdisplay_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_displayio) },

    { MP_ROM_QSTR(MP_QSTR_BusDisplay), MP_ROM_PTR(&busdisplay_busdisplay_type) },
};
static MP_DEFINE_CONST_DICT(busdisplay_module_globals, busdisplay_module_globals_table);

const mp_obj_module_t busdisplay_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&busdisplay_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_busdisplay, busdisplay_module);
