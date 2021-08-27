/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

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
//|   import pwmio
//|   import time
//|   from board import *
//|
//|   pwm = pwmio.PWMOut(D13)
//|   pwm.duty_cycle = 2 ** 15
//|   time.sleep(0.1)
//|
//| This example will initialize the the device, set
//| :py:data:`~pwmio.PWMOut.duty_cycle`, and then sleep 0.1 seconds.
//| CircuitPython will automatically turn off the PWM when it resets all
//| hardware after program completion. Use ``deinit()`` or a ``with`` statement
//| to do it yourself."""
//|

STATIC const mp_rom_map_elem_t pwmio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_pwmio) },
    { MP_ROM_QSTR(MP_QSTR_PWMOut), MP_ROM_PTR(&pwmio_pwmout_type) },
};

STATIC MP_DEFINE_CONST_DICT(pwmio_module_globals, pwmio_module_globals_table);

const mp_obj_module_t pwmio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&pwmio_module_globals,
};
