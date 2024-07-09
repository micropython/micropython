// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/enum.h"
#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/fourwire/__init__.h"
#include "shared-bindings/fourwire/FourWire.h"

//| """Connects to a BusDisplay over a four wire bus
//|
//| """

static const mp_rom_map_elem_t fourwire_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_fourwire) },

    { MP_ROM_QSTR(MP_QSTR_FourWire), MP_ROM_PTR(&fourwire_fourwire_type) },
};
static MP_DEFINE_CONST_DICT(fourwire_module_globals, fourwire_module_globals_table);

const mp_obj_module_t fourwire_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&fourwire_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_fourwire, fourwire_module);
