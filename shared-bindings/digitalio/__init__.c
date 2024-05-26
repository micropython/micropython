// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/digitalio/__init__.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/digitalio/Direction.h"
#include "shared-bindings/digitalio/DriveMode.h"
#include "shared-bindings/digitalio/Pull.h"

#include "py/runtime.h"

//| """Basic digital pin support
//|
//| The `digitalio` module contains classes to provide access to basic digital IO.
//|
//| All classes change hardware state and should be deinitialized when they
//| are no longer needed if the program continues after use. To do so, either
//| call :py:meth:`!deinit` or use a context manager. See
//| :ref:`lifetime-and-contextmanagers` for more info.
//|
//| For example::
//|
//|   import digitalio
//|   import board
//|
//|   pin = digitalio.DigitalInOut(board.LED)
//|   print(pin.value)
//|
//| This example will initialize the the device, read
//| :py:data:`~digitalio.DigitalInOut.value` and then
//| :py:meth:`~digitalio.DigitalInOut.deinit` the hardware.
//|
//| Here is blinky::
//|
//|   import time
//|   import digitalio
//|   import board
//|
//|   led = digitalio.DigitalInOut(board.LED)
//|   led.direction = digitalio.Direction.OUTPUT
//|   while True:
//|       led.value = True
//|       time.sleep(0.1)
//|       led.value = False
//|       time.sleep(0.1)
//|
//| For the essentials of `digitalio`, see the `CircuitPython Essentials
//| Learn guide <https://learn.adafruit.com/circuitpython-essentials/circuitpython-digital-in-out>`_
//|
//| For more information on using `digitalio`, see `this additional Learn guide
//| <https://learn.adafruit.com/circuitpython-digital-inputs-and-outputs>`_
//| """

static const mp_rom_map_elem_t digitalio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_digitalio) },
    { MP_ROM_QSTR(MP_QSTR_DigitalInOut),  MP_ROM_PTR(&digitalio_digitalinout_type) },

    // Enum-like Classes.
    { MP_ROM_QSTR(MP_QSTR_Direction),          MP_ROM_PTR(&digitalio_direction_type) },
    { MP_ROM_QSTR(MP_QSTR_DriveMode),          MP_ROM_PTR(&digitalio_drive_mode_type) },
    { MP_ROM_QSTR(MP_QSTR_Pull),               MP_ROM_PTR(&digitalio_pull_type) },
};

static MP_DEFINE_CONST_DICT(digitalio_module_globals, digitalio_module_globals_table);

const mp_obj_module_t digitalio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&digitalio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_digitalio, digitalio_module);
