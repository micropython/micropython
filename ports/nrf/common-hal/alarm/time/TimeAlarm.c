/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Dan Halbert for Adafruit Industries
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

//#include "esp_sleep.h"

#include "py/runtime.h"
//#include "supervisor/esp_port.h"
#include <stdio.h>

#include "shared-bindings/alarm/time/TimeAlarm.h"
#include "shared-bindings/time/__init__.h"

void common_hal_alarm_time_timealarm_construct(alarm_time_timealarm_obj_t *self, mp_float_t monotonic_time) {
    self->monotonic_time = monotonic_time;
}

mp_float_t common_hal_alarm_time_timealarm_get_monotonic_time(alarm_time_timealarm_obj_t *self) {
    return self->monotonic_time;
}

mp_obj_t alarm_time_timealarm_get_wakeup_alarm(size_t n_alarms, const mp_obj_t *alarms) {
    // First, check to see if we match
    for (size_t i = 0; i < n_alarms; i++) {
        if (MP_OBJ_IS_TYPE(alarms[i], &alarm_time_timealarm_type)) {
            return alarms[i];
        }
    }
    alarm_time_timealarm_obj_t *timer = m_new_obj(alarm_time_timealarm_obj_t);
    timer->base.type = &alarm_time_timealarm_type;
    // TODO: Set monotonic_time based on the RTC state.
    timer->monotonic_time = 0.0f;
    return timer;
}

extern volatile int rtc_woke_up_counter;
bool alarm_time_timealarm_woke_us_up(void) {
  return rtc_woke_up_counter;
}

extern void port_disable_interrupt_after_ticks_ch(uint32_t channel);
void alarm_time_timealarm_reset(void) {
    port_disable_interrupt_after_ticks_ch(1);
    rtc_woke_up_counter = 0;
}

extern void port_interrupt_after_ticks_ch(uint32_t channel, uint32_t ticks);//XXX in port.c

void alarm_time_timealarm_set_alarms(bool deep_sleep, size_t n_alarms, const mp_obj_t *alarms) {
    bool timealarm_set = false;
    alarm_time_timealarm_obj_t *timealarm = MP_OBJ_NULL;

    for (size_t i = 0; i < n_alarms; i++) {
        if (!MP_OBJ_IS_TYPE(alarms[i], &alarm_time_timealarm_type)) {
            continue;
        }
        if (timealarm_set) {
            mp_raise_ValueError(translate("Only one alarm.time alarm can be set."));
        }
        timealarm  = MP_OBJ_TO_PTR(alarms[i]);
        timealarm_set = true;
    }
    if (!timealarm_set) {
        return;
    }

    // Compute how long to actually sleep, considering the time now.
    mp_float_t now_secs = uint64_to_float(common_hal_time_monotonic_ms()) / 1000.0f;
    mp_float_t wakeup_in_secs = MAX(0.0f, timealarm->monotonic_time - now_secs);
    int wsecs = (int)(wakeup_in_secs);
    if (wsecs > 510) { //XXX
      mp_raise_ValueError(translate("Alarm time is too far."));
    }

    uint32_t wakeup_in_ticks = (uint32_t)(wakeup_in_secs * 1024.0f);
    //printf("alarm_time_timealarm_set_alarms() %d secs 0x%08X ticks\r\n", wsecs, (int)wakeup_in_ticks);
    port_interrupt_after_ticks_ch(1, wakeup_in_ticks);
    rtc_woke_up_counter = 0;
}
