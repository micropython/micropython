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
#include "hpl/pm/hpl_pm_base.h"

#include "shared-bindings/alarm/__init__.h"
#include "shared-bindings/alarm/time/TimeAlarm.h"
#include "shared-bindings/time/__init__.h"
#include "supervisor/port.h"

STATIC volatile bool woke_up = false;
STATIC mp_float_t wakeup_time;

void common_hal_alarm_time_timealarm_construct(alarm_time_timealarm_obj_t *self, mp_float_t monotonic_time) {
    // TODO: when issuing light/seep sleep, throw a ValueError if the
    //       time exceeds the maximum value.  For light sleep, max =
    //       2**32 / 16384 = 3 days.  For deep sleep, max = 2**32 / 32
    //       = 1550 days.
    self->monotonic_time = monotonic_time;
}

mp_float_t common_hal_alarm_time_timealarm_get_monotonic_time(alarm_time_timealarm_obj_t *self) {
    return self->monotonic_time;
}

mp_obj_t alarm_time_timealarm_find_triggered_alarm(size_t n_alarms, const mp_obj_t *alarms) {
    // TODO: this function currently assumes you can only have a single TimeAlarm
    //       If you want to support more, it will need to somehow detect which one went off.
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
    //       Or don't, most of the other ports don't have this either.
    alarm->monotonic_time = 0.0f;
    return alarm;
}

void time_alarm_callback(void) {
    if (SAMD_ALARM_FLAG_TIME_CHK) {
        RTC->MODE0.INTENCLR.reg = RTC_MODE0_INTENCLR_CMP1;
        // SAMD_ALARM_FLAG_TIME_CLR;
        woke_up = true;
    }
}

bool alarm_time_timealarm_woke_this_cycle(void) {
    if (SAMD_ALARM_FLAG_TIME_CHK) {
        mp_float_t now_secs = uint64_to_float(common_hal_time_monotonic_ms()) / 1000.0f;
        if (now_secs > wakeup_time) {
            woke_up = true;
        }
    }
    return woke_up;
}

void alarm_time_timealarm_reset(void) {
    SAMD_ALARM_FLAG_TIME_CLR;
    woke_up = false;
    RTC->MODE0.INTENCLR.reg = RTC_MODE0_INTENCLR_CMP1;
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
            mp_raise_ValueError(translate("Only one alarm.time alarm can be set"));
        }
        timealarm = MP_OBJ_TO_PTR(alarms[i]);
        timealarm_set = true;
    }
    if (!timealarm_set) {
        return;
    }

    // In the true deep sleep case, counter is set again based on
    // wakeup_time in alarm_time_timealarm_prepare_for_deep_sleep.
    wakeup_time = timealarm->monotonic_time;

    // Compute how long to actually sleep, considering the time now.
    // At least 1 count = 1/16384 sec is necessary.
    mp_float_t now_secs = uint64_to_float(common_hal_time_monotonic_ms()) / 1000.0f;
    uint32_t wakeup_in_counts = MAX(1, (uint32_t)((wakeup_time - now_secs) * 16384));


    SAMD_ALARM_FLAG_TIME_SET;
    RTC->MODE0.COMP[1].reg = RTC->MODE0.COUNT.reg + wakeup_in_counts;
    while ((RTC->MODE0.SYNCBUSY.reg & (RTC_MODE0_SYNCBUSY_COMP1)) != 0) {
    }
    RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_CMP1;
    RTC->MODE0.INTENSET.reg = RTC_MODE0_INTENSET_CMP1;
}

void alarm_time_timealarm_prepare_for_deep_sleep(void) {
    // set up RTC->COMP[1] again, since it needs to start AFTER the USB enumeration delay.
    // Just do the exact same setup as alarm_time_timealarm_set_alarms(). Note, this
    // is used for both fake and real deep sleep, so it still needs the callback.
    // See STM32 for reference.
    //
    // In deep sleep mode, prescaler is set to 1024, so that 1 count = 1/32 s.
    // At least 1 count is necessary.

    mp_float_t now_secs = uint64_to_float(common_hal_time_monotonic_ms()) / 1000.0f;
    uint32_t wakeup_in_counts = MAX(1, (uint32_t)((wakeup_time - now_secs) * 32));
    RTC->MODE0.COMP[1].reg = wakeup_in_counts;
    while ((RTC->MODE0.SYNCBUSY.reg & (RTC_MODE0_SYNCBUSY_COMP1)) != 0) {
    }
}
