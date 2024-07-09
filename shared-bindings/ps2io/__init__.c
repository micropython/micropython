// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Elvis Pfutzenreuter <epxx@epxx.co>
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/ps2io/Ps2.h"

//| """Support for PS/2 protocol
//|
//| The `ps2io` module contains classes to provide PS/2 communication.
//|
//| .. warning:: This module is not available in some SAMD21 builds. See the
//|   :ref:`module-support-matrix` for more info.
//|
//| All classes change hardware state and should be deinitialized when they
//| are no longer needed if the program continues after use. To do so, either
//| call :py:meth:`!deinit` or use a context manager. See
//| :ref:`lifetime-and-contextmanagers` for more info."""

static const mp_rom_map_elem_t ps2io_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ps2io) },
    { MP_ROM_QSTR(MP_QSTR_Ps2), MP_ROM_PTR(&ps2io_ps2_type) },
};

static MP_DEFINE_CONST_DICT(ps2io_module_globals, ps2io_module_globals_table);

const mp_obj_module_t ps2io_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&ps2io_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_ps2io, ps2io_module);
