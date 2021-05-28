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
#include "shared-bindings/alarm/touch/TouchAlarm.h"

#include "shared-bindings/microcontroller/__init__.h"

#include "supervisor/port.h"
#include "supervisor/shared/workflow.h"

#include "pico/stdlib.h"
#include "hardware/sync.h"
#include "hardware/clocks.h"
#include "hardware/xosc.h"
#include "hardware/structs/scb.h"
#include "hardware/watchdog.h"
#include "hardware/structs/watchdog.h"

// XOSC shutdown
#include "hardware/rtc.h"
#include "hardware/pll.h"
#include "hardware/regs/io_bank0.h"

// Watchdog scratch register
// Not used elsewhere in the SDK for now, keep an eye on it
#define RP_WKUP_SCRATCH_REG 0

// Light sleep turns off nonvolatile Busio and other wake-only peripherals
// TODO: this only saves about 2mA right now, expand with other non-essentials
const uint32_t RP_LIGHTSLEEP_EN0_MASK = ~(
    CLOCKS_SLEEP_EN0_CLK_SYS_SPI1_BITS |
    CLOCKS_SLEEP_EN0_CLK_PERI_SPI1_BITS |
    CLOCKS_SLEEP_EN0_CLK_SYS_SPI0_BITS |
    CLOCKS_SLEEP_EN0_CLK_PERI_SPI0_BITS |
    CLOCKS_SLEEP_EN0_CLK_SYS_PWM_BITS |
    CLOCKS_SLEEP_EN0_CLK_SYS_PIO1_BITS |
    CLOCKS_SLEEP_EN0_CLK_SYS_PIO0_BITS |
    CLOCKS_SLEEP_EN0_CLK_SYS_I2C1_BITS |
    CLOCKS_SLEEP_EN0_CLK_SYS_I2C0_BITS |
    CLOCKS_SLEEP_EN0_CLK_SYS_ADC_BITS |
    CLOCKS_SLEEP_EN0_CLK_ADC_ADC_BITS
    );
// This bank has the USB clocks in it, leave it for now
const uint32_t RP_LIGHTSLEEP_EN1_MASK = CLOCKS_SLEEP_EN1_RESET;

// Light sleeps used for TimeAlarm deep sleep turn off almost everything
const uint32_t RP_LIGHTSLEEP_EN0_MASK_HARSH = (
    CLOCKS_SLEEP_EN0_CLK_RTC_RTC_BITS |
    CLOCKS_SLEEP_EN0_CLK_SYS_PADS_BITS
    );
const uint32_t RP_LIGHTSLEEP_EN1_MASK_HARSH = 0x0;

STATIC void prepare_for_dormant_xosc(void);

// Singleton instance of SleepMemory.
const alarm_sleep_memory_obj_t alarm_sleep_memory_obj = {
    .base = {
        .type = &alarm_sleep_memory_type,
    },
};

void alarm_reset(void) {
    alarm_sleep_memory_reset();
    alarm_pin_pinalarm_reset();
    alarm_time_timealarm_reset();

    // Reset the scratch source
    watchdog_hw->scratch[RP_WKUP_SCRATCH_REG] = RP_SLEEP_WAKEUP_UNDEF;
}

STATIC uint8_t _get_wakeup_cause(void) {
    // First check if the modules remember what last woke up
    if (alarm_pin_pinalarm_woke_this_cycle()) {
        return RP_SLEEP_WAKEUP_GPIO;
    }
    if (alarm_time_timealarm_woke_this_cycle()) {
        return RP_SLEEP_WAKEUP_RTC;
    }
    // If waking from true deep sleep, modules will have lost their state,
    // so check the deep wakeup cause manually
    if (watchdog_hw->scratch[RP_WKUP_SCRATCH_REG] != RP_SLEEP_WAKEUP_UNDEF) {
        return watchdog_hw->scratch[RP_WKUP_SCRATCH_REG];
    }
    return RP_SLEEP_WAKEUP_UNDEF;
}

// Set up light sleep or deep sleep alarms.
STATIC void _setup_sleep_alarms(bool deep_sleep, size_t n_alarms, const mp_obj_t *alarms) {
    alarm_pin_pinalarm_set_alarms(deep_sleep, n_alarms, alarms);
    alarm_time_timealarm_set_alarms(deep_sleep, n_alarms, alarms);
}

bool common_hal_alarm_woken_from_sleep(void) {
    return _get_wakeup_cause() != RP_SLEEP_WAKEUP_UNDEF;
}

mp_obj_t common_hal_alarm_create_wake_alarm(void) {
    // If woken from deep sleep, create a copy alarm similar to what would have
    // been passed in originally. Otherwise, just return none
    uint8_t cause = _get_wakeup_cause();
    switch (cause) {
        case RP_SLEEP_WAKEUP_RTC: {
            return alarm_time_timealarm_create_wakeup_alarm();
        }

        case RP_SLEEP_WAKEUP_GPIO: {
            return alarm_pin_pinalarm_create_wakeup_alarm();
        }

        case RP_SLEEP_WAKEUP_UNDEF:
        default:
            // Not a deep sleep reset.
            break;
    }
    return mp_const_none;
}

mp_obj_t common_hal_alarm_light_sleep_until_alarms(size_t n_alarms, const mp_obj_t *alarms) {
    _setup_sleep_alarms(false, n_alarms, alarms);

    mp_obj_t wake_alarm = mp_const_none;

    while (!mp_hal_is_interrupted()) {
        RUN_BACKGROUND_TASKS;
        // Detect if interrupt was alarm or ctrl-C interrupt.
        if (common_hal_alarm_woken_from_sleep()) {
            uint8_t cause = _get_wakeup_cause();
            switch (cause) {
                case RP_SLEEP_WAKEUP_RTC: {
                    wake_alarm = alarm_time_timealarm_find_triggered_alarm(n_alarms,alarms);
                    break;
                }
                case RP_SLEEP_WAKEUP_GPIO: {
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

        // Prune the clock for sleep
        clocks_hw->sleep_en0 &= RP_LIGHTSLEEP_EN0_MASK;
        clocks_hw->sleep_en1 = RP_LIGHTSLEEP_EN1_MASK;

        // Enable System Control Block (SCB) deep sleep
        uint save = scb_hw->scr;
        scb_hw->scr = save | M0PLUS_SCR_SLEEPDEEP_BITS;

        __wfi();
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
    bool timealarm_set = alarm_time_timealarm_is_set();

    // If there's a timealarm, just enter a very deep light sleep
    if (timealarm_set) {
        // Prune the clock for sleep
        clocks_hw->sleep_en0 &= RP_LIGHTSLEEP_EN0_MASK_HARSH;
        clocks_hw->sleep_en1 = RP_LIGHTSLEEP_EN1_MASK_HARSH;
        // Enable System Control Block (SCB) deep sleep
        uint save = scb_hw->scr;
        scb_hw->scr = save | M0PLUS_SCR_SLEEPDEEP_BITS;
        __wfi();
    } else {
        prepare_for_dormant_xosc();
        xosc_dormant();
    }
    // // TODO: support ROSC when available in SDK
    // rosc_set_dormant();

    // Reset uses the watchdog. Use scratch registers to store wake reason
    watchdog_hw->scratch[RP_WKUP_SCRATCH_REG] = _get_wakeup_cause();
    reset_cpu();
}

void common_hal_alarm_gc_collect(void) {
    gc_collect_ptr(shared_alarm_get_wake_alarm());
}

STATIC void prepare_for_dormant_xosc(void) {
    // TODO: add ROSC support with sleep_run_from_dormant_source when it's added to SDK
    uint src_hz = XOSC_MHZ * MHZ;
    uint clk_ref_src = CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC;
    clock_configure(clk_ref,
        clk_ref_src,
        0,         // No aux mux
        src_hz,
        src_hz);
    clock_configure(clk_sys,
        CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLK_REF,
        0,             // Using glitchless mux
        src_hz,
        src_hz);
    clock_stop(clk_usb);
    clock_stop(clk_adc);
    uint clk_rtc_src = CLOCKS_CLK_RTC_CTRL_AUXSRC_VALUE_XOSC_CLKSRC;
    clock_configure(clk_rtc,
        0,         // No GLMUX
        clk_rtc_src,
        src_hz,
        46875);
    clock_configure(clk_peri,
        0,
        CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
        src_hz,
        src_hz);
    pll_deinit(pll_sys);
    pll_deinit(pll_usb);
}
