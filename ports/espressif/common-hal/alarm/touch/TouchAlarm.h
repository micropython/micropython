// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "common-hal/microcontroller/Pin.h"

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *pin;
} alarm_touch_touchalarm_obj_t;

mp_obj_t alarm_touch_touchalarm_find_triggered_alarm(const size_t n_alarms, const mp_obj_t *alarms);
mp_obj_t alarm_touch_touchalarm_record_wake_alarm(void);

void alarm_touch_touchalarm_prepare_for_deep_sleep(void);
void alarm_touch_touchalarm_reset(void);
void alarm_touch_touchalarm_set_alarm(const bool deep_sleep, const size_t n_alarms, const mp_obj_t *alarms);
bool alarm_touch_touchalarm_woke_this_cycle(void);
