// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/pulseio/__init__.h"
#include "shared-bindings/pulseio/PulseIn.h"
#include "shared-bindings/pulseio/PulseOut.h"

//| """Support for individual pulse based protocols
//|
//| The `pulseio` module contains classes to provide access to basic pulse IO.
//| Individual pulses are commonly used in infrared remotes and in DHT
//| temperature sensors.
//|
//| All classes change hardware state and should be deinitialized when they
//| are no longer needed if the program continues after use. To do so, either
//| call :py:meth:`!deinit` or use a context manager. See
//| :ref:`lifetime-and-contextmanagers` for more info."""

static const mp_rom_map_elem_t pulseio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_pulseio) },
    { MP_ROM_QSTR(MP_QSTR_PulseIn), MP_ROM_PTR(&pulseio_pulsein_type) },
    { MP_ROM_QSTR(MP_QSTR_PulseOut), MP_ROM_PTR(&pulseio_pulseout_type) },
};

static MP_DEFINE_CONST_DICT(pulseio_module_globals, pulseio_module_globals_table);

const mp_obj_module_t pulseio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&pulseio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_pulseio, pulseio_module);
