// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/audioio/__init__.h"
#include "shared-bindings/audioio/AudioOut.h"

//| """Support for audio output
//|
//| The `audioio` module contains classes to provide access to audio IO.
//|
//| All classes change hardware state and should be deinitialized when they
//| are no longer needed if the program continues after use. To do so, either
//| call :py:meth:`!deinit` or use a context manager. See
//| :ref:`lifetime-and-contextmanagers` for more info.
//|
//| For more information on working with this module, refer to the
//| `CircuitPython Essentials Learn Guide
//| <https://learn.adafruit.com/circuitpython-essentials/circuitpython-audio-out>`_.
//|
//| Since CircuitPython 5, `RawSample` and `WaveFile` are moved
//| to :mod:`audiocore`, and `Mixer` is moved to :mod:`audiomixer`.
//|
//| For compatibility with CircuitPython 4.x, some builds allow the items in
//| `audiocore` to be imported from `audioio`.  This will be removed for all
//| boards in a future build of CircuitPython."""

static const mp_rom_map_elem_t audioio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_audioio) },
    { MP_ROM_QSTR(MP_QSTR_AudioOut), MP_ROM_PTR(&audioio_audioout_type) },
};

static MP_DEFINE_CONST_DICT(audioio_module_globals, audioio_module_globals_table);

const mp_obj_module_t audioio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&audioio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_audioio, audioio_module);
