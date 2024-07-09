// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/pwmio/__init__.h"
#include "shared-bindings/pwmio/PWMOut.h"

//| """Support for PWM based protocols
//|
//| The `pwmio` module contains classes to provide access to basic pulse IO.
//|
//| All classes change hardware state and should be deinitialized when they
//| are no longer needed if the program continues after use. To do so, either
//| call :py:meth:`!deinit` or use a context manager. See
//| :ref:`lifetime-and-contextmanagers` for more info.
//|
//| For example::
//|
//|   import time
//|   import pwmio
//|   import board
//|
//|   pwm = pwmio.PWMOut(board.LED)
//|   pwm.duty_cycle = 2 ** 15
//|   time.sleep(0.1)
//|
//| This example will initialize the the device, set
//| :py:data:`~pwmio.PWMOut.duty_cycle`, and then sleep 0.1 seconds.
//| CircuitPython will automatically turn off the PWM when it resets all
//| hardware after program completion. Use ``deinit()`` or a ``with`` statement
//| to do it yourself.
//|
//| For the essentials of `pwmio`, see the `CircuitPython Essentials
//| Learn guide <https://learn.adafruit.com/circuitpython-essentials/circuitpython-pwm>`_.
//| """

static const mp_rom_map_elem_t pwmio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_pwmio) },
    { MP_ROM_QSTR(MP_QSTR_PWMOut), MP_ROM_PTR(&pwmio_pwmout_type) },
};

static MP_DEFINE_CONST_DICT(pwmio_module_globals, pwmio_module_globals_table);

const mp_obj_module_t pwmio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&pwmio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_pwmio, pwmio_module);
