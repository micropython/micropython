/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 microDev
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

#include "shared-bindings/alarm/touch/TouchAlarm.h"
#include "shared-bindings/board/__init__.h"

#include "py/objproperty.h"

//| class TouchAlarm:
//|     """Trigger an alarm when touch is detected."""
//|
//|     def __init__(self, *pin: microcontroller.Pin) -> None:
//|         """Create an alarm that will be triggered when the given pin is touched.
//|         The alarm is not active until it is passed to an `alarm`-enabling function, such as
//|         `alarm.light_sleep_until_alarms()` or `alarm.exit_and_deep_sleep_until_alarms()`.
//|
//|         :param microcontroller.Pin pin: The pin to monitor. On some ports, the choice of pin
//|           may be limited due to hardware restrictions, particularly for deep-sleep alarms.
//|         """
//|         ...
//|
STATIC mp_obj_t alarm_touch_touchalarm_make_new(const mp_obj_type_t *type,
    mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    alarm_touch_touchalarm_obj_t *self = m_new_obj(alarm_touch_touchalarm_obj_t);
    self->base.type = &alarm_touch_touchalarm_type;

    enum { ARG_pin };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pin, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mcu_pin_obj_t *pin = validate_obj_is_free_pin(args[ARG_pin].u_obj);

    common_hal_alarm_touch_touchalarm_construct(self, pin);

    return MP_OBJ_FROM_PTR(self);
}

//|     pin: microcontroller.Pin
//|     """The trigger pin."""
//|
STATIC mp_obj_t alarm_touch_touchalarm_obj_get_pin(mp_obj_t self_in) {
    alarm_touch_touchalarm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_FROM_PTR(self->pin);
}
MP_DEFINE_CONST_FUN_OBJ_1(alarm_touch_touchalarm_get_pin_obj, alarm_touch_touchalarm_obj_get_pin);

const mp_obj_property_t alarm_touch_touchalarm_pin_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&alarm_touch_touchalarm_get_pin_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

STATIC const mp_rom_map_elem_t alarm_touch_touchalarm_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_pin), MP_ROM_PTR(&alarm_touch_touchalarm_pin_obj) },
};
STATIC MP_DEFINE_CONST_DICT(alarm_touch_touchalarm_locals_dict, alarm_touch_touchalarm_locals_dict_table);

const mp_obj_type_t alarm_touch_touchalarm_type = {
    { &mp_type_type },
    .name = MP_QSTR_TouchAlarm,
    .make_new = alarm_touch_touchalarm_make_new,
    .locals_dict = (mp_obj_t)&alarm_touch_touchalarm_locals_dict,
};
