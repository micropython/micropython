// SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
// SPDX-FileCopyrightText: Copyright (c) 2016 Damien P. George
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"
#include "py/mperrno.h"
#include "extmod/machine_pulse.h"

#if MICROPY_PY_MACHINE_PULSE

mp_uint_t machine_time_pulse_us(mp_hal_pin_obj_t pin, int pulse_level, mp_uint_t timeout_us) {
    mp_uint_t start = mp_hal_ticks_us();
    while (mp_hal_pin_read(pin) != pulse_level) {
        if ((mp_uint_t)(mp_hal_ticks_us() - start) >= timeout_us) {
            return (mp_uint_t)-2;
        }
    }
    start = mp_hal_ticks_us();
    while (mp_hal_pin_read(pin) == pulse_level) {
        if ((mp_uint_t)(mp_hal_ticks_us() - start) >= timeout_us) {
            return (mp_uint_t)-1;
        }
    }
    return mp_hal_ticks_us() - start;
}

STATIC mp_obj_t machine_time_pulse_us_(size_t n_args, const mp_obj_t *args) {
    mp_hal_pin_obj_t pin = mp_hal_get_pin_obj(args[0]);
    int level = 0;
    if (mp_obj_is_true(args[1])) {
        level = 1;
    }
    mp_uint_t timeout_us = 1000000;
    if (n_args > 2) {
        timeout_us = mp_obj_get_int(args[2]);
    }
    mp_uint_t us = machine_time_pulse_us(pin, level, timeout_us);
    // May return -1 or -2 in case of timeout
    return mp_obj_new_int(us);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_time_pulse_us_obj, 2, 3, machine_time_pulse_us_);

#endif
