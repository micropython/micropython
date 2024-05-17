// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    mp_float_t monotonic_time;      // values compatible with time.monotonic_time()
} alarm_time_timealarm_obj_t;

mp_obj_t alarm_time_timealarm_find_triggered_alarm(size_t n_alarms, const mp_obj_t *alarms);
mp_obj_t alarm_time_timealarm_record_wake_alarm(void);
void time_alarm_callback(void);
bool alarm_time_timealarm_woke_this_cycle(void);
void alarm_time_timealarm_set_alarms(bool deep_sleep, size_t n_alarms, const mp_obj_t *alarms);
void alarm_time_timealarm_reset(void);

void alarm_time_timealarm_prepare_for_deep_sleep(void);
