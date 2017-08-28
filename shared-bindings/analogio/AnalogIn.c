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

#include <string.h>

#include "lib/utils/context_manager_helpers.h"
#include "py/binary.h"
#include "py/mphal.h"
#include "py/nlr.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/analogio/AnalogIn.h"

//| .. currentmodule:: analogio
//|
//| :class:`AnalogIn` -- read analog voltage
//| ============================================
//|
//| Usage::
//|
//|    import analogio
//|    from board import *
//|
//|    adc = analogio.AnalogIn(A1)
//|    val = adc.value
//|

//| .. class:: AnalogIn(pin)
//|
//|   Use the AnalogIn on the given pin. The reference voltage varies by
//|   platform so use ``reference_voltage`` to read the configured setting.
//|
//|   :param ~microcontroller.Pin pin: the pin to read from
//|
STATIC mp_obj_t analogio_analogin_make_new(const mp_obj_type_t *type,
        mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check number of arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    // 1st argument is the pin
    mp_obj_t pin_obj = args[0];
    assert_pin(pin_obj, false);

    analogio_analogin_obj_t *self = m_new_obj(analogio_analogin_obj_t);
    self->base.type = &analogio_analogin_type;
    const mcu_pin_obj_t *pin = MP_OBJ_TO_PTR(pin_obj);
    assert_pin_free(pin);
    common_hal_analogio_analogin_construct(self, pin);

    return (mp_obj_t) self;
}

//|   .. method:: deinit()
//|
//|      Turn off the AnalogIn and release the pin for other use.
//|
STATIC mp_obj_t analogio_analogin_deinit(mp_obj_t self_in) {
   analogio_analogin_obj_t *self = MP_OBJ_TO_PTR(self_in);
   common_hal_analogio_analogin_deinit(self);
   return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(analogio_analogin_deinit_obj, analogio_analogin_deinit);

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
STATIC mp_obj_t analogio_analogin___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_analogio_analogin_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(analogio_analogin___exit___obj, 4, 4, analogio_analogin___exit__);

//|   .. attribute:: value
//|
//|     Read the value on the analog pin and return it.  The returned value
//|     will be between 0 and 65535 inclusive (16-bit). Even if the underlying
//|     analog to digital converter (ADC) is lower resolution, the result will
//|     be scaled to be 16-bit.
//|
//|     :return: the data read
//|     :rtype: int
//|
STATIC mp_obj_t analogio_analogin_obj_get_value(mp_obj_t self_in) {
   analogio_analogin_obj_t *self = MP_OBJ_TO_PTR(self_in);
   return MP_OBJ_NEW_SMALL_INT(common_hal_analogio_analogin_get_value(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(analogio_analogin_get_value_obj, analogio_analogin_obj_get_value);

const mp_obj_property_t analogio_analogin_value_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&analogio_analogin_get_value_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|   .. attribute:: reference_voltage
//|
//|     The maximum voltage measurable. Also known as the reference voltage.
//|
//|     :return: the reference voltage
//|     :rtype: float
//|
STATIC mp_obj_t analogio_analogin_obj_get_reference_voltage(mp_obj_t self_in) {
   analogio_analogin_obj_t *self = MP_OBJ_TO_PTR(self_in);
   return mp_obj_new_float(common_hal_analogio_analogin_get_reference_voltage(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(analogio_analogin_get_reference_voltage_obj,
                          analogio_analogin_obj_get_reference_voltage);

const mp_obj_property_t analogio_analogin_reference_voltage_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&analogio_analogin_get_reference_voltage_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC const mp_rom_map_elem_t analogio_analogin_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit),             MP_ROM_PTR(&analogio_analogin_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__),          MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__),           MP_ROM_PTR(&analogio_analogin___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_value),              MP_ROM_PTR(&analogio_analogin_value_obj)},
    { MP_ROM_QSTR(MP_QSTR_reference_voltage),  MP_ROM_PTR(&analogio_analogin_reference_voltage_obj)},
};

STATIC MP_DEFINE_CONST_DICT(analogio_analogin_locals_dict, analogio_analogin_locals_dict_table);

const mp_obj_type_t analogio_analogin_type = {
    { &mp_type_type },
    .name = MP_QSTR_AnalogIn,
    .make_new = analogio_analogin_make_new,
    .locals_dict = (mp_obj_t)&analogio_analogin_locals_dict,
};
