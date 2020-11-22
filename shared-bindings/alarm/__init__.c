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

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/alarm/__init__.h"
#include "shared-bindings/alarm/ResetReason.h"
#include "shared-bindings/alarm/pin/PinAlarm.h"
#include "shared-bindings/alarm/time/DurationAlarm.h"

//| """Power-saving light and deep sleep. Alarms can be set to wake up from sleep.
//|
//| The `alarm` module provides sleep related functionality. There are two supported levels of
//| sleep, light and deep.
//|
//| Light sleep leaves the CPU and RAM powered so that CircuitPython can resume where it left off
//| after being woken up. CircuitPython automatically goes into a light sleep when `time.sleep()` is
//| called. To light sleep until a non-time alarm use `alarm.sleep_until_alarm()`. Any active
//| peripherals, such as I2C, are left on.
//|
//| Deep sleep shuts down power to nearly all of the chip including the CPU and RAM. This can save
//| a more significant amount of power, but CircuitPython must start ``code.py`` from the beginning when woken
//| up. CircuitPython will enter deep sleep automatically when the current program exits without error
//| or calls ``sys.exit(0)``.
//| If an error causes CircuitPython to exit, error LED error flashes will be done periodically.
//| An error includes an uncaught exception, or sys.exit called with a non-zero argumetn.
//| To set alarms for deep sleep use `alarm.restart_on_alarm()` they will apply to next deep sleep only."""
//|
//| wake_alarm: Alarm
//| """The most recent alarm to wake us up from a sleep (light or deep.)"""
//|

//| def sleep_until_alarm(*alarms: Alarm) -> Alarm:
//|     """Performs a light sleep until woken by one of the alarms. The alarm that triggers the wake
//|        is returned, and is also available as `alarm.wake_alarm`
//|     """
//|     ...
//|
STATIC mp_obj_t alarm_sleep_until_alarm(size_t n_args, const mp_obj_t *args) {
    // TODO
    common_hal_alarm_sleep_until_alarm(size_t n_args, const mp_obj_t *args);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(alarm_sleep_until_alarm_obj, 1, MP_OBJ_FUN_ARGS_MAX, alarm_sleep_until_alarm);

//| def restart_on_alarm(*alarms: Alarm) -> None:
//|     """Set one or more alarms to wake up from a deep sleep.
//|     When awakened, ``code.py`` will restart from the beginning.
//|     The last alarm to wake us up is available as `alarm.wake_alarm`.
//|     """
//|     ...
//|
STATIC mp_obj_t alarm_restart_on_alarm(size_t n_args, const mp_obj_t *args) {
    // TODO
    common_hal_alarm_restart_on_alarm(size_t n_args, const mp_obj_t *args);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(alarm_restart_on_alarm_obj, 1, MP_OBJ_FUN_ARGS_MAX, alarm_restart_on_alarm);

//| """The `alarm.pin` module contains alarm attributes and classes related to pins.
//| """
//|
STATIC const mp_map_elem_t alarm_pin_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_pin) },

    { MP_ROM_QSTR(MP_QSTR_PinAlarm), MP_OBJ_FROM_PTR(&alarm_pin_pin_alarm_type) },
};

STATIC MP_DEFINE_CONST_DICT(alarm_pin_globals, alarm_pin_globals_table);

STATIC const mp_obj_module_t alarm_pin_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&alarm_pin_globals,
};

//| """The `alarm.time` module contains alarm attributes and classes related to time-keeping.
//| """
//|
STATIC const mp_map_elem_t alarm_time_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_time) },

    { MP_ROM_QSTR(MP_QSTR_DurationAlarm), MP_OBJ_FROM_PTR(&alarm_time_duration_alarm_type) },
};

STATIC MP_DEFINE_CONST_DICT(alarm_time_globals, alarm_time_globals_table);

STATIC const mp_obj_module_t alarm_time_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&alarm_time_globals,
};

STATIC mp_map_elem_t alarm_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_alarm) },

    // wake_alarm and reset_reason are mutable attributes.
    { MP_ROM_QSTR(MP_QSTR_wake_alarm), mp_const_none },

    { MP_ROM_QSTR(MP_QSTR_sleep_until_alarm), MP_OBJ_FROM_PTR(&alarm_sleep_until_alarm_obj) },
    { MP_ROM_QSTR(MP_QSTR_restart_on_alarm), MP_OBJ_FROM_PTR(&alarm_restart_on_alarm_obj) },

    { MP_ROM_QSTR(MP_QSTR_pin), MP_OBJ_FROM_PTR(&alarm_pin_module) },
    { MP_ROM_QSTR(MP_QSTR_time), MP_OBJ_FROM_PTR(&alarm_time_module) }

};
STATIC MP_DEFINE_MUTABLE_DICT(alarm_module_globals, alarm_module_globals_table);

void common_hal_alarm_set_wake_alarm(mp_obj_t alarm) {
    // Equivalent of:
    // alarm.wake_alarm = alarm
    mp_map_elem_t *elem =
        mp_map_lookup(&alarm_module_globals.map, MP_ROM_QSTR(MP_QSTR_wake_alarm), MP_MAP_LOOKUP);
    if (elem) {
        elem->value = alarm;
    }
}

const mp_obj_module_t alarm_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&alarm_module_globals,
};
