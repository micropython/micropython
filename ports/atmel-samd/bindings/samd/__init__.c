// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Noralf Trønnes
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/runtime.h"

#include "bindings/samd/Clock.h"

//| """SAMD implementation settings"""

//| """:mod:`samd.clock` --- samd clock names
//| --------------------------------------------------------
//|
//| .. module:: samd.clock
//|   :synopsis: samd clock names
//|   :platform: SAMD21
//|
//| References to clocks as named by the microcontroller"""
const mp_obj_module_t samd_clock_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&samd_clock_globals,
};

static const mp_rom_map_elem_t samd_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_samd) },
    { MP_ROM_QSTR(MP_QSTR_clock),  MP_ROM_PTR(&samd_clock_module) },
};

static MP_DEFINE_CONST_DICT(samd_module_globals, samd_module_globals_table);

const mp_obj_module_t samd_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&samd_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_samd, samd_module);
