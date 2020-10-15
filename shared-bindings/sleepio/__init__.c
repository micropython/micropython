/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Scott Shawcroft
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

#include "shared-bindings/alarm/__init__.h"

//| """Light and deep sleep used to save power
//|
//| The `sleepio` module provides sleep related functionality. There are two supported levels of
//| sleep, light and deep.
//|
//| Light sleep leaves the CPU and RAM powered so that CircuitPython can resume where it left off
//| after being woken up. Light sleep is automatically done by CircuitPython when `time.sleep()` is
//| called. To light sleep until a non-time alarm use `sleepio.sleep_until_alarm()`. Any active
//| peripherals, such as I2C, are left on.
//|
//| Deep sleep shuts down power to nearly all of the chip including the CPU and RAM. This can save
//| a more significant amount of power at the cost of starting CircuitPython from scratch when woken
//| up. CircuitPython will enter deep sleep automatically when code exits without error. If an
//| error causes CircuitPython to exit, error LED error flashes will be done periodically. To set
//| alarms for deep sleep use `sleepio.set_alarms` they will apply to next deep sleep only."""
//|

//| wake_alarm: Alarm
//| """The most recent alarm to wake us up from a sleep (light or deep.)"""
//|

//| def sleep_until_alarm(alarm: Alarm, ...) -> Alarm:
//|     """Performs a light sleep until woken by one of the alarms. The alarm that woke us up is
//|        returned."""
//|     ...
//|

STATIC mp_obj_t sleepio_sleep_until_alarm(size_t n_args, const mp_obj_t *args) {
    // mp_int_t size = MP_OBJ_SMALL_INT_VALUE(struct_calcsize(args[0]));
    // vstr_t vstr;
    // vstr_init_len(&vstr, size);
    // byte *p = (byte*)vstr.buf;
    // memset(p, 0, size);
    // byte *end_p = &p[size];
    // shared_modules_struct_pack_into(args[0], p, end_p, n_args - 1, &args[1]);
    // return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(sleepio_sleep_until_alarm_obj, 1, MP_OBJ_FUN_ARGS_MAX, sleepio_sleep_until_alarm);

//| def set_alarms(alarm: Alarm, ...) -> None:
//|     """Set one or more alarms to wake up from a deep sleep. The last alarm to wake us up is
//|        available as `wake_alarm`."""
//|     ...
//|
STATIC mp_obj_t sleepio_set_alarms(size_t n_args, const mp_obj_t *args) {
    // mp_int_t size = MP_OBJ_SMALL_INT_VALUE(struct_calcsize(args[0]));
    // vstr_t vstr;
    // vstr_init_len(&vstr, size);
    // byte *p = (byte*)vstr.buf;
    // memset(p, 0, size);
    // byte *end_p = &p[size];
    // shared_modules_struct_pack_into(args[0], p, end_p, n_args - 1, &args[1]);
    // return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(sleepio_set_alarms_obj, 1, MP_OBJ_FUN_ARGS_MAX, sleepio_set_alarms);


mp_map_elem_t sleepio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_sleepio) },

    { MP_ROM_QSTR(MP_QSTR_wake_alarm), mp_const_none },

    { MP_ROM_QSTR(MP_QSTR_sleep_until_alarm), sleepio_sleep_until_alarm_obj },
    { MP_ROM_QSTR(MP_QSTR_set_alarms), sleepio_set_alarms_obj },
};
STATIC MP_DEFINE_CONST_DICT(sleepio_module_globals, sleepio_module_globals_table);

void common_hal_sleepio_set_wake_alarm(mp_obj_t alarm) {
    // sleepio_module_globals_table[1].value = alarm;
}

const mp_obj_module_t sleepio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&sleepio_module_globals,
};
