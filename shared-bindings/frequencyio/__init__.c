// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Michael Schroeder
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/frequencyio/__init__.h"
#include "shared-bindings/frequencyio/FrequencyIn.h"

//| """Support for frequency based protocols
//|
//| .. warning:: This module is not available in SAMD21 builds. See the
//|   :ref:`module-support-matrix` for more info.
//|
//| All classes change hardware state and should be deinitialized when they
//| are no longer needed if the program continues after use. To do so, either
//| call :py:meth:`!deinit` or use a context manager. See
//| :ref:`lifetime-and-contextmanagers` for more info.
//|
//| For example::
//|
//|   import time
//|   import frequencyio
//|   import board
//|
//|   frequency = frequencyio.FrequencyIn(board.D11)
//|   frequency.capture_period = 15
//|   time.sleep(0.1)
//|
//| This example will initialize the the device, set
//| :py:data:`~frequencyio.FrequencyIn.capture_period`, and then sleep 0.1 seconds.
//| CircuitPython will automatically turn off FrequencyIn capture when it resets all
//| hardware after program completion. Use ``deinit()`` or a ``with`` statement
//| to do it yourself."""

static const mp_rom_map_elem_t frequencyio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_frequencyio) },
    { MP_ROM_QSTR(MP_QSTR_FrequencyIn), MP_ROM_PTR(&frequencyio_frequencyin_type) },
};

static MP_DEFINE_CONST_DICT(frequencyio_module_globals, frequencyio_module_globals_table);

const mp_obj_module_t frequencyio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&frequencyio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_frequencyio, frequencyio_module);
