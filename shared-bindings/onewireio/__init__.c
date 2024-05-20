// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/onewireio/__init__.h"
#include "shared-bindings/onewireio/OneWire.h"

#include "py/runtime.h"

//| """Low-level bit primitives for Maxim (formerly Dallas Semi) one-wire protocol.
//|
//|    Protocol definition is here: https://www.analog.com/en/technical-articles/1wire-communication-through-software.html"""

static const mp_rom_map_elem_t onewireio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_onewireio) },
    { MP_ROM_QSTR(MP_QSTR_OneWire),   MP_ROM_PTR(&onewireio_onewire_type) },
};

static MP_DEFINE_CONST_DICT(onewireio_module_globals, onewireio_module_globals_table);

const mp_obj_module_t onewireio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&onewireio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_onewireio, onewireio_module);
