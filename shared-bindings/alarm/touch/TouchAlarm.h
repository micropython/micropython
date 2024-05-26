// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/runtime.h"

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/alarm/touch/TouchAlarm.h"

extern const mp_obj_type_t alarm_touch_touchalarm_type;

extern void common_hal_alarm_touch_touchalarm_construct(alarm_touch_touchalarm_obj_t *self, const mcu_pin_obj_t *pin);
