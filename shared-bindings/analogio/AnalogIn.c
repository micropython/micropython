/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
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
#include "shared-bindings/util.h"

//| class AnalogIn:
//|     """Read analog voltage levels
//|
//|     Usage::
//|
//|        import analogio
//|        from board import *
//|
//|        adc = analogio.AnalogIn(A1)
//|        val = adc.value"""
//|

//|     def __init__(self, pin: microcontroller.Pin) -> None:
//|         """Use the AnalogIn on the given pin. The reference voltage varies by
//|         platform so use ``reference_voltage`` to read the configured setting.
//|
//|         :param ~microcontroller.Pin pin: the pin to read from"""
//|         ...
//|
STATIC mp_obj_t analogio_analogin_make_new(const mp_obj_type_t *type,
    mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    // check number of arguments
    mp_arg_check_num(n_args, kw_args, 1, 1, false);

    // 1st argument is the pin
    const mcu_pin_obj_t *pin = validate_obj_is_free_pin(args[0]);

    analogio_analogin_obj_t *self = m_new_obj(analogio_analogin_obj_t);
    self->base.type = &analogio_analogin_type;
    common_hal_analogio_analogin_construct(self, pin);

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Turn off the AnalogIn and release the pin for other use."""
//|         ...
//|
STATIC mp_obj_t analogio_analogin_deinit(mp_obj_t self_in) {
    analogio_analogin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_analogio_analogin_deinit(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(analogio_analogin_deinit_obj, analogio_analogin_deinit);

STATIC void check_for_deinit(analogio_analogin_obj_t *self) {
    if (common_hal_analogio_analogin_deinited(self)) {
        raise_deinited_error();
    }
}
//|     def __enter__(self) -> AnalogIn:
//|         """No-op used by Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
STATIC mp_obj_t analogio_analogin___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_analogio_analogin_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(analogio_analogin___exit___obj, 4, 4, analogio_analogin___exit__);

//|     value: int
//|     """The value on the analog pin between 0 and 65535 inclusive (16-bit). (read-only)
//|
//|     Even if the underlying analog to digital converter (ADC) is lower
//|     resolution, the value is 16-bit."""
//|
STATIC mp_obj_t analogio_analogin_obj_get_value(mp_obj_t self_in) {
    analogio_analogin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_analogio_analogin_get_value(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(analogio_analogin_get_value_obj, analogio_analogin_obj_get_value);

const mp_obj_property_t analogio_analogin_value_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&analogio_analogin_get_value_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

//|     reference_voltage: float
//|     """The maximum voltage measurable (also known as the reference voltage) as a
//|     `float` in Volts."""
//|
STATIC mp_obj_t analogio_analogin_obj_get_reference_voltage(mp_obj_t self_in) {
    analogio_analogin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    float reference_voltage = common_hal_analogio_analogin_get_reference_voltage(self);
    if (reference_voltage <= 0.0f) {
        return mp_const_none;
    } else {
        return mp_obj_new_float(reference_voltage);
    }
}
MP_DEFINE_CONST_FUN_OBJ_1(analogio_analogin_get_reference_voltage_obj,
    analogio_analogin_obj_get_reference_voltage);

const mp_obj_property_t analogio_analogin_reference_voltage_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&analogio_analogin_get_reference_voltage_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
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
