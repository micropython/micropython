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

#include "lib/timeutils/timeutils.h"

#include "hardware/gpio.h"
#include "hardware/rtc.h"

STATIC bool woke_up = false;
STATIC bool _timealarm_set = false;

void timer_callback(void) {
    woke_up = true;
}

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

mp_obj_t alarm_time_timealarm_create_wakeup_alarm(void) {
    alarm_time_timealarm_obj_t *timer = m_new_obj(alarm_time_timealarm_obj_t);
    timer->base.type = &alarm_time_timealarm_type;
    // TODO: Set monotonic_time based on the RTC state.
    timer->monotonic_time = 0.0f;
    return timer;
}

bool alarm_time_timealarm_woke_this_cycle(void) {
    return woke_up;
}

void alarm_time_timealarm_reset(void) {
    rtc_disable_alarm();
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
            mp_raise_ValueError(translate("Only one alarm.time alarm can be set."));
        }
        timealarm = MP_OBJ_TO_PTR(alarms[i]);
        timealarm_set = true;
    }
    if (!timealarm_set) {
        return;
    }
    if (deep_sleep) {
        _timealarm_set = true;
    }

    // Compute how long to actually sleep, considering the time now.
    mp_float_t mono_seconds_to_date = uint64_to_float(common_hal_time_monotonic_ms()) / 1000.0f;
    mp_float_t wakeup_in_secs = MAX(0.0f, timealarm->monotonic_time - mono_seconds_to_date);
    datetime_t t;

    rtc_get_datetime(&t);

    uint32_t rtc_seconds_to_date = timeutils_seconds_since_2000(t.year, t.month,
        t.day, t.hour, t.min, t.sec);

    // The float value is always slightly under, so add 1 to compensate
    uint32_t alarm_seconds = rtc_seconds_to_date + (uint32_t)wakeup_in_secs + 1;
    timeutils_struct_time_t tm;
    timeutils_seconds_since_2000_to_struct_time(alarm_seconds, &tm);

    // reuse t
    t.hour = tm.tm_hour;
    t.min = tm.tm_min;
    t.sec = tm.tm_sec;
    t.day = tm.tm_mday;
    t.month = tm.tm_mon;
    t.year = tm.tm_year;
    t.dotw = (tm.tm_wday + 1) % 7;

    rtc_set_alarm(&t, &timer_callback);

    woke_up = false;
}

bool alarm_time_timealarm_is_set(void) {
    return _timealarm_set;
}
