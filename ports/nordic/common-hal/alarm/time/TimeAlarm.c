// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2021 Junji Sakai
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"
#include <stdio.h>

#include "shared-bindings/alarm/__init__.h"
#include "shared-bindings/alarm/time/TimeAlarm.h"
#include "shared-bindings/time/__init__.h"

void common_hal_alarm_time_timealarm_construct(alarm_time_timealarm_obj_t *self, mp_float_t monotonic_time) {
    self->monotonic_time = monotonic_time;
}

mp_float_t common_hal_alarm_time_timealarm_get_monotonic_time(alarm_time_timealarm_obj_t *self) {
    return self->monotonic_time;
}

mp_obj_t alarm_time_timealarm_find_triggered_alarm(size_t n_alarms, const mp_obj_t *alarms) {
    for (size_t i = 0; i < n_alarms; i++) {
        if (mp_obj_is_type(alarms[i], &alarm_time_timealarm_type)) {
            return alarms[i];
        }
    }
    return mp_const_none;
}

mp_obj_t alarm_time_timealarm_record_wake_alarm(void) {
    alarm_time_timealarm_obj_t *const alarm = &alarm_wake_alarm.time_alarm;

    alarm->base.type = &alarm_time_timealarm_type;
    // TODO: Set monotonic_time based on the RTC state.
    alarm->monotonic_time = 0.0f;
    return alarm;
}

bool alarm_time_timealarm_woke_this_cycle(void) {
    return sleepmem_wakeup_event == SLEEPMEM_WAKEUP_BY_TIMER;
}

int64_t wakeup_time_saved = 0;

int64_t alarm_time_timealarm_get_wakeup_timediff_ms(void) {
    if (wakeup_time_saved == 0) {
        return -1;
    }
    return wakeup_time_saved - common_hal_time_monotonic_ms();
}

void alarm_time_timealarm_reset(void) {
    port_disable_interrupt_after_ticks_ch(1);
    wakeup_time_saved = 0;
}

void alarm_time_timealarm_set_alarms(bool deep_sleep, size_t n_alarms, const mp_obj_t *alarms) {
    bool timealarm_set = false;
    alarm_time_timealarm_obj_t *timealarm = MP_OBJ_NULL;
    wakeup_time_saved = 0;

    for (size_t i = 0; i < n_alarms; i++) {
        if (!mp_obj_is_type(alarms[i], &alarm_time_timealarm_type)) {
            continue;
        }
        if (timealarm_set) {
            mp_raise_ValueError(MP_ERROR_TEXT("Only one alarm.time alarm can be set"));
        }
        timealarm = MP_OBJ_TO_PTR(alarms[i]);
        timealarm_set = true;
    }
    if (!timealarm_set) {
        return;
    }

    wakeup_time_saved = (int64_t)(timealarm->monotonic_time * 1000.0f);
}

void alarm_time_timealarm_prepare_for_deep_sleep(void) {
}
