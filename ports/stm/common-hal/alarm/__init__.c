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

#include "py/gc.h"
#include "py/obj.h"
#include "py/objtuple.h"
#include "py/runtime.h"
#include "lib/utils/interrupt_char.h"

#include "shared-bindings/alarm/__init__.h"
#include "shared-bindings/alarm/pin/PinAlarm.h"
#include "shared-bindings/alarm/time/TimeAlarm.h"

#include "shared-bindings/microcontroller/__init__.h"

#include "supervisor/port.h"
#include "supervisor/shared/workflow.h"

#define STM_WAKEUP_UNDEF    0
#define STM_WAKEUP_GPIO     1
#define STM_WAKEUP_RTC      2

#define STM_ALARM_FLAG      RTC->BKP0R

void common_hal_alarm_reset(void) {
    // Reset the alarm flag
    STM_ALARM_FLAG = 0x00;
    // alarm_sleep_memory_reset();
    alarm_pin_pinalarm_reset();
    alarm_time_timealarm_reset();
    // esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
}

STATIC uint8_t _get_wakeup_cause(void) {
    // If in light/fake sleep, check modules
    if (alarm_pin_pinalarm_woke_us_up()) {
        return STM_WAKEUP_GPIO;
    }
    if (alarm_time_timealarm_woke_us_up()) {
        return STM_WAKEUP_RTC;
    }
    if (RTC->BKP0R & 0x01) {
        // We've woken from deep sleep. Was it the WKUP pin or the RTC?
        if (RTC->ISR & RTC_FLAG_ALRBF) {
            // Alarm B is the deep sleep alarm
            return STM_WAKEUP_RTC;
        } else {
            return STM_WAKEUP_GPIO;
        }
    }
    return STM_WAKEUP_UNDEF;
}

bool common_hal_alarm_woken_from_sleep(void) {
    return _get_wakeup_cause() != STM_WAKEUP_UNDEF;
}

STATIC mp_obj_t _get_wake_alarm(size_t n_alarms, const mp_obj_t *alarms) {
    uint8_t cause = _get_wakeup_cause();
    switch (cause) {
        case STM_WAKEUP_RTC: {
            return alarm_time_timealarm_get_wakeup_alarm(n_alarms, alarms);
        }
        case STM_WAKEUP_GPIO: {
            return alarm_pin_pinalarm_get_wakeup_alarm(n_alarms, alarms);
        }
        case STM_WAKEUP_UNDEF:
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
        if (common_hal_alarm_woken_from_sleep()) {
            shared_alarm_save_wake_alarm();
            return;
        }
        port_idle_until_interrupt();
    }
}

mp_obj_t common_hal_alarm_light_sleep_until_alarms(size_t n_alarms, const mp_obj_t *alarms) {
    // If USB is active, only pretend to sleep. Otherwise, light sleep
    if (supervisor_workflow_active()) {
        _setup_sleep_alarms(false, n_alarms, alarms);
        _idle_until_alarm();
    } else {
        _setup_sleep_alarms(false, n_alarms, alarms);
        port_disable_tick();
        HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
        port_enable_tick();
    }

    mp_obj_t wake_alarm = _get_wake_alarm(n_alarms, alarms);
    common_hal_alarm_reset();
    return wake_alarm;
}

void common_hal_alarm_set_deep_sleep_alarms(size_t n_alarms, const mp_obj_t *alarms) {
    _setup_sleep_alarms(true, n_alarms, alarms);
}

//#define NORETURN __attribute__((noreturn))
void NORETURN common_hal_alarm_enter_deep_sleep(void) {
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,1);
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,0);
    alarm_pin_pinalarm_prepare_for_deep_sleep();
    alarm_time_timealarm_prepare_for_deep_sleep();
    port_disable_tick();
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

    // Set a flag in the backup registers to indicate sleep wakeup
    STM_ALARM_FLAG = 0x01;
    // HAL_PWR_EnableBkUpAccess();
    // __HAL_RCC_BACKUPRESET_FORCE();
    // __HAL_RCC_BACKUPRESET_RELEASE();

    HAL_PWR_EnterSTANDBYMode();

    // The above shuts down RAM, so we should never hit this
    while(1);
}

void common_hal_alarm_gc_collect(void) {
    gc_collect_ptr(common_hal_alarm_get_wake_alarm());
}
