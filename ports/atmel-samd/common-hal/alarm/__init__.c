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

#include "shared-bindings/alarm/__init__.h"
#include "shared-bindings/alarm/SleepMemory.h"
#include "shared-bindings/alarm/pin/PinAlarm.h"
#include "shared-bindings/alarm/time/TimeAlarm.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared/runtime/interrupt_char.h"
#include "samd/external_interrupts.h"
#include "supervisor/port.h"
#include "supervisor/workflow.h"

// Singleton instance of SleepMemory.
const alarm_sleep_memory_obj_t alarm_sleep_memory_obj = {
    .base = {
        .type = &alarm_sleep_memory_type,
    },
};
// TODO: make a custom enum to avoid weird values like PM_SLEEPCFG_SLEEPMODE_BACKUP_Val?
STATIC volatile uint32_t _target;
STATIC bool fake_sleep;
STATIC bool pin_wake;

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
    if (!fake_sleep && RSTC->RCAUSE.bit.BACKUP) {
        // This is checked during rtc_init to cache TAMPID if necessary
        if (pin_wake || RTC->MODE0.TAMPID.reg) {
            pin_wake = true;
            return SAMD_WAKEUP_GPIO;
        }
        return SAMD_WAKEUP_RTC;
    }
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
    fake_sleep = false;
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
        // ATTEMPT ------------------------------
        // This works but achieves same power consumption as time.sleep()

        // Clear the FPU interrupt because it can prevent us from sleeping.
        if (__get_FPSCR() & ~(0x9f)) {
            __set_FPSCR(__get_FPSCR() & ~(0x9f));
            (void)__get_FPSCR();
        }

        // Disable RTC interrupts
        NVIC_DisableIRQ(RTC_IRQn);
        // Set standby power domain stuff
        PM->STDBYCFG.reg = PM_STDBYCFG_RAMCFG_OFF;
        // Set-up Sleep Mode
        PM->SLEEPCFG.reg = PM_SLEEPCFG_SLEEPMODE_STANDBY;
        while (PM->SLEEPCFG.bit.SLEEPMODE != PM_SLEEPCFG_SLEEPMODE_STANDBY_Val) {
            ;
        }

        __DSB(); // Data Synchronization Barrier
        __WFI(); // Wait For Interrupt
        // Enable RTC interrupts
        NVIC_EnableIRQ(RTC_IRQn);
        // END ATTEMPT ------------------------------
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
    _target = RTC->MODE0.COMP[1].reg;
    port_disable_tick(); // TODO: Required for SAMD?

    // cache alarm flag since backup registers about to be reset
    uint32_t _SAMD_ALARM_FLAG = SAMD_ALARM_FLAG;

    // Clear the FPU interrupt because it can prevent us from sleeping.
    if (__get_FPSCR() & ~(0x9f)) {
        __set_FPSCR(__get_FPSCR() & ~(0x9f));
        (void)__get_FPSCR();
    }

    NVIC_DisableIRQ(RTC_IRQn);
    // Must disable the RTC before writing to EVCTRL and TMPCTRL
    RTC->MODE0.CTRLA.bit.ENABLE = 0;                     // Disable the RTC
    while (RTC->MODE0.SYNCBUSY.bit.ENABLE) {             // Wait for synchronization
        ;
    }
    RTC->MODE0.CTRLA.bit.SWRST = 1;                      // Software reset the RTC
    while (RTC->MODE0.SYNCBUSY.bit.SWRST) {              // Wait for synchronization
        ;
    }
    RTC->MODE0.CTRLA.reg = RTC_MODE0_CTRLA_PRESCALER_DIV1024 |   // Set prescaler to 1024
        RTC_MODE0_CTRLA_MODE_COUNT32;                            // Set RTC to mode 0, 32-bit timer

    // Check if we're setting TimeAlarm
    if (_SAMD_ALARM_FLAG & SAMD_ALARM_FLAG_TIME) {
        RTC->MODE0.COMP[1].reg = (_target / 1024) * 32;
        while (RTC->MODE0.SYNCBUSY.reg) {
            ;
        }
    }
    // Check if we're setting PinAlarm
    if (_SAMD_ALARM_FLAG & SAMD_ALARM_FLAG_PIN) {
        RTC->MODE0.TAMPCTRL.bit.DEBNC2 = 1;  // Edge triggered when INn is stable for 4 CLK_RTC_DEB periods
        RTC->MODE0.TAMPCTRL.bit.TAMLVL2 = 1; // rising edge
        // PA02 = IN2
        RTC->MODE0.TAMPCTRL.bit.IN2ACT = 1;  // WAKE on IN2 (doesn't save timestamp)
    }
    // Enable interrupts
    NVIC_SetPriority(RTC_IRQn, 0);
    NVIC_EnableIRQ(RTC_IRQn);
    if (_SAMD_ALARM_FLAG & SAMD_ALARM_FLAG_TIME) {
        // Set interrupts for COMPARE1
        RTC->MODE0.INTENSET.reg = RTC_MODE0_INTENSET_CMP1;
    }
    if (_SAMD_ALARM_FLAG & SAMD_ALARM_FLAG_PIN) {
        // Set interrupts for TAMPER pins
        RTC->MODE0.INTENSET.reg = RTC_MODE0_INTENSET_TAMPER;
    }

    // Set-up Deep Sleep Mode & RAM retention
    PM->BKUPCFG.reg = PM_BKUPCFG_BRAMCFG(0x2);       // No RAM retention 0x2 partial:0x1
    while (PM->BKUPCFG.bit.BRAMCFG != 0x2) {         // Wait for synchronization
        ;
    }
    PM->SLEEPCFG.reg = PM_SLEEPCFG_SLEEPMODE_BACKUP;
    while (PM->SLEEPCFG.bit.SLEEPMODE != PM_SLEEPCFG_SLEEPMODE_BACKUP_Val) {
        ;
    }
    RTC->MODE0.CTRLA.bit.ENABLE = 1;                      // Enable the RTC
    while (RTC->MODE0.SYNCBUSY.bit.ENABLE) {              // Wait for synchronization
        ;
    }

    __DSB(); // Data Synchronization Barrier
    __WFI(); // Wait For Interrupt

    // The above shuts down RAM and triggers a reset, so we should never hit this
    while (1) {
        ;
    }
}

void common_hal_alarm_pretending_deep_sleep(void) {
    // TODO:
    //      If tamper detect interrupts cannot be used to wake from the Idle tier of sleep,
    //      This section will need to re-initialize the pins to allow the PORT peripheral
    //      to generate external interrupts again. See STM32 for reference.

    if (!fake_sleep) {
        fake_sleep = true;
    }
}

void common_hal_alarm_gc_collect(void) {
    gc_collect_ptr(shared_alarm_get_wake_alarm());
}
