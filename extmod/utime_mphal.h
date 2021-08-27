// Copyright (c) 2016 Paul Sokolovsky
// SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
// SPDX-FileCopyrightText: Copyright (c) 2013-2016 Damien P. George
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_EXTMOD_UTIME_MPHAL_H
#define MICROPY_INCLUDED_EXTMOD_UTIME_MPHAL_H

#include "py/obj.h"

MP_DECLARE_CONST_FUN_OBJ_1(mp_utime_sleep_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_utime_sleep_ms_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_utime_sleep_us_obj);
MP_DECLARE_CONST_FUN_OBJ_0(mp_utime_ticks_ms_obj);
MP_DECLARE_CONST_FUN_OBJ_0(mp_utime_ticks_us_obj);
MP_DECLARE_CONST_FUN_OBJ_0(mp_utime_ticks_cpu_obj);
MP_DECLARE_CONST_FUN_OBJ_2(mp_utime_ticks_diff_obj);
MP_DECLARE_CONST_FUN_OBJ_2(mp_utime_ticks_add_obj);
MP_DECLARE_CONST_FUN_OBJ_0(mp_utime_time_ns_obj);

#endif // MICROPY_INCLUDED_EXTMOD_UTIME_MPHAL_H
