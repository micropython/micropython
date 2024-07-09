// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"

#include "shared-bindings/alarm/touch/TouchAlarm.h"
#include "shared-bindings/microcontroller/__init__.h"

// static volatile bool woke_up = false;

void common_hal_alarm_touch_touchalarm_construct(alarm_touch_touchalarm_obj_t *self, const mcu_pin_obj_t *pin) {
    mp_raise_NotImplementedError(NULL);
    (void)pin;
}

mp_obj_t alarm_touch_touchalarm_find_triggered_alarm(const size_t n_alarms, const mp_obj_t *alarms) {
    return mp_const_none;
}

mp_obj_t alarm_touch_touchalarm_record_wake_alarm(void) {
    return mp_const_none;
}

void alarm_touch_touchalarm_set_alarm(const bool deep_sleep, const size_t n_alarms, const mp_obj_t *alarms) {
}

void alarm_touch_touchalarm_prepare_for_deep_sleep(void) {
}

bool alarm_touch_touchalarm_woke_this_cycle(void) {
    return false;
}

void alarm_touch_touchalarm_reset(void) {
}
