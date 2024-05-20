// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/enum.h"
#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/paralleldisplaybus/__init__.h"
#include "shared-bindings/paralleldisplaybus/ParallelBus.h"

//| """Native helpers for driving parallel displays"""


static const mp_rom_map_elem_t paralleldisplaybus_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_paralleldisplaybus) },
    { MP_ROM_QSTR(MP_QSTR_ParallelBus), MP_ROM_PTR(&paralleldisplaybus_parallelbus_type) },
};

static MP_DEFINE_CONST_DICT(paralleldisplaybus_module_globals, paralleldisplaybus_module_globals_table);

const mp_obj_module_t paralleldisplaybus_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&paralleldisplaybus_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_paralleldisplaybus, paralleldisplaybus_module);

// Remove in CircuitPython 10
MP_REGISTER_MODULE(MP_QSTR_paralleldisplay, paralleldisplaybus_module);
