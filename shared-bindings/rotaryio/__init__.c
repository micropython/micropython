// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/rotaryio/__init__.h"
#include "shared-bindings/rotaryio/IncrementalEncoder.h"

//| """Support for reading rotation sensors
//|
//| The `rotaryio` module contains classes to read different rotation encoding schemes. See
//| `Wikipedia's Rotary Encoder page <https://en.wikipedia.org/wiki/Rotary_encoder>`_ for more
//| background.
//|
//| For more information on working with rotary encoders using this library, see
//| `this Learn Guide <https://learn.adafruit.com/rotary-encoder>`_.
//|
//| All classes change hardware state and should be deinitialized when they
//| are no longer needed if the program continues after use. To do so, either
//| call :py:meth:`!deinit` or use a context manager. See
//| :ref:`lifetime-and-contextmanagers` for more info."""

static const mp_rom_map_elem_t rotaryio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_rotaryio) },
    { MP_ROM_QSTR(MP_QSTR_IncrementalEncoder), MP_ROM_PTR(&rotaryio_incrementalencoder_type) },
};

static MP_DEFINE_CONST_DICT(rotaryio_module_globals, rotaryio_module_globals_table);

const mp_obj_module_t rotaryio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&rotaryio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_rotaryio, rotaryio_module);
