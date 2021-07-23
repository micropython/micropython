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
#include "shared-bindings/alarm/SleepMemory.h"
#include "shared-bindings/alarm/pin/PinAlarm.h"
#include "shared-bindings/alarm/time/TimeAlarm.h"

#include "shared-bindings/microcontroller/__init__.h"

#include "supervisor/port.h"
#include "supervisor/workflow.h"

// Singleton instance of SleepMemory.
const alarm_sleep_memory_obj_t alarm_sleep_memory_obj = {
    .base = {
        .type = &alarm_sleep_memory_type,
    },
};

// TODO: make a custom enum to avoid weird values like PM_SLEEPCFG_SLEEPMODE_BACKUP_Val?

void alarm_reset(void) {
    // Reset the alarm flag
    SAMD_ALARM_FLAG = 0x00;
    alarm_pin_pinalarm_reset();
    alarm_time_timealarm_reset();
}

samd_sleep_source_t alarm_get_wakeup_cause(void) {
    // If in light/fake sleep, check modules
    if (alarm_pin_pinalarm_woke_this_cycle()) {
        return SAMD_WAKEUP_GPIO;
    }
    if (alarm_time_timealarm_woke_this_cycle()) {
        return SAMD_WAKEUP_RTC;
    }
    // TODO: for deep sleep, manually determine how the chip woke up
    // TODO: try checking the interrupt flag tables for RTC TAMPER vs COMPARE
    return SAMD_WAKEUP_UNDEF;
}

bool common_hal_alarm_woken_from_sleep(void) {
    return alarm_get_wakeup_cause() != SAMD_WAKEUP_UNDEF;
}

mp_obj_t common_hal_alarm_create_wake_alarm(void) {
    // If woken from deep sleep, create a copy alarm similar to what would have
    // been passed in originally. Otherwise, just return none
    samd_sleep_source_t cause = alarm_get_wakeup_cause();
    switch (cause) {
        case SAMD_WAKEUP_RTC: {
            return alarm_time_timealarm_create_wakeup_alarm();
        }
        case SAMD_WAKEUP_GPIO: {
            return alarm_pin_pinalarm_create_wakeup_alarm();
        }
        case SAMD_WAKEUP_UNDEF:
        default:
            // Not a deep sleep reset.
            break;
    }
    return mp_const_none;
}

// Set up light sleep or deep sleep alarms.
STATIC void _setup_sleep_alarms(bool deep_sleep, size_t n_alarms, const mp_obj_t *alarms) {
    alarm_pin_pinalarm_set_alarms(deep_sleep, n_alarms, alarms);
    alarm_time_timealarm_set_alarms(deep_sleep, n_alarms, alarms);
}

mp_obj_t common_hal_alarm_light_sleep_until_alarms(size_t n_alarms, const mp_obj_t *alarms) {
    _setup_sleep_alarms(false, n_alarms, alarms);
    mp_obj_t wake_alarm = mp_const_none;

    while (!mp_hal_is_interrupted()) {
        RUN_BACKGROUND_TASKS;
        // Detect if interrupt was alarm or ctrl-C interrupt.
        if (common_hal_alarm_woken_from_sleep()) {
            samd_sleep_source_t cause = alarm_get_wakeup_cause();
            switch (cause) {
                case SAMD_WAKEUP_RTC: {
                    wake_alarm = alarm_time_timealarm_find_triggered_alarm(n_alarms,alarms);
                    break;
                }
                case SAMD_WAKEUP_GPIO: {
                    wake_alarm = alarm_pin_pinalarm_find_triggered_alarm(n_alarms,alarms);
                    break;
                }
                default:
                    // Should not reach this, if all light sleep types are covered correctly
                    break;
            }
            shared_alarm_save_wake_alarm(wake_alarm);
            break;
        }
        // TODO: the SAMD implementation of this (purportedly) disables interrupts
        //       Presumably this doesn't impact the RTC interrupts, somehow, or it would never wake up?
        //       Will it prevent an external interrupt from waking?
        port_idle_until_interrupt();
        //       Alternative would be `sleep(PM_SLEEPCFG_SLEEPMODE_IDLE2_Val)`, I think?
    }

    if (mp_hal_is_interrupted()) {
        return mp_const_none; // Shouldn't be given to python code because exception handling should kick in.
    }

    alarm_reset();
    return wake_alarm;
}

void common_hal_alarm_set_deep_sleep_alarms(size_t n_alarms, const mp_obj_t *alarms) {
    _setup_sleep_alarms(true, n_alarms, alarms);
}

void NORETURN common_hal_alarm_enter_deep_sleep(void) {
    alarm_pin_pinalarm_prepare_for_deep_sleep();
    alarm_time_timealarm_prepare_for_deep_sleep();
    port_disable_tick(); // TODO: Required for SAMD?

    // Set a flag in the backup registers to indicate sleep wakeup
    SAMD_ALARM_FLAG = 0x01;

    // TODO: make sure this actually works, or replace with extracted register version
    // sleep(PM_SLEEPCFG_SLEEPMODE_BACKUP_Val);

    // The above shuts down RAM, so we should never hit this
    while (1) {
        ;
    }
}

void common_hal_alarm_pretending_deep_sleep(void) {
    // TODO:
    //      If tamper detect interrupts cannot be used to wake from the Idle tier of sleep,
    //      This section will need to re-initialize the pins to allow the PORT peripheral
    //      to generate external interrupts again. See STM32 for reference.
}

void common_hal_alarm_gc_collect(void) {
    gc_collect_ptr(shared_alarm_get_wake_alarm());
}
