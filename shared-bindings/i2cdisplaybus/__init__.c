// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/enum.h"
#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/i2cdisplaybus/__init__.h"
#include "shared-bindings/i2cdisplaybus/I2CDisplayBus.h"

//| """Communicates to a display IC over I2C
//|
//| """

static const mp_rom_map_elem_t i2cdisplaybus_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_i2cdisplaybus) },

    { MP_ROM_QSTR(MP_QSTR_I2CDisplayBus), MP_ROM_PTR(&i2cdisplaybus_i2cdisplaybus_type) },
};
static MP_DEFINE_CONST_DICT(i2cdisplaybus_module_globals, i2cdisplaybus_module_globals_table);

const mp_obj_module_t i2cdisplaybus_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&i2cdisplaybus_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_i2cdisplaybus, i2cdisplaybus_module);
