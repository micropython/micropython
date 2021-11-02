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
// #include <stdio.h>

#include "shared-bindings/alarm/time/TimeAlarm.h"
#include "shared-bindings/time/__init__.h"
#include "common-hal/alarm/__init__.h"
#include "supervisor/port.h"

STATIC volatile bool woke_up;
STATIC uint32_t deep_sleep_ticks;
// TODO: replace timealarm_on with SAMD_ALARM_FLAG bit flags
STATIC volatile bool timealarm_on;

void common_hal_alarm_time_timealarm_construct(alarm_time_timealarm_obj_t *self, mp_float_t monotonic_time) {
    // TODO: throw a ValueError if the input time exceeds the maximum
    //       value of the Compare register. This must be calculated from the
    //       setup values in port.c. Should be ~3 days. Give it some margin.
    //
    //       UPDATE: for deep sleep at least, it's far more than 3 days since
    //               prescalar is set to 1024. (2^32)/32 seconds so >1500 days?

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

mp_obj_t alarm_time_timealarm_create_wakeup_alarm(void) {
    alarm_time_timealarm_obj_t *timer = m_new_obj(alarm_time_timealarm_obj_t);
    timer->base.type = &alarm_time_timealarm_type;
    // TODO: Set monotonic_time based on the RTC state.
    //       Or don't, most of the other ports don't have this either.
    timer->monotonic_time = 0.0f;
    return timer;
}

void time_alarm_callback(void) {
    if (timealarm_on) {
        RTC->MODE0.INTENCLR.reg = RTC_MODE0_INTENCLR_CMP1; // clear flags
        woke_up = true;
        timealarm_on = false;
    }
}

bool alarm_time_timealarm_woke_this_cycle(void) {
    if (timealarm_on && (((uint32_t)port_get_raw_ticks(NULL) << 4) > RTC->MODE0.COMP[1].reg)) {
        woke_up = true;
    }
    return woke_up;
}

void alarm_time_timealarm_reset(void) {
    timealarm_on = false;
    woke_up = false;
    SAMD_ALARM_FLAG &= ~SAMD_ALARM_FLAG_TIME; // clear flag
}

void alarm_time_timealarm_set_alarms(bool deep_sleep, size_t n_alarms, const mp_obj_t *alarms) {
    // Turn on debug control
    // RTC->MODE0.DBGCTRL.bit.DBGRUN = 1;
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
    // (otherwise it will go off while USB enumerates, and we'll risk entering deep sleep
    // without any way to wake up again)
    if (deep_sleep) {
        deep_sleep_ticks = wakeup_in_ticks;
    } else {
        deep_sleep_ticks = 0;
    }
    timealarm_on = true;
    // Set COMP1 for fake sleep. This will be read and reset for real deep sleep anyways.
    // RTC->MODE0.COMP[1].reg = wakeup_in_ticks;
    RTC->MODE0.COMP[1].reg = ((uint32_t)port_get_raw_ticks(NULL) + wakeup_in_ticks) << 4;
    while ((RTC->MODE0.SYNCBUSY.reg & (RTC_MODE0_SYNCBUSY_COMP1)) != 0) {
    }
    RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_CMP1;
    RTC->MODE0.INTENSET.reg = RTC_MODE0_INTENSET_CMP1;
    SAMD_ALARM_FLAG |= SAMD_ALARM_FLAG_TIME; // set TimeAlarm flag

    // This is set for fake sleep. Max fake sleep time is ~72 hours
    // True deep sleep isn't limited by this
    // port_interrupt_after_ticks(wakeup_in_ticks);
    // printf("second t %lu, cmp0 %lu, cmp1 %lu\n", (uint32_t)port_get_raw_ticks(NULL),RTC->MODE0.COMP[0].reg,RTC->MODE0.COMP[1].reg);
    // TODO: set up RTC->COMP[1] and create a callback pointing to
    //       time_alarm_callback. See atmel-samd/supervisor/port.c -> _port_interrupt_after_ticks()
    //       for how to set this up. I don't know how you do the callback, though. You MUST use
    //       COMP[1], since port.c uses COMP[0] already and needs to set that for
    //       things like the USB enumeration delay.

    // If true deep sleep is called, it will either ignore or overwrite the above setup depending on
    // whether it is shorter or longer than the USB delay
    // printf("set deep alarm finished\n");

}

void alarm_time_timealarm_prepare_for_deep_sleep(void) {
    if (deep_sleep_ticks) {
        // TODO: set up RTC->COMP[1] again, since it needs to start AFTER the USB enumeration delay.
        //       Just do the exact same setup as alarm_time_timealarm_set_alarms(). Note, this
        //       is used for both fake and real deep sleep, so it still needs the callback.
        //       See STM32 for reference.

        RTC->MODE0.COMP[1].reg = deep_sleep_ticks;
        while ((RTC->MODE0.SYNCBUSY.reg & (RTC_MODE0_SYNCBUSY_COMP1)) != 0) {
        }
        deep_sleep_ticks = 0;
    }
}
