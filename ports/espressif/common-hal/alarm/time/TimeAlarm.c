// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "esp_sleep.h"

#include "py/runtime.h"
#include "supervisor/port.h"

#include "components/esp_timer/include/esp_timer.h"

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

esp_timer_handle_t pretend_sleep_timer;
static bool woke_up = false;

// This is run in the timer task. We use it to wake the main CircuitPython task.
static void timer_callback(void *arg) {
    (void)arg;
    woke_up = true;
    port_wake_main_task();
}

bool alarm_time_timealarm_woke_this_cycle(void) {
    return woke_up;
}

void alarm_time_timealarm_reset(void) {
    esp_timer_stop(pretend_sleep_timer);
    esp_timer_delete(pretend_sleep_timer);
    pretend_sleep_timer = NULL;
    woke_up = false;
}

void alarm_time_timealarm_set_alarms(bool deep_sleep, size_t n_alarms, const mp_obj_t *alarms) {
    bool timealarm_set = false;
    alarm_time_timealarm_obj_t *timealarm = MP_OBJ_NULL;

    for (size_t i = 0; i < n_alarms; i++) {
        if (!mp_obj_is_type(alarms[i], &alarm_time_timealarm_type)) {
            continue;
        }
        if (timealarm_set) {
            mp_raise_ValueError(MP_ERROR_TEXT("Only one alarm.time alarm can be set."));
        }
        timealarm = MP_OBJ_TO_PTR(alarms[i]);
        timealarm_set = true;
    }
    if (!timealarm_set) {
        return;
    }

    if (pretend_sleep_timer != NULL) {
        esp_timer_stop(pretend_sleep_timer);
    } else {
        // Configure the timer to use during pretend sleep.
        esp_timer_create_args_t args;
        args.callback = timer_callback;
        args.arg = NULL;
        args.dispatch_method = ESP_TIMER_TASK;
        args.name = "Pretend deep sleep";
        esp_timer_create(&args, &pretend_sleep_timer);
    }

    // Compute how long to actually sleep, considering the time now.
    mp_float_t now_secs = uint64_to_float(common_hal_time_monotonic_ms()) / 1000.0f;
    mp_float_t wakeup_in_secs = MAX(0.0f, timealarm->monotonic_time - now_secs);
    const uint64_t sleep_for_us = (uint64_t)(wakeup_in_secs * 1000000);
    esp_sleep_enable_timer_wakeup(sleep_for_us);

    // Also set the RTC interrupt so it can wake our task. This will be wiped out
    // if we actually deep sleep.
    woke_up = false;
    esp_timer_start_once(pretend_sleep_timer, sleep_for_us);
}
