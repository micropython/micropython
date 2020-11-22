/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Dan Halbert for Adafruit Industries
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

#include "shared-bindings/board/__init__.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/alarm/pin/PinAlarm.h"

#include "py/nlr.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "supervisor/shared/translate.h"

//| class PinAlarm:
//|     """Trigger an alarm when a pin changes state."""
//|
//|     def __init__(self, pin: microcontroller.Pin, level: bool, *, edge: bool = False, pull: bool = False) -> None:
//|         """Create an alarm triggered by a `~microcontroller.Pin` level. The alarm is not active
//|         until it is listed in an `alarm`-enabling function, such as `alarm.sleep_until_alarm()` or
//|         `alarm.restart_on_alarm()`.
//|
//|         :param ~microcontroller.Pin pin: The pin to monitor. On some ports, the choice of pin
//|         may be limited due to hardware restrictions, particularly for deep-sleep alarms.
//|         :param bool level: When active, trigger when the level is high (``True``) or low (``False``).
//|         On some ports, multiple `PinAlarm` objects may need to have coordinated levels
//|         for deep-sleep alarms.
//|         :param bool edge: If ``True``, trigger only when there is a transition to the specified
//|         value of `level`. If ``True``, if the alarm becomes active when the pin level already
//|         matches `level`, the alarm is not triggered: the pin must transition from ``not level``
//|         to ``level`` to trigger the alarm. On some ports, edge-triggering may not be available,
//|         particularly for deep-sleep alarms.
//|         :param bool pull: Enable a pull-up or pull-down which pulls the pin to level opposite
//|         opposite that of `level`. For instance, if `level` is set to ``True``, setting `pull`
//|         to ``True`` will enable a pull-down, to hold the pin low normally until an outside signal
//|         pulls it high.
//|         """
//|         ...
//|
STATIC mp_obj_t alarm_pin_pin_alarm_make_new(const mp_obj_type_t *type, mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    alarm_pin_pin_alarm_obj_t *self = m_new_obj(alarm_pin_pin_alarm_obj_t);
    self->base.type = &alarm_pin_pin_alarm_type;
    enum { ARG_pin, ARG_level, ARG_edge, ARG_pull };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pin, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_level, MP_ARG_REQUIRED | MP_ARG_BOOL },
        { MP_QSTR_edge, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_pull, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mcu_pin_obj_t* pin = validate_obj_is_free_pin(args[ARG_pin].u_obj);

    common_hal_alarm_pin_pin_alarm_construct(
        self, pin, args[ARG_level].u_bool, args[ARG_edge].u_bool, args[ARG_pull].u_bool);

    return MP_OBJ_FROM_PTR(self);
}

//|     def __eq__(self, other: object) -> bool:
//|         """Two PinAlarm objects are equal if their durations are the same if their pin,
//|         level, edge, and pull attributes are all the same."""
//|         ...
//|
STATIC mp_obj_t alarm_pin_pin_alarm_binary_op(mp_binary_op_t op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    switch (op) {
        case MP_BINARY_OP_EQUAL:
            if (MP_OBJ_IS_TYPE(rhs_in, &alarm_pin_pin_alarm_type)) {
                // Pins are singletons, so we can compare them directly.
                return mp_obj_new_bool(
                    common_hal_alarm_pin_pin_alarm_get_pin(lhs_in) ==
                    common_hal_alarm_pin_pin_alarm_get_pin(rhs_in) &&
                    common_hal_alarm_pin_pin_alarm_get_level(lhs_in) ==
                    common_hal_alarm_pin_pin_alarm_get_level(rhs_in) &&
                    common_hal_alarm_pin_pin_alarm_get_edge(lhs_in) ==
                    common_hal_alarm_pin_pin_alarm_get_edge(rhs_in) &&
                    common_hal_alarm_pin_pin_alarm_get_pull(lhs_in) ==
                    common_hal_alarm_pin_pin_alarm_get_pull(rhs_in));
            }
            return mp_const_false;

        default:
            return MP_OBJ_NULL; // op not supported
    }
}

STATIC const mp_rom_map_elem_t alarm_pin_pin_alarm_locals_dict_table[] = {
};

STATIC MP_DEFINE_CONST_DICT(alarm_pin_pin_alarm_locals_dict, alarm_pin_pin_alarm_locals_dict_table);

const mp_obj_type_t alarm_pin_pin_alarm_type = {
    { &mp_type_type },
    .name = MP_QSTR_PinAlarm,
    .make_new = alarm_pin_pin_alarm_make_new,
    .binary_op = alarm_pin_pin_alarm_binary_op,
    .locals_dict = (mp_obj_t)&alarm_pin_pin_alarm_locals_dict,
};
