/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
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

#include "py/gc.h"
#include "py/obj.h"
#include "py/objtuple.h"
#include "py/runtime.h"

#include "shared-bindings/alarm/__init__.h"
#include "shared-bindings/alarm/pin/PinAlarm.h"
#include "shared-bindings/alarm/SleepMemory.h"
#include "shared-bindings/alarm/time/TimeAlarm.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/wifi/__init__.h"

#include "supervisor/port.h"
#include "supervisor/shared/workflow.h"

#include "esp_sleep.h"

#include "components/soc/soc/esp32s2/include/soc/rtc_cntl_reg.h"
#include "components/driver/include/driver/uart.h"

// Singleton instance of SleepMemory.
const alarm_sleep_memory_obj_t alarm_sleep_memory_obj = {
    .base = {
        .type = &alarm_sleep_memory_type,
    },
};

void alarm_reset(void) {
    alarm_time_timealarm_reset();
    alarm_pin_pinalarm_reset();
    alarm_sleep_memory_reset();
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
}

STATIC esp_sleep_wakeup_cause_t _get_wakeup_cause(void) {
    if (alarm_time_timealarm_woke_us_up()) {
        return ESP_SLEEP_WAKEUP_TIMER;
    }
    if (alarm_pin_pinalarm_woke_us_up()) {
        return ESP_SLEEP_WAKEUP_GPIO;
    }

    return esp_sleep_get_wakeup_cause();
}

bool alarm_woken_from_sleep(void) {
    return _get_wakeup_cause() != ESP_SLEEP_WAKEUP_UNDEFINED;
}

STATIC mp_obj_t _get_wake_alarm(size_t n_alarms, const mp_obj_t *alarms) {
    esp_sleep_wakeup_cause_t cause = _get_wakeup_cause();
    switch (cause) {
        case ESP_SLEEP_WAKEUP_TIMER: {
            return alarm_time_timealarm_get_wakeup_alarm(n_alarms, alarms);
        }

        case ESP_SLEEP_WAKEUP_GPIO:
        case ESP_SLEEP_WAKEUP_EXT0:
        case ESP_SLEEP_WAKEUP_EXT1: {
            return alarm_pin_pinalarm_get_wakeup_alarm(n_alarms, alarms);
        }

        case ESP_SLEEP_WAKEUP_TOUCHPAD:
            // TODO: implement TouchIO
            // Wake up from touch on pad, esp_sleep_get_touchpad_wakeup_status()
            break;

        case ESP_SLEEP_WAKEUP_UNDEFINED:
        default:
            // Not a deep sleep reset.
            break;
    }
    return mp_const_none;
}

mp_obj_t common_hal_alarm_get_wake_alarm(void) {
    return _get_wake_alarm(0, NULL);
}

// Set up light sleep or deep sleep alarms.
STATIC void _setup_sleep_alarms(bool deep_sleep, size_t n_alarms, const mp_obj_t *alarms) {
    alarm_pin_pinalarm_set_alarms(deep_sleep, n_alarms, alarms);
    alarm_time_timealarm_set_alarms(deep_sleep, n_alarms, alarms);
}

STATIC void _idle_until_alarm(void) {
    // Poll for alarms.
    while (!mp_hal_is_interrupted()) {
        RUN_BACKGROUND_TASKS;
        // Allow ctrl-C interrupt.
        if (alarm_woken_from_sleep()) {
            return;
        }

        port_idle_until_interrupt();
    }
}

mp_obj_t common_hal_alarm_light_sleep_until_alarms(size_t n_alarms, const mp_obj_t *alarms) {
    _setup_sleep_alarms(false, n_alarms, alarms);

    // We cannot esp_light_sleep_start() here because it shuts down all non-RTC peripherals.
    _idle_until_alarm();

    if (mp_hal_is_interrupted()) {
        return mp_const_none; // Shouldn't be given to python code because exception handling should kick in.
    }

    mp_obj_t wake_alarm = _get_wake_alarm(n_alarms, alarms);
    alarm_reset();
    return wake_alarm;
}

void common_hal_alarm_set_deep_sleep_alarms(size_t n_alarms, const mp_obj_t *alarms) {
    _setup_sleep_alarms(true, n_alarms, alarms);
}

void NORETURN alarm_enter_deep_sleep(void) {
    alarm_pin_pinalarm_prepare_for_deep_sleep();
    // The ESP-IDF caches the deep sleep settings and applies them before sleep.
    // We don't need to worry about resetting them in the interim.
    esp_deep_sleep_start();
}

void common_hal_alarm_gc_collect(void) {
    gc_collect_ptr(alarm_get_wake_alarm());
}
