/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Lucian Copeland for Adafruit Industries
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

#include "py/runtime.h"

#include "shared-bindings/alarm/time/TimeAlarm.h"
#include "shared-bindings/time/__init__.h"
#include "supervisor/port.h"
#include "peripherals/rtc.h"

#include STM32_HAL_H

STATIC volatile bool woke_up;
STATIC uint32_t deep_sleep_ticks;

void common_hal_alarm_time_timealarm_construct(alarm_time_timealarm_obj_t *self, mp_float_t monotonic_time) {
    self->monotonic_time = monotonic_time;
}

mp_float_t common_hal_alarm_time_timealarm_get_monotonic_time(alarm_time_timealarm_obj_t *self) {
    return self->monotonic_time;
}

mp_obj_t alarm_time_timealarm_find_triggered_alarm(size_t n_alarms, const mp_obj_t *alarms) {
    // First, check to see if we match
    for (size_t i = 0; i < n_alarms; i++) {
        if (mp_obj_is_type(alarms[i], &alarm_time_timealarm_type)) {
            return alarms[i];
        }
    }
    return mp_const_none;
}

mp_obj_t alarm_time_timealarm_create_wakeup_alarm(void) {
    alarm_time_timealarm_obj_t *timer = m_new_obj(alarm_time_timealarm_obj_t);
    timer->base.type = &alarm_time_timealarm_type;
    // TODO: Set monotonic_time based on the RTC state.
    timer->monotonic_time = 0.0f;
    return timer;
}

// This is run in the timer task. We use it to wake the main CircuitPython task.
STATIC void timer_callback(void) {
    woke_up = true;
}

bool alarm_time_timealarm_woke_this_cycle(void) {
    return woke_up;
}

void alarm_time_timealarm_reset(void) {
    woke_up = false;
}

void alarm_time_timealarm_set_alarms(bool deep_sleep, size_t n_alarms, const mp_obj_t *alarms) {
    // Search through alarms for TimeAlarm instances, and check that there's only one
    bool timealarm_set = false;
    alarm_time_timealarm_obj_t *timealarm = MP_OBJ_NULL;
    for (size_t i = 0; i < n_alarms; i++) {
        if (!mp_obj_is_type(alarms[i], &alarm_time_timealarm_type)) {
            continue;
        }
        if (timealarm_set) {
            mp_raise_ValueError(translate("Only one alarm.time alarm can be set."));
        }
        timealarm = MP_OBJ_TO_PTR(alarms[i]);
        timealarm_set = true;
    }
    if (!timealarm_set) {
        return;
    }

    // Compute how long to actually sleep, considering the time now.
    mp_float_t now_secs = uint64_to_float(common_hal_time_monotonic_ms()) / 1000.0f;
    uint32_t wakeup_in_secs = MAX(0.0f, timealarm->monotonic_time - now_secs);
    uint32_t wakeup_in_ticks = wakeup_in_secs * 1024;

    // In the deep sleep case, we can't start the timer until the USB delay has finished
    if (deep_sleep) {
        deep_sleep_ticks = wakeup_in_ticks;
    } else {
        deep_sleep_ticks = 0;
    }
    // Use alarm B, since port reserves A
    // If true deep sleep is called, it will either ignore or overwrite this depending on
    // whether it is shorter or longer than the USB delay
    stm32_peripherals_rtc_assign_alarm_callback(PERIPHERALS_ALARM_B,timer_callback);
    stm32_peripherals_rtc_set_alarm(PERIPHERALS_ALARM_B,wakeup_in_ticks);
}

void alarm_time_timealarm_prepare_for_deep_sleep(void) {
    if (deep_sleep_ticks) {
        // This is used for both fake and real deep sleep, so it still needs the callback
        stm32_peripherals_rtc_assign_alarm_callback(PERIPHERALS_ALARM_B,timer_callback);
        stm32_peripherals_rtc_set_alarm(PERIPHERALS_ALARM_B,deep_sleep_ticks);
        deep_sleep_ticks = 0;
    }
}
