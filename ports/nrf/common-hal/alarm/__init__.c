/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2020 Dan Halbert for Adafruit Industries
 * Copyright (c) 2021 Junji Sakai
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
#include <stdio.h>
#include <string.h>

#include "shared-bindings/alarm/__init__.h"
#include "shared-bindings/alarm/SleepMemory.h"
#include "shared-bindings/alarm/pin/PinAlarm.h"
#include "shared-bindings/alarm/time/TimeAlarm.h"
#include "shared-bindings/alarm/touch/TouchAlarm.h"
#include "shared-bindings/time/__init__.h"

#include "supervisor/port.h"
#include "supervisor/serial.h"  // serial_connected()
#ifdef NRF_DEBUG_PRINT
#include "supervisor/serial.h" // dbg_printf()
extern int dbg_check_RTCprescaler(void);
#endif
#include "supervisor/qspi_flash.h"

#include "nrf.h"
#include "nrf_power.h"
#include "nrfx.h"
#include "nrfx_gpiote.h"

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
    alarm_touch_touchalarm_reset();
}

extern uint32_t reset_reason_saved;
STATIC nrf_sleep_source_t _get_wakeup_cause(void) {
    // First check if the modules remember what last woke up
    if (alarm_pin_pinalarm_woke_this_cycle()) {
        return NRF_SLEEP_WAKEUP_GPIO;
    }
    if (alarm_time_timealarm_woke_this_cycle()) {
        return NRF_SLEEP_WAKEUP_TIMER;
    }
    if (alarm_touch_touchalarm_woke_this_cycle()) {
        return NRF_SLEEP_WAKEUP_TOUCHPAD;
    }
    // If waking from true deep sleep, modules will have lost their state,
    // so check the deep wakeup cause manually
    if (reset_reason_saved & NRF_POWER_RESETREAS_RESETPIN_MASK) {
        return NRF_SLEEP_WAKEUP_RESETPIN;
    } else if (reset_reason_saved & NRF_POWER_RESETREAS_OFF_MASK) {
        return NRF_SLEEP_WAKEUP_GPIO;
    } else if (reset_reason_saved & NRF_POWER_RESETREAS_VBUS_MASK) {
        return NRF_SLEEP_WAKEUP_VBUS;
    }
    return NRF_SLEEP_WAKEUP_UNDEFINED;
}

#ifdef NRF_DEBUG_PRINT
static const char *cause_str[] = {
    "UNDEFINED",
    "GPIO",
    "TIMER",
    "TOUCHPAD",
    "VBUS",
    "RESETPIN",
};
void print_wakeup_cause(nrf_sleep_source_t cause) {
    if (cause >= 0 && cause < NRF_SLEEP_WAKEUP_ZZZ) {
        dbg_printf("wakeup cause = NRF_SLEEP_WAKEUP_%s\r\n",
            cause_str[(int)cause]);
    }
}
#endif

bool common_hal_alarm_woken_from_sleep(void) {
    nrf_sleep_source_t cause = _get_wakeup_cause();
    #ifdef NRF_DEBUG_PRINT
    if (cause != NRF_SLEEP_WAKEUP_UNDEFINED) {
        // print_wakeup_cause(cause);
    }
    #endif
    return cause == NRF_SLEEP_WAKEUP_GPIO || cause == NRF_SLEEP_WAKEUP_TIMER
           || cause == NRF_SLEEP_WAKEUP_TOUCHPAD;
}

mp_obj_t common_hal_alarm_create_wake_alarm(void) {
    // If woken from deep sleep, create a copy alarm similar to what would have
    // been passed in originally. Otherwise, just return none
    nrf_sleep_source_t cause = _get_wakeup_cause();
    switch (cause) {
        case NRF_SLEEP_WAKEUP_TIMER: {
            return alarm_time_timealarm_create_wakeup_alarm();
        }
        case NRF_SLEEP_WAKEUP_TOUCHPAD: {
            return alarm_touch_touchalarm_create_wakeup_alarm();
        }
        case NRF_SLEEP_WAKEUP_GPIO: {
            return alarm_pin_pinalarm_create_wakeup_alarm();
        }
        default:
            break;
    }
    return mp_const_none;
}

// Set up light sleep or deep sleep alarms.
STATIC void _setup_sleep_alarms(bool deep_sleep, size_t n_alarms, const mp_obj_t *alarms) {
    alarm_pin_pinalarm_set_alarms(deep_sleep, n_alarms, alarms);
    alarm_time_timealarm_set_alarms(deep_sleep, n_alarms, alarms);
    alarm_touch_touchalarm_set_alarm(deep_sleep, n_alarms, alarms);
}

// TODO: this handles all possible types of wakeup, which is redundant with main.
// revise to extract all parts essential to enabling sleep wakeup, but leave the
// alarm/non-alarm sorting to the existing main loop.
void system_on_idle_until_alarm(int64_t timediff_ms, uint32_t prescaler) {
    bool have_timeout = false;
    uint64_t start_tick = 0, end_tick = 0;
    int64_t tickdiff;

    #if defined(MICROPY_QSPI_CS)
    qspi_flash_enter_sleep();
    #endif

    if (timediff_ms != -1) {
        have_timeout = true;
        #if 0
        int64_t now = common_hal_time_monotonic_ms();
        dbg_printf("now_ms=%ld timediff_ms=%ld\r\n", (long)now, (long)timediff_ms);
        #endif
        if (timediff_ms < 0) {
            timediff_ms = 0;
        }
        if (prescaler == 0) {
            // 1 tick = 1/1024 sec = 1000/1024 ms
            // -> 1 ms = 1024/1000 ticks
            tickdiff = (mp_uint_t)(timediff_ms * 1024 / 1000); // ms -> ticks
        } else {
            // 1 tick = prescaler/1024 sec = prescaler*1000/1024 ms
            // -> 1ms = 1024/(1000*prescaler) ticks
            tickdiff = (mp_uint_t)(timediff_ms * 1024 / (1000 * prescaler));
        }
        start_tick = port_get_raw_ticks(NULL);
        end_tick = start_tick + tickdiff;
    }
    #if 0
    dbg_printf("start_tick=%ld end_tick=%ld have_timeout=%c\r\n", (long)start_tick, (long)end_tick, have_timeout ? 'T' : 'F');
    #endif

    int64_t remaining;
    sleepmem_wakeup_event = SLEEPMEM_WAKEUP_BY_NONE;
    sleepmem_wakeup_pin = WAKEUP_PIN_UNDEF;

    #ifdef NRF_DEBUG_PRINT
    int ct = 40;
    char reason = '?';
#define WAKEUP_REASON(x) reason = (x)
    #else
#define WAKEUP_REASON(x)
    #endif

    while (1) {
        if (mp_hal_is_interrupted()) {
            WAKEUP_REASON('I');
            break;
        }
        if (serial_connected() && serial_bytes_available()) {
            WAKEUP_REASON('S');
            break;
        }
        RUN_BACKGROUND_TASKS;
        if (common_hal_alarm_woken_from_sleep()) {
            WAKEUP_REASON('W');
            break;
        }
        if (have_timeout) {
            remaining = end_tick - port_get_raw_ticks(NULL);
            // We break a bit early so we don't risk setting the alarm before the time when we call
            // sleep.
            if (remaining < 1) {
                WAKEUP_REASON('t');
                break;
            }
            port_interrupt_after_ticks(remaining);
        }
        // Idle until an interrupt happens.
        port_idle_until_interrupt();
        #ifdef NRF_DEBUG_PRINT
        if (ct > 0) {
            dbg_printf("_");
            --ct;
        }
        #endif
        if (have_timeout) {
            remaining = end_tick - port_get_raw_ticks(NULL);
            if (remaining <= 0) {
                sleepmem_wakeup_event = SLEEPMEM_WAKEUP_BY_TIMER;
                WAKEUP_REASON('T');
                break;
            }
        }
    }
    #ifdef NRF_DEBUG_PRINT
    dbg_printf("%c\r\n", reason);
    #endif

    #if defined(MICROPY_QSPI_CS)
    qspi_flash_exit_sleep();
    #endif

    #ifdef NRF_DEBUG_PRINT
    tickdiff = port_get_raw_ticks(NULL) - start_tick;
    double sec;
    if (prescaler == 0) {
        sec = (double)tickdiff / 1024;
    } else {
        sec = (double)(tickdiff * prescaler) / 1024;
    }
    dbg_printf("lapse %6.1f sec\r\n", sec);
    #endif
}

mp_obj_t common_hal_alarm_light_sleep_until_alarms(size_t n_alarms, const mp_obj_t *alarms) {
    mp_obj_t wake_alarm = mp_const_none;
    alarm_time_timealarm_clear_wakeup_time();
    _setup_sleep_alarms(false, n_alarms, alarms);

    #ifdef NRF_DEBUG_PRINT
    dbg_printf("\r\nlight sleep...");
    #endif

    int64_t timediff_ms = alarm_time_timealarm_get_wakeup_timediff_ms();
    system_on_idle_until_alarm(timediff_ms, 0);

    if (mp_hal_is_interrupted()) {
        wake_alarm = mp_const_none;
    } else {
        if (common_hal_alarm_woken_from_sleep()) {
            nrf_sleep_source_t cause = _get_wakeup_cause();
            switch (cause) {
                case NRF_SLEEP_WAKEUP_TIMER: {
                    wake_alarm = alarm_time_timealarm_find_triggered_alarm(n_alarms,alarms);
                    break;
                }
                case NRF_SLEEP_WAKEUP_GPIO: {
                    wake_alarm = alarm_pin_pinalarm_find_triggered_alarm(n_alarms,alarms);
                    break;
                }
                default:
                    // Should not reach this, if all light sleep types are covered correctly
                    break;
            }
            shared_alarm_save_wake_alarm(wake_alarm);
        }
    }
    alarm_reset();
    return wake_alarm;
}

void common_hal_alarm_set_deep_sleep_alarms(size_t n_alarms, const mp_obj_t *alarms) {
    alarm_time_timealarm_clear_wakeup_time();
    _setup_sleep_alarms(true, n_alarms, alarms);
}

#define PRESCALER_VALUE_IN_DEEP_SLEEP (1024)

void NORETURN common_hal_alarm_enter_deep_sleep(void) {
    alarm_pin_pinalarm_prepare_for_deep_sleep();
    alarm_time_timealarm_prepare_for_deep_sleep();

    #ifdef NRF_DEBUG_PRINT
    dbg_printf("\r\ndeep sleep...");
    #endif
    int64_t timediff_ms = alarm_time_timealarm_get_wakeup_timediff_ms();
    tick_set_prescaler(PRESCALER_VALUE_IN_DEEP_SLEEP - 1);
    #ifdef NRF_DEBUG_PRINT
    dbg_check_RTCprescaler(); // XXX
    #endif
    system_on_idle_until_alarm(timediff_ms, PRESCALER_VALUE_IN_DEEP_SLEEP);

    #ifdef NRF_DEBUG_PRINT
    dbg_printf("RESET...\r\n\r\n");
    #endif

    reset_cpu();

    // should not reach here..
    while (1) {
        ;
    }
}

// old version deep sleep code that was used in common_hal_alarm_enter_deep_sleep()
//   for anyone who might want true System OFF sleep ..
#if 0
void OLD_go_system_off(void) {
    sleepmem_wakeup_event = SLEEPMEM_WAKEUP_BY_NONE;
    sleepmem_wakeup_pin = WAKEUP_PIN_UNDEF;
    uint8_t sd_enabled;
    sd_softdevice_is_enabled(&sd_enabled);
    set_memory_retention();
    dbg_printf("OLD go system off.. %d\r\n", sd_enabled);
    if (sd_enabled) {
        sd_power_system_off();
    } else {
        NRF_POWER->SYSTEMOFF = 1;
    }
}
#endif

void common_hal_alarm_pretending_deep_sleep(void) {
    alarm_pin_pinalarm_prepare_for_deep_sleep();
    alarm_time_timealarm_prepare_for_deep_sleep();

    #ifdef NRF_DEBUG_PRINT
    dbg_printf("\r\npretending to deep sleep...");
    #endif

    int64_t timediff_ms = alarm_time_timealarm_get_wakeup_timediff_ms();
    system_on_idle_until_alarm(timediff_ms, 0);

    alarm_reset();
}

void common_hal_alarm_gc_collect(void) {
    gc_collect_ptr(shared_alarm_get_wake_alarm());
}
