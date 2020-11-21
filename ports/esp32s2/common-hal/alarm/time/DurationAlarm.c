/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 @microDev1 (GitHub)
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

#include "esp_sleep.h"

#include "py/runtime.h"

#include "shared-bindings/alarm/time/DurationAlarm.h"

void common_hal_alarm_time_duration_alarm_construct(alarm_time_duration_alarm_obj_t *self, mp_float_t duration) {
    self->duration = duration;
}

mp_float_t common_hal_alarm_time_duration_alarm_get_duration(alarm_time_duration_alarm_obj_t *self) {
    return self->duration;
}

void common_hal_alarm_time_duration_alarm_enable(alarm_time_duration_alarm_obj_t *self) {
    if (esp_sleep_enable_timer_wakeup((uint64_t) (self->duration * 1000000)) == ESP_ERR_INVALID_ARG) {
        mp_raise_ValueError(translate("duration out of range"));
    }
}

void common_hal_alarm_time_duration_alarm_disable (alarm_time_duration_alarm_obj_t *self) {
    (void) self;
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
}
