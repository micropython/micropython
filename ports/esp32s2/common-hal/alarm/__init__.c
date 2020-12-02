 /*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2019 Lucian Copeland for Adafruit Industries
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

#include "py/obj.h"
#include "py/objtuple.h"
#include "py/runtime.h"

#include "shared-bindings/alarm/pin/PinAlarm.h"
#include "shared-bindings/alarm/time/TimeAlarm.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/time/__init__.h"
#include "shared-bindings/wifi/__init__.h"

#include "common-hal/alarm/__init__.h"

#include "esp_log.h"
#include "esp_sleep.h"

STATIC mp_obj_tuple_t *_deep_sleep_alarms;

void alarm_reset(void) {
    _deep_sleep_alarms = mp_const_empty_tuple;
}

void common_hal_alarm_disable_all(void) {
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
}

mp_obj_t common_hal_alarm_get_wake_alarm(void) {
    switch (esp_sleep_get_wakeup_cause()) {
        case ESP_SLEEP_WAKEUP_TIMER: {
            // Wake up from timer.
            alarm_time_time_alarm_obj_t *timer = m_new_obj(alarm_time_time_alarm_obj_t);
            timer->base.type = &alarm_time_time_alarm_type;
            return timer;
        }

        case ESP_SLEEP_WAKEUP_EXT0: {
            // Wake up from GPIO
            alarm_pin_pin_alarm_obj_t *ext0 = m_new_obj(alarm_pin_pin_alarm_obj_t);
            ext0->base.type = &alarm_pin_pin_alarm_type;
            return ext0;
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

// Set up light sleep or deep sleep alarms.
STATIC void setup_sleep_alarms(size_t n_alarms, const mp_obj_t *alarms) {
    bool time_alarm_set = false;
    alarm_time_time_alarm_obj_t *time_alarm = MP_OBJ_NULL;

    for (size_t i = 0; i < n_alarms; i++) {
        if (MP_OBJ_IS_TYPE(alarms[i], &alarm_pin_pin_alarm_type)) {
            mp_raise_NotImplementedError(translate("PinAlarm not yet implemented"));
        }
        else if (MP_OBJ_IS_TYPE(alarms[i], &alarm_time_time_alarm_type)) {
            if (time_alarm_set) {
                mp_raise_ValueError(translate("Only one alarm.time alarm can be set."));
            }
            time_alarm  = MP_OBJ_TO_PTR(alarms[i]);
            time_alarm_set = true;
        }
    }

    if (time_alarm != MP_OBJ_NULL) {
        // Compute how long to actually sleep, considering the time now.
        mp_float_t now_secs = uint64_to_float(common_hal_time_monotonic_ms()) / 1000.0f;
        mp_float_t wakeup_in_secs = MAX(0.0f, time_alarm->monotonic_time - now_secs);
        const uint64_t sleep_for_us = (uint64_t) (wakeup_in_secs * 1000000);
        ESP_LOGI("ALARM", "will sleep for us: %lld", sleep_for_us);
        esp_sleep_enable_timer_wakeup(sleep_for_us);
    }
}

mp_obj_t common_hal_alarm_wait_until_alarms(size_t n_alarms, const mp_obj_t *alarms) {
    if (n_alarms == 0) {
        return mp_const_none;
    }

    bool time_alarm_set = false;
    alarm_time_time_alarm_obj_t *time_alarm = MP_OBJ_NULL;

    for (size_t i = 0; i < n_alarms; i++) {
        if (MP_OBJ_IS_TYPE(alarms[i], &alarm_pin_pin_alarm_type)) {
            mp_raise_NotImplementedError(translate("PinAlarm not yet implemented"));
        }
        else if (MP_OBJ_IS_TYPE(alarms[i], &alarm_time_time_alarm_type)) {
            if (time_alarm_set) {
                mp_raise_ValueError(translate("Only one alarm.time alarm can be set."));
            }
            time_alarm  = MP_OBJ_TO_PTR(alarms[i]);
            time_alarm_set = true;
        }
    }

    ESP_LOGI("ALARM", "waiting for alarms");

    if (time_alarm_set && n_alarms == 1) {
        // If we're only checking time, avoid a polling loop, so maybe we can save some power.
        const mp_float_t now_secs = uint64_to_float(common_hal_time_monotonic_ms()) / 1000.0f;
        const mp_float_t wakeup_in_secs = MAX(0.0f, time_alarm->monotonic_time - now_secs);
        const uint32_t delay_ms = (uint32_t) (wakeup_in_secs * 1000.0f);
        ESP_LOGI("ALARM", "Delay for ms: %d", delay_ms);
        common_hal_time_delay_ms((uint32_t) delay_ms);
    } else {
        // Poll for alarms.
        while (true) {
            RUN_BACKGROUND_TASKS;
            // Allow ctrl-C interrupt.
            if (mp_hal_is_interrupted()) {
                return mp_const_none;
            }

            // TODO: Check PinAlarms.

            if (time_alarm != MP_OBJ_NULL &&
                common_hal_time_monotonic_ms() * 1000.f >= time_alarm->monotonic_time) {
                return time_alarm;
            }
        }
    }

    return mp_const_none;
}

// Is it safe to do a light sleep? Check whether WiFi is on or there are
// other ongoing tasks that should not be shut down.
static bool light_sleep_ok(void) {
    return !common_hal_wifi_radio_get_enabled(&common_hal_wifi_radio_obj);
}

mp_obj_t common_hal_alarm_light_sleep_until_alarms(size_t n_alarms, const mp_obj_t *alarms) {
    if (n_alarms == 0) {
        return mp_const_none;
    }

    if (light_sleep_ok()) {
        ESP_LOGI("ALARM", "start light sleep");
        setup_sleep_alarms(n_alarms, alarms);
        esp_light_sleep_start();
        return common_hal_alarm_get_wake_alarm();
    } else {
        // Don't do an ESP32 light sleep.
        return common_hal_alarm_wait_until_alarms(n_alarms, alarms);
    }
}

void common_hal_alarm_exit_and_deep_sleep_until_alarms(size_t n_alarms, const mp_obj_t *alarms) {
    setup_sleep_alarms(n_alarms, alarms);

    // Raise an exception, which will be processed in main.c.
    mp_raise_arg1(&mp_type_DeepSleepRequest, NULL);
}

void common_hal_alarm_prepare_for_deep_sleep(void) {
    // Turn off WiFi and anything else that should be shut down cleanly.
    common_hal_wifi_radio_set_enabled(&common_hal_wifi_radio_obj, false);
}

void NORETURN common_hal_alarm_enter_deep_sleep(void) {
    ESP_LOGI("ALARM", "start deep sleep");
    esp_deep_sleep_start();
}
