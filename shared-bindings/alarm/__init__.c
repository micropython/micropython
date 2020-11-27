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
#include "shared-bindings/alarm/pin/PinAlarm.h"
#include "shared-bindings/alarm/time/TimeAlarm.h"
#include "shared-bindings/time/__init__.h"
#include "supervisor/shared/rgb_led_status.h"
#include "supervisor/shared/workflow.h"

// Wait this long to see if USB is being connected (enumeration starting).
#define CIRCUITPY_USB_CONNECTING_DELAY 1
// Wait this long before going into deep sleep if connected. This
// allows the user to ctrl-C before deep sleep starts.
#define CIRCUITPY_USB_CONNECTED_DEEP_SLEEP_DELAY 5

//| """Power-saving light and deep sleep. Alarms can be set to wake up from sleep.
//|
//| The `alarm` module provides sleep related functionality. There are two supported levels of
//| sleep, light and deep.
//|
//| Light sleep leaves the CPU and RAM powered so that CircuitPython can resume where it left off
//| after being woken up. CircuitPython automatically goes into a light sleep when `time.sleep()` is
//| called. To light sleep until a non-time alarm use `alarm.sleep_until_alarms()`. Any active
//| peripherals, such as I2C, are left on.
//|
//| Deep sleep shuts down power to nearly all of the chip including the CPU and RAM. This can save
//| a more significant amount of power, but CircuitPython must start ``code.py`` from the beginning when
//| awakened.
//| """

//|
//| wake_alarm: Alarm
//| """The most recent alarm to wake us up from a sleep (light or deep.)"""
//|
void validate_objs_are_alarms(size_t n_args, const mp_obj_t *objs) {
    for (size_t i = 0; i < n_args; i++) {
        if (MP_OBJ_IS_TYPE(objs[i], &alarm_pin_pin_alarm_type) ||
            MP_OBJ_IS_TYPE(objs[i], &alarm_time_time_alarm_type)) {
            continue;
        }
        mp_raise_TypeError_varg(translate("Expected an alarm"));
    }
}

//| def sleep_until_alarms(*alarms: Alarm) -> Alarm:
//|     """Go into a light sleep until awakened one of the alarms. The alarm causing the wake-up
//|        is returned, and is also available as `alarm.wake_alarm`.
//|     """
//|     ...
//|
STATIC mp_obj_t alarm_sleep_until_alarms(size_t n_args, const mp_obj_t *args) {
    validate_objs_are_alarms(n_args, args);
    common_hal_alarm_sleep_until_alarms(n_args, args);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(alarm_sleep_until_alarms_obj, 1, MP_OBJ_FUN_ARGS_MAX, alarm_sleep_until_alarms);

//| def exit_and_deep_sleep_until_alarms(*alarms: Alarm) -> None:
//|     """Exit the program and go into a deep sleep, until awakened by one of the alarms.
//|
//|     When awakened, the microcontroller will restart and will run ``boot.py`` and ``code.py``
//|     from the beginning.
//|
//|     An alarm equivalent to the one that caused the wake-up is available as `alarm.wake_alarm`.
//|     Its type and/or attributes may not correspond exactly to the original alarm.
//|     For time-base alarms, currently, an `alarm.time.TimeAlarm()` is created.
//|
//|     If no alarms are specified, the microcontroller will deep sleep until reset.
//|
//|     If CircuitPython is unconnected to a host computer, go into deep sleep immediately.
//|     But if it already connected or in the process of connecting to a host computer, wait at least
//|     five seconds after starting code.py before entering deep sleep.
//|     This allows interrupting a program that would otherwise go into deep sleep too quickly
//|     to interrupt from the keyboard.
//|     """
//|     ...
//|
STATIC mp_obj_t alarm_exit_and_deep_sleep_until_alarms(size_t n_args, const mp_obj_t *args) {
    validate_objs_are_alarms(n_args, args);

    int64_t connecting_delay_msec = CIRCUITPY_USB_CONNECTING_DELAY * 1024 - supervisor_ticks_ms64();
    if (connecting_delay_msec > 0) {
        common_hal_time_delay_ms(connecting_delay_msec * 1000 / 1024);
    }

    // If connected, wait for the program to be running at least as long as
    // CIRCUITPY_USB_CONNECTED_DEEP_SLEEP_DELAY. This allows a user to ctrl-C the running
    // program in case it is in a tight deep sleep loop that would otherwise be difficult
    // or impossible to interrupt.
    // Indicate that we're delaying with the SAFE_MODE color.
    int64_t delay_before_sleeping_msec =
        supervisor_ticks_ms64() - CIRCUITPY_USB_CONNECTED_DEEP_SLEEP_DELAY * 1000;
    if (supervisor_workflow_connecting() && delay_before_sleeping_msec > 0) {
        temp_status_color(SAFE_MODE);
        common_hal_time_delay_ms(delay_before_sleeping_msec);
        clear_temp_status();
    }

    common_hal_alarm_exit_and_deep_sleep_until_alarms(n_args, args);
    // Does not return.
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(alarm_exit_and_deep_sleep_until_alarms_obj, 1, MP_OBJ_FUN_ARGS_MAX, alarm_exit_and_deep_sleep_until_alarms);

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

    { MP_ROM_QSTR(MP_QSTR_TimeAlarm), MP_OBJ_FROM_PTR(&alarm_time_time_alarm_type) },
};

STATIC MP_DEFINE_CONST_DICT(alarm_time_globals, alarm_time_globals_table);

STATIC const mp_obj_module_t alarm_time_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&alarm_time_globals,
};

STATIC mp_map_elem_t alarm_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_alarm) },

    // wake_alarm is a mutable attribute.
    { MP_ROM_QSTR(MP_QSTR_wake_alarm), mp_const_none },

    { MP_ROM_QSTR(MP_QSTR_sleep_until_alarms), MP_OBJ_FROM_PTR(&alarm_sleep_until_alarms_obj) },
    { MP_ROM_QSTR(MP_QSTR_exit_and_deep_sleep_until_alarms),
                                               MP_OBJ_FROM_PTR(&alarm_exit_and_deep_sleep_until_alarms_obj) },

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
