// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
//
// SPDX-License-Identifier: MIT

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
//|
//|         **Limitations:** Not available on SAMD, Nordic, or RP2040.
//|         """
//|         ...
static mp_obj_t alarm_touch_touchalarm_make_new(const mp_obj_type_t *type,
    size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    alarm_touch_touchalarm_obj_t *self = mp_obj_malloc(alarm_touch_touchalarm_obj_t, &alarm_touch_touchalarm_type);

    enum { ARG_pin };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pin, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mcu_pin_obj_t *pin = validate_obj_is_free_pin(args[ARG_pin].u_obj, MP_QSTR_pin);

    common_hal_alarm_touch_touchalarm_construct(self, pin);

    return MP_OBJ_FROM_PTR(self);
}

//|     pin: microcontroller.Pin
//|     """The trigger pin."""
//|
static mp_obj_t alarm_touch_touchalarm_obj_get_pin(mp_obj_t self_in) {
    alarm_touch_touchalarm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_FROM_PTR(self->pin);
}
MP_DEFINE_CONST_FUN_OBJ_1(alarm_touch_touchalarm_get_pin_obj, alarm_touch_touchalarm_obj_get_pin);

MP_PROPERTY_GETTER(alarm_touch_touchalarm_pin_obj,
    (mp_obj_t)&alarm_touch_touchalarm_get_pin_obj);

static const mp_rom_map_elem_t alarm_touch_touchalarm_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_pin), MP_ROM_PTR(&alarm_touch_touchalarm_pin_obj) },
};
static MP_DEFINE_CONST_DICT(alarm_touch_touchalarm_locals_dict, alarm_touch_touchalarm_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    alarm_touch_touchalarm_type,
    MP_QSTR_TouchAlarm,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, alarm_touch_touchalarm_make_new,
    locals_dict, &alarm_touch_touchalarm_locals_dict
    );
