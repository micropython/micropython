// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/analogio/__init__.h"
#include "shared-bindings/analogio/AnalogIn.h"
#include "shared-bindings/analogio/AnalogOut.h"

//| """Analog hardware support
//|
//| The `analogio` module contains classes to provide access to analog IO
//| typically implemented with digital-to-analog (DAC) and analog-to-digital
//| (ADC) converters.
//|
//| All classes change hardware state and should be deinitialized when they
//| are no longer needed if the program continues after use. To do so, either
//| call :py:meth:`!deinit` or use a context manager. See
//| :ref:`lifetime-and-contextmanagers` for more info.
//|
//| For example::
//|
//|   import analogio
//|   from board import *
//|
//|   pin = analogio.AnalogIn(A0)
//|   print(pin.value)
//|   pin.deinit()
//|
//| This example will initialize the the device, read
//| :py:data:`~analogio.AnalogIn.value` and then
//| :py:meth:`~analogio.AnalogIn.deinit` the hardware. The last step is optional
//| because CircuitPython will do it automatically after the program finishes.
//|
//| For the essentials of `analogio`, see the `CircuitPython Essentials
//| Learn guide <https://learn.adafruit.com/circuitpython-essentials/circuitpython-analog-in>`_
//|
//| For more information on using `analogio`, see `this additional Learn guide
//| <https://learn.adafruit.com/circuitpython-basics-analog-inputs-and-outputs>`_
//| """

static const mp_rom_map_elem_t analogio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_analogio) },
    { MP_ROM_QSTR(MP_QSTR_AnalogIn),   MP_ROM_PTR(&analogio_analogin_type) },
    { MP_ROM_QSTR(MP_QSTR_AnalogOut),   MP_ROM_PTR(&analogio_analogout_type) },
};

static MP_DEFINE_CONST_DICT(analogio_module_globals, analogio_module_globals_table);

const mp_obj_module_t analogio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&analogio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_analogio, analogio_module);
