// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 by Daniel Pollard
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/countio/__init__.h"
#include "shared-bindings/countio/Counter.h"
#include "shared-bindings/countio/Edge.h"

//| """Support for edge counting
//|
//| The `countio` module contains logic to read and count edge transitions
//|
//| For more information on the applications of counting edges, see
//| `this Learn Guide on sequential circuits
//| <https://learn.adafruit.com/digital-circuits-4-sequential-circuits>`_.
//|
//| All classes change hardware state and should be deinitialized when they
//| are no longer needed if the program continues after use. To do so, either
//| call :py:meth:`!deinit` or use a context manager. See
//| :ref:`lifetime-and-contextmanagers` for more info."""

static const mp_rom_map_elem_t countio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_countio) },
    { MP_ROM_QSTR(MP_QSTR_Counter),  MP_ROM_PTR(&countio_counter_type) },
    { MP_ROM_QSTR(MP_QSTR_Edge),  MP_ROM_PTR(&countio_edge_type) },
};

static MP_DEFINE_CONST_DICT(countio_module_globals, countio_module_globals_table);

const mp_obj_module_t countio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&countio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_countio, countio_module);
