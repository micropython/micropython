// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/touchio/__init__.h"
#include "shared-bindings/touchio/TouchIn.h"

#include "py/runtime.h"

//| """Touch related IO
//|
//| The `touchio` module contains classes to provide access to touch IO typically
//| accelerated by hardware on the onboard microcontroller.
//|
//| All classes change hardware state and should be deinitialized when they
//| are no longer needed if the program continues after use. To do so, either
//| call :py:meth:`!deinit` or use a context manager. See
//| :ref:`lifetime-and-contextmanagers` for more info.
//|
//| For more information about working with the `touchio` module in CircuitPython,
//| see `this Learn guide page <https://learn.adafruit.com/circuitpython-essentials/circuitpython-cap-touch>`_.
//|
//| Example::
//|
//|   import touchio
//|   from board import *
//|
//|   touch_pin = touchio.TouchIn(D6)
//|   print(touch_pin.value)
//|
//| This example will initialize the the device, and print the
//| :py:data:`~touchio.TouchIn.value`."""

static const mp_rom_map_elem_t touchio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_touchio) },
    { MP_ROM_QSTR(MP_QSTR_TouchIn),   MP_ROM_PTR(&touchio_touchin_type) },
};

static MP_DEFINE_CONST_DICT(touchio_module_globals, touchio_module_globals_table);

const mp_obj_module_t touchio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&touchio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_touchio, touchio_module);
