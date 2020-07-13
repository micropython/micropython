// SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
// SPDX-FileCopyrightText: Copyright (c) 2016 Damien P. George
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_EXTMOD_MACHINE_PULSE_H
#define MICROPY_INCLUDED_EXTMOD_MACHINE_PULSE_H

#include "py/obj.h"
#include "py/mphal.h"

mp_uint_t machine_time_pulse_us(mp_hal_pin_obj_t pin, int pulse_level, mp_uint_t timeout_us);

MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(machine_time_pulse_us_obj);

#endif // MICROPY_INCLUDED_EXTMOD_MACHINE_PULSE_H
