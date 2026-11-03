// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/gc.h"
#include "py/obj.h"
#include "py/objtuple.h"
#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"

#include "shared-bindings/alarm/__init__.h"
#include "shared-bindings/alarm/SleepMemory.h"
#include "shared-bindings/alarm/pin/PinAlarm.h"
#include "shared-bindings/alarm/time/TimeAlarm.h"
#include "shared-bindings/alarm/touch/TouchAlarm.h"

#include "shared-bindings/microcontroller/__init__.h"

#if CIRCUITPY_CYW43
#include "bindings/cyw43/__init__.h"
#endif

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
#include "hardware/pll.h"
#include "hardware/regs/io_bank0.h"

#if PICO_RP2350
#include "hardware/powman.h"
#include "pico/low_power.h"
#include "pico/multicore.h"
#endif

// Light sleep turns off nonvolatile Busio and other wake-only peripherals
// TODO: this only saves about 2mA right now, expand with other non-essentials
#if PICO_RP2040
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
#else
// Same peripherals as RP2040. RP2350 adds PIO2 and moves SPI to the second bank.
const uint32_t RP_LIGHTSLEEP_EN0_MASK = ~(
    CLOCKS_SLEEP_EN0_CLK_SYS_PWM_BITS |
    CLOCKS_SLEEP_EN0_CLK_SYS_PIO2_BITS |
    CLOCKS_SLEEP_EN0_CLK_SYS_PIO1_BITS |
    CLOCKS_SLEEP_EN0_CLK_SYS_PIO0_BITS |
    CLOCKS_SLEEP_EN0_CLK_SYS_I2C1_BITS |
    CLOCKS_SLEEP_EN0_CLK_SYS_I2C0_BITS |
    CLOCKS_SLEEP_EN0_CLK_SYS_ADC_BITS |
    CLOCKS_SLEEP_EN0_CLK_ADC_BITS
    );
// This bank has the USB clocks in it, only turn off SPI
const uint32_t RP_LIGHTSLEEP_EN1_MASK = CLOCKS_SLEEP_EN1_RESET & ~(
    CLOCKS_SLEEP_EN1_CLK_SYS_SPI1_BITS |
    CLOCKS_SLEEP_EN1_CLK_PERI_SPI1_BITS |
    CLOCKS_SLEEP_EN1_CLK_SYS_SPI0_BITS |
    CLOCKS_SLEEP_EN1_CLK_PERI_SPI0_BITS
    );
#endif

#if PICO_RP2040
// Light sleeps used for TimeAlarm deep sleep turn off almost everything
const uint32_t RP_LIGHTSLEEP_EN0_MASK_HARSH = (
    CLOCKS_SLEEP_EN0_CLK_RTC_RTC_BITS |
    CLOCKS_SLEEP_EN0_CLK_SYS_PADS_BITS
    );
const uint32_t RP_LIGHTSLEEP_EN1_MASK_HARSH = 0x0;

static void prepare_for_dormant_xosc(void);
#endif

// Singleton instance of SleepMemory.
const alarm_sleep_memory_obj_t alarm_sleep_memory_obj = {
    .base = {
        .type = &alarm_sleep_memory_type,
    },
};

// Non-heap alarm object recording alarm (if any) that woke up CircuitPython after light or deep sleep.
// This object lives across VM instantiations, so none of these objects can contain references to the heap.
alarm_wake_alarm_union_t alarm_wake_alarm;

#if PICO_RP2350
// powman remembers what caused the last power up until the next one. Only report
// it until the first alarm_reset() after boot.
static bool powman_wakeup_reported;

bool alarm_woke_from_powman(void) {
    // powman is not reset by a watchdog reboot, so its registers would be stale.
    return (powman_hw->chip_reset & POWMAN_CHIP_RESET_HAD_SWCORE_PD_BITS) &&
           !watchdog_caused_reboot() &&
           (powman_hw->last_swcore_pwrup & (RP_POWMAN_PWRUP_GPIO_BITS | RP_POWMAN_PWRUP_ALARM_BITS));
}
#endif

void alarm_reset(void) {
    alarm_sleep_memory_reset();
    alarm_pin_pinalarm_reset();
    alarm_time_timealarm_reset();

    // Reset the scratch source
    watchdog_hw->scratch[RP_WKUP_SCRATCH_REG] = RP_SLEEP_WAKEUP_UNDEF;
    #if PICO_RP2350
    powman_wakeup_reported = true;
    #endif
}

static uint8_t _get_wakeup_cause(void) {
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
    #if PICO_RP2350
    if (!powman_wakeup_reported && alarm_woke_from_powman()) {
        if (powman_hw->last_swcore_pwrup & RP_POWMAN_PWRUP_ALARM_BITS) {
            return RP_SLEEP_WAKEUP_RTC;
        }
        return RP_SLEEP_WAKEUP_GPIO;
    }
    #endif
    return RP_SLEEP_WAKEUP_UNDEF;
}

// Set up light sleep or deep sleep alarms.
static void _setup_sleep_alarms(bool deep_sleep, size_t n_alarms, const mp_obj_t *alarms) {
    alarm_pin_pinalarm_set_alarms(deep_sleep, n_alarms, alarms);
    alarm_time_timealarm_set_alarms(deep_sleep, n_alarms, alarms);
}

bool common_hal_alarm_woken_from_sleep(void) {
    return _get_wakeup_cause() != RP_SLEEP_WAKEUP_UNDEF;
}

mp_obj_t common_hal_alarm_record_wake_alarm(void) {
    // If woken from deep sleep, create a copy alarm similar to what would have
    // been passed in originally. Otherwise, just return none
    uint8_t cause = _get_wakeup_cause();
    switch (cause) {
        case RP_SLEEP_WAKEUP_RTC: {
            return alarm_time_timealarm_record_wake_alarm();
        }

        case RP_SLEEP_WAKEUP_GPIO: {
            return alarm_pin_pinalarm_record_wake_alarm();
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

    // Save current clocks.
    uint32_t saved_sleep_en0 = clocks_hw->sleep_en0;
    uint32_t saved_sleep_en1 = clocks_hw->sleep_en1;

    while (!mp_hal_is_interrupted()) {
        RUN_BACKGROUND_TASKS;
        // Detect if interrupt was alarm or ctrl-C interrupt.
        if (common_hal_alarm_woken_from_sleep()) {
            uint8_t cause = _get_wakeup_cause();
            switch (cause) {
                case RP_SLEEP_WAKEUP_RTC: {
                    wake_alarm = alarm_time_timealarm_find_triggered_alarm(n_alarms, alarms);
                    break;
                }
                case RP_SLEEP_WAKEUP_GPIO: {
                    wake_alarm = alarm_pin_pinalarm_find_triggered_alarm(n_alarms, alarms);
                    break;
                }
                default:
                    // Should not reach this, if all light sleep types are covered correctly
                    break;
            }
            shared_alarm_save_wake_alarm(wake_alarm);
            break;
        }

        // Prune the clocks for sleep.
        clocks_hw->sleep_en0 &= RP_LIGHTSLEEP_EN0_MASK;
        clocks_hw->sleep_en1 = RP_LIGHTSLEEP_EN1_MASK;

        // Enable System Control Block (SCB) deep sleep
        scb_hw->scr |= ARM_CPU_PREFIXED(SCR_SLEEPDEEP_BITS);

        __wfi();
    }

    // Restore clocks so other wfi() uses, like time.sleep(), won't use the light-sleep settings.
    clocks_hw->sleep_en0 = saved_sleep_en0;
    clocks_hw->sleep_en1 = saved_sleep_en1;

    if (mp_hal_is_interrupted()) {
        return mp_const_none; // Shouldn't be given to python code because exception handling should kick in.
    }


    alarm_reset();
    return wake_alarm;
}

void common_hal_alarm_set_deep_sleep_alarms(size_t n_alarms, const mp_obj_t *alarms, size_t n_dios, digitalio_digitalinout_obj_t **preserve_dios) {
    _setup_sleep_alarms(true, n_alarms, alarms);
}

#if PICO_RP2350
// Power down with powman. Waking up reboots into main(), so this does not return.
static void MP_NORETURN rp2350_enter_deep_sleep(void) {
    #if CIRCUITPY_CYW43
    cyw43_enter_deep_sleep();
    #endif

    // Powman does not power down until both cores are in __wfi(). Core1 may be
    // busy, for example running the PIO USB host loop, so park it in the bootrom.
    multicore_reset_core1();

    // pico_low_power arms one wakeup per call and leaves the others alone, so
    // arm every pin alarm first.
    alarm_pin_pinalarm_enable_powman_wakeups();

    // A NULL power state keeps only the SRAM that holds .persistent_data powered.
    // That is where alarm.sleep_memory lives.
    uint pin_number;
    bool edge;
    bool value;
    if (!alarm_time_timealarm_is_set() &&
        alarm_pin_pinalarm_first_powman_wakeup(&pin_number, &edge, &value)) {
        low_power_pstate_until_gpio_pin_state(pin_number, edge, value, NULL, NULL);
    } else {
        // With no alarm at all, sleep until a time that never comes.
        uint64_t wakeup_ms = alarm_time_timealarm_is_set() ? alarm_time_timealarm_get_wakeup_ms() : INT64_MAX / 1000;
        low_power_pstate_until_aon_timer(from_us_since_boot(wakeup_ms * 1000), NULL, NULL);
    }

    // Could not power down, most likely because an alarm already fired.
    watchdog_hw->scratch[RP_WKUP_SCRATCH_REG] = _get_wakeup_cause();
    reset_cpu();
}
#endif

void MP_NORETURN common_hal_alarm_enter_deep_sleep(void) {
    #if PICO_RP2350
    rp2350_enter_deep_sleep();
    #else
    bool timealarm_set = alarm_time_timealarm_is_set();

    #if CIRCUITPY_CYW43
    cyw43_enter_deep_sleep();
    #endif

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
    alarm_pin_pinalarm_save_wake_pin();

    // Just before reset, enable the pinalarm interrupt.
    alarm_pin_pinalarm_entering_deep_sleep();
    reset_cpu();
    #endif
}

void common_hal_alarm_gc_collect(void) {
    gc_collect_ptr(shared_alarm_get_wake_alarm());
}

#if PICO_RP2040
static void prepare_for_dormant_xosc(void) {
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
#endif
