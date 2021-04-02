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

STATIC uint8_t true_deep_wake_reason;
STATIC mp_obj_t most_recent_alarm;

void alarm_reset(void) {
    most_recent_alarm = NULL;
    // Reset the alarm flag
    STM_ALARM_FLAG = 0x00;
    alarm_pin_pinalarm_reset();
    alarm_time_timealarm_reset();
}

// Kind of a hack, required as RTC is reset in port.c
// TODO: in the future, don't reset it at all, just override critical flags
void alarm_set_wakeup_reason(uint8_t reason) {
    true_deep_wake_reason = reason;
}

STATIC uint8_t _get_wakeup_cause(void) {
    // If in light/fake sleep, check modules
    if (alarm_pin_pinalarm_woke_us_up()) {
        return STM_WAKEUP_GPIO;
    }
    if (alarm_time_timealarm_woke_us_up()) {
        return STM_WAKEUP_RTC;
    }
    // Check to see if we woke from deep sleep (reason set in port_init)
    if (true_deep_wake_reason) {
        return true_deep_wake_reason;
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
    // If we woke from light sleep, override with that alarm
    if (most_recent_alarm != NULL) {
        return most_recent_alarm;
    }
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
        // Detect if interrupt was alarm or ctrl-C interrupt.
        if (common_hal_alarm_woken_from_sleep()) {
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

    // TODO: make assignment to global array less roundabout
    most_recent_alarm = wake_alarm;
    shared_alarm_save_wake_alarm();

    // Can't use alarm_reset since it resets most_recent_alarm
    alarm_pin_pinalarm_reset();
    alarm_time_timealarm_reset();
    return wake_alarm;
}

void common_hal_alarm_set_deep_sleep_alarms(size_t n_alarms, const mp_obj_t *alarms) {
    most_recent_alarm = NULL;
    _setup_sleep_alarms(true, n_alarms, alarms);
}

void NORETURN common_hal_alarm_enter_deep_sleep(void) {
    alarm_pin_pinalarm_prepare_for_deep_sleep();
    alarm_time_timealarm_prepare_for_deep_sleep();
    port_disable_tick();
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

    // Set a flag in the backup registers to indicate sleep wakeup
    STM_ALARM_FLAG = 0x01;

    HAL_PWR_EnterSTANDBYMode();

    // The above shuts down RAM, so we should never hit this
    while(1);
}

void common_hal_alarm_pretending_deep_sleep(void) {
    // Re-enable the WKUP pin (PA00) since VM cleanup resets it
    // If there are no PinAlarms, EXTI won't be turned on, and this won't do anything
    // TODO: replace with `prepare_for_fake_deep_sleep` if other WKUP are added.
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin_mask(0);
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    port_idle_until_interrupt();
}

void common_hal_alarm_gc_collect(void) {
    gc_collect_ptr(shared_alarm_get_wake_alarm());
}
