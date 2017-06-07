/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#include <string.h>

#include "lib/utils/context_manager_helpers.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/analogio/AnalogOut.h"

//| .. currentmodule:: analogio
//|
//| :class:`AnalogOut` -- output analog voltage
//| ============================================
//|
//| The AnalogOut is used to output analog values (a specific voltage).
//|
//| Example usage::
//|
//|     import analogio
//|     from microcontroller import pin
//|
//|     dac = analogio.AnalogOut(pin.PA02)          # output on pin PA02
//|     dac.value = 32768                           # makes PA02 1.65V
//|

//| .. class:: AnalogOut(pin)
//|
//|   Use the AnalogOut on the given pin.
//|
//|   :param ~microcontroller.Pin pin: the pin to output to
//|
STATIC mp_obj_t analogio_analogout_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    assert_pin(args[0], false);
    const mcu_pin_obj_t *pin = MP_OBJ_TO_PTR(args[0]);

    analogio_analogout_obj_t *self = m_new_obj(analogio_analogout_obj_t);
    self->base.type = &analogio_analogout_type;
    assert_pin_free(pin);
    common_hal_analogio_analogout_construct(self, pin);

    return self;
}

//|   .. method:: deinit()
//|
//|     Turn off the AnalogOut and release the pin for other use.
//|
STATIC mp_obj_t analogio_analogout_deinit(mp_obj_t self_in) {
    analogio_analogout_obj_t *self = self_in;

    common_hal_analogio_analogout_deinit(self);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(analogio_analogout_deinit_obj, analogio_analogout_deinit);

//|   .. method:: __enter__()
//|
//|      No-op used by Context Managers.
//|
//  Provided by context manager helper.

//|   .. method:: __exit__()
//|
//|      Automatically deinitializes the hardware when exiting a context. See
//|      :ref:`lifetime-and-contextmanagers` for more info.
//|
STATIC mp_obj_t analogio_analogout___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_analogio_analogout_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(analogio_analogout___exit___obj, 4, 4, analogio_analogout___exit__);

//|   .. attribute:: value
//|
//|     The value on the analog pin.  The value must be between 0 and 65535
//|     inclusive (16-bit). Even if the underlying digital to analog converter
//|     is lower resolution, the input must be scaled to be 16-bit.
//|
//|     :return: the last value written
//|     :rtype: int
//|
STATIC mp_obj_t analogio_analogout_obj_set_value(mp_obj_t self_in, mp_obj_t value) {
   analogio_analogout_obj_t *self = MP_OBJ_TO_PTR(self_in);
   uint32_t v = mp_obj_get_int(value);
   if (v >= (1 << 16)) {
       mp_raise_ValueError("AnalogOut is only 16 bits. Value must be less than 65536.");
   }
   common_hal_analogio_analogout_set_value(self, v);
   return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(analogio_analogout_set_value_obj, analogio_analogout_obj_set_value);

const mp_obj_property_t analogio_analogout_value_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&analogio_analogout_set_value_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC const mp_rom_map_elem_t analogio_analogout_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&analogio_analogout_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__),  MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__),   MP_ROM_PTR(&analogio_analogout___exit___obj) },

    // Properties
    { MP_OBJ_NEW_QSTR(MP_QSTR_value), (mp_obj_t)&analogio_analogout_value_obj },
};

STATIC MP_DEFINE_CONST_DICT(analogio_analogout_locals_dict, analogio_analogout_locals_dict_table);

const mp_obj_type_t analogio_analogout_type = {
    { &mp_type_type },
    .name = MP_QSTR_AnalogOut,
    .make_new = analogio_analogout_make_new,
    .locals_dict = (mp_obj_t)&analogio_analogout_locals_dict,
};
