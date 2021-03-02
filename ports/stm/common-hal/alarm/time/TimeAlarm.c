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

void common_hal_alarm_time_timealarm_construct(alarm_time_timealarm_obj_t *self, mp_float_t monotonic_time) {
    self->monotonic_time = monotonic_time;
}

mp_float_t common_hal_alarm_time_timealarm_get_monotonic_time(alarm_time_timealarm_obj_t *self) {
    return self->monotonic_time;
}

// mp_obj_t alarm_time_timealarm_get_wakeup_alarm(size_t n_alarms, const mp_obj_t *alarms) {
//     // First, check to see if we match
//     for (size_t i = 0; i < n_alarms; i++) {
//         if (MP_OBJ_IS_TYPE(alarms[i], &alarm_time_timealarm_type)) {
//             return alarms[i];
//         }
//     }
//     alarm_time_timealarm_obj_t *timer = m_new_obj(alarm_time_timealarm_obj_t);
//     timer->base.type = &alarm_time_timealarm_type;
//     // TODO: Set monotonic_time based on the RTC state.
//     timer->monotonic_time = 0.0f;
//     return timer;
// }

// This is run in the timer task. We use it to wake the main CircuitPython task.
// void timer_callback(void *arg) {
//     (void) arg;
//     woke_up = true;
//     xTaskNotifyGive(circuitpython_task);
// }

bool alarm_time_timealarm_woke_us_up(void) {
    // return woke_up;
    return false;
}

// void alarm_time_timealarm_reset(void) {
//     esp_timer_stop(pretend_sleep_timer);
//     esp_timer_delete(pretend_sleep_timer);
//     pretend_sleep_timer = NULL;
//     woke_up = false;
// }

void alarm_time_timealarm_set_alarms(bool deep_sleep, size_t n_alarms, const mp_obj_t *alarms) {
    // HAL_RTCEx_SetWakeUpTimer_IT(&RTCHandle, 0xA017, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
    // HAL_RTCEx_SetWakeUpTimer_IT(&_hrtc, rtc_clock_frequency / 1024 / 2, RTC_WAKEUPCLOCK_RTCCLK_DIV2);
    // HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 1, 0U);
    // HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
}
