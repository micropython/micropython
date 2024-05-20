// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

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

static uint32_t TAMPID = 0;

// Singleton instance of SleepMemory.
const alarm_sleep_memory_obj_t alarm_sleep_memory_obj = {
    .base = {
        .type = &alarm_sleep_memory_type,
    },
};

// Non-heap alarm object recording alarm (if any) that woke up CircuitPython after light or deep sleep.
// This object lives across VM instantiations, so none of these objects can contain references to the heap.
alarm_wake_alarm_union_t alarm_wake_alarm;

void alarm_reset(void) {
    // Reset the alarm flag
    alarm_pin_pinalarm_reset();
    alarm_time_timealarm_reset();
}

void alarm_get_wakeup_cause(void) {
    // Called from rtc_init, just before SWRST of RTC.  It is called
    // at an early stage of main(), to save TAMPID from SWRST.  Later,
    // common_hal_alarm_record_wake_alarm is called to make a wakeup
    // alarm from the deep sleep.

    TAMPID = RTC->MODE0.TAMPID.reg;
}

bool common_hal_alarm_woken_from_sleep(void) {
    return alarm_pin_pinalarm_woke_this_cycle() || alarm_time_timealarm_woke_this_cycle();
}

mp_obj_t common_hal_alarm_record_wake_alarm(void) {
    // Called from main.c on the first start up, just before alarm_reset.
    // Return a copy of wakeup alarm from deep sleep / fake deep sleep.
    // In case of fake sleep, status should be left in TimeAlarm/PinAlarm.
    bool true_deep = RSTC->RCAUSE.bit.BACKUP;

    if (alarm_pin_pinalarm_woke_this_cycle()) {
        TAMPID = RTC->MODE0.TAMPID.reg;
        RTC->MODE0.TAMPID.reg = TAMPID;         // clear register
        return alarm_pin_pinalarm_record_wake_alarm(TAMPID);
    }
    if (alarm_time_timealarm_woke_this_cycle() || (true_deep && TAMPID == 0)) {
        return alarm_time_timealarm_record_wake_alarm();
    }
    if (true_deep) {
        return alarm_pin_pinalarm_record_wake_alarm(TAMPID);
    }
    return mp_const_none;
}

// Set up light sleep or deep sleep alarms.
static void _setup_sleep_alarms(bool deep_sleep, size_t n_alarms, const mp_obj_t *alarms) {
    alarm_pin_pinalarm_set_alarms(deep_sleep, n_alarms, alarms);
    alarm_time_timealarm_set_alarms(deep_sleep, n_alarms, alarms);
}

mp_obj_t common_hal_alarm_light_sleep_until_alarms(size_t n_alarms, const mp_obj_t *alarms) {
    _setup_sleep_alarms(false, n_alarms, alarms);
    mp_obj_t wake_alarm = mp_const_none;

    // This works but achieves same power consumption as time.sleep()
    PM->SLEEPCFG.reg = PM_SLEEPCFG_SLEEPMODE_STANDBY;
    while (PM->SLEEPCFG.bit.SLEEPMODE != PM_SLEEPCFG_SLEEPMODE_STANDBY_Val) {
    }
    // STDBYCFG is left to be 0 to retain SYSRAM.  Note that, even if
    // RAMCFG_OFF is set here, SYSRAM seems to be retained, probably
    // because RTC and/or USB keeps sleepwalking.

    while (!mp_hal_is_interrupted()) {
        RUN_BACKGROUND_TASKS;
        // Detect if interrupt was alarm or ctrl-C interrupt.
        if (alarm_time_timealarm_woke_this_cycle()) {
            wake_alarm = alarm_time_timealarm_find_triggered_alarm(n_alarms, alarms);
            break;
        }
        if (alarm_pin_pinalarm_woke_this_cycle()) {
            wake_alarm = alarm_pin_pinalarm_find_triggered_alarm(n_alarms, alarms);
            break;
        }

        // Clear the FPU interrupt because it can prevent us from sleeping.
        if (__get_FPSCR() & ~(0x9f)) {
            __set_FPSCR(__get_FPSCR() & ~(0x9f));
            (void)__get_FPSCR();
        }

        common_hal_mcu_disable_interrupts();
        __DSB(); // Data Synchronization Barrier
        __WFI(); // Wait For Interrupt
        common_hal_mcu_enable_interrupts();
    }
    // Restore SLEEPCFG or port_idle_until_interrupt sleeps in STANDBY mode.
    PM->SLEEPCFG.reg = PM_SLEEPCFG_SLEEPMODE_IDLE2;
    while (PM->SLEEPCFG.bit.SLEEPMODE != PM_SLEEPCFG_SLEEPMODE_IDLE2_Val) {
    }
    alarm_pin_pinalarm_deinit_alarms(n_alarms, alarms); // after care for alarm_pin_pinalarm_set_alarms
    alarm_reset();

    if (mp_hal_is_interrupted()) {
        return mp_const_none; // Shouldn't be given to python code because exception handling should kick in.
    }

    return wake_alarm;
}

void common_hal_alarm_set_deep_sleep_alarms(size_t n_alarms, const mp_obj_t *alarms, size_t n_dios, digitalio_digitalinout_obj_t **preserve_dios) {
    if (n_dios > 0) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_preserve_dios);
    }
    _setup_sleep_alarms(true, n_alarms, alarms);
}

void NORETURN common_hal_alarm_enter_deep_sleep(void) {
    alarm_pin_pinalarm_prepare_for_deep_sleep();
    alarm_time_timealarm_prepare_for_deep_sleep();
    // port_disable_tick(); // TODO: Required for SAMD?

    // cache alarm flag and etc since RTC about to be reset
    uint32_t _flag = SAMD_ALARM_FLAG;           // RTC->MODE0.BKUP[0].reg
    uint32_t _target = RTC->MODE0.COMP[1].reg;
    uint32_t _tampctrl = RTC->MODE0.TAMPCTRL.reg;

    // Clear the FPU interrupt because it can prevent us from sleeping.
    if (__get_FPSCR() & ~(0x9f)) {
        __set_FPSCR(__get_FPSCR() & ~(0x9f));
        (void)__get_FPSCR();
    }

    common_hal_mcu_disable_interrupts();
    // Must disable the RTC before writing to EVCTRL and TMPCTRL
    RTC->MODE0.CTRLA.bit.SWRST = 1;                      // Software reset the RTC
    while (RTC->MODE0.SYNCBUSY.bit.SWRST) {              // Wait for synchronization
    }
    RTC->MODE0.CTRLA.reg = RTC_MODE0_CTRLA_PRESCALER_DIV1024 |   // Set prescaler to 1024
        RTC_MODE0_CTRLA_MODE_COUNT32;                            // Set RTC to mode 0, 32-bit timer

    SAMD_ALARM_FLAG = _flag;
    // Check if we're setting TimeAlarm
    if (SAMD_ALARM_FLAG_TIME_CHK) {
        RTC->MODE0.COMP[1].reg = _target;
        while (RTC->MODE0.SYNCBUSY.reg) {
        }
        RTC->MODE0.INTENSET.reg = RTC_MODE0_INTENSET_CMP1;
    }
    // Check if we're setting PinAlarm
    if (SAMD_ALARM_FLAG_PIN_CHK) {
        RTC->MODE0.TAMPCTRL.reg = _tampctrl;
        RTC->MODE0.INTENSET.reg = RTC_MODE0_INTENSET_TAMPER;
    }
    // Enable interrupts
    common_hal_mcu_enable_interrupts();

    // Set-up Deep Sleep Mode with backup RAM retention
    PM->SLEEPCFG.reg = PM_SLEEPCFG_SLEEPMODE_BACKUP;
    while (PM->SLEEPCFG.bit.SLEEPMODE != PM_SLEEPCFG_SLEEPMODE_BACKUP_Val) {
    }
    RTC->MODE0.CTRLA.bit.ENABLE = 1;                      // Enable the RTC
    while (RTC->MODE0.SYNCBUSY.bit.ENABLE) {              // Wait for synchronization
    }

    __DSB(); // Data Synchronization Barrier
    __WFI(); // Wait For Interrupt

    // The above shuts down RAM and triggers a reset, so we should never hit this
    while (1) {
        ;
    }
}

// Default common_hal_alarm_pretending_deep_sleep is defined in
// shared-bindings, which is used here.  Note that "pretending" does
// not work on REPL; it only works for main.py (or code.py, ...).

void common_hal_alarm_gc_collect(void) {
    gc_collect_ptr(shared_alarm_get_wake_alarm());
}
