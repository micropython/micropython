// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/audiopwmio/__init__.h"
#include "shared-bindings/audiopwmio/PWMAudioOut.h"

//| """Audio output via digital PWM
//|
//| The `audiopwmio` module contains classes to provide access to audio IO.
//|
//| All classes change hardware state and should be deinitialized when they
//| are no longer needed if the program continues after use. To do so, either
//| call :py:meth:`!deinit` or use a context manager. See
//| :ref:`lifetime-and-contextmanagers` for more info.
//|
//| Since CircuitPython 5, `Mixer`, `RawSample` and `WaveFile` are moved
//| to :mod:`audiocore`."""

static const mp_rom_map_elem_t audiopwmio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_audiopwmio) },
    { MP_ROM_QSTR(MP_QSTR_PWMAudioOut), MP_ROM_PTR(&audiopwmio_pwmaudioout_type) },
};

static MP_DEFINE_CONST_DICT(audiopwmio_module_globals, audiopwmio_module_globals_table);

const mp_obj_module_t audiopwmio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&audiopwmio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_audiopwmio, audiopwmio_module);
