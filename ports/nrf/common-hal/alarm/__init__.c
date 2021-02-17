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
#include <stdio.h>
#include <string.h>

#include "shared-bindings/alarm/__init__.h"
#include "shared-bindings/alarm/SleepMemory.h"
#include "shared-bindings/alarm/pin/PinAlarm.h"
#include "shared-bindings/alarm/time/TimeAlarm.h"
#include "shared-bindings/alarm/touch/TouchAlarm.h"

//#include "shared-bindings/wifi/__init__.h"
//#include "shared-bindings/microcontroller/__init__.h"

#include "supervisor/port.h"

#include "nrf.h"
#include "nrf_power.h"
#include "nrfx.h"
#include "nrfx_gpiote.h"

extern void _debug_print(const char* s);
extern void _xxx_dumpRTC(void);//XXXX

#define DEBUG_LED_PIN (NRF_GPIO_PIN_MAP(1, 11)) // P1_11 = LED
void _debug_led_init(void) {
    nrf_gpio_cfg_output(DEBUG_LED_PIN);
}
void _debug_led_set(int v) {
    nrf_gpio_pin_write(DEBUG_LED_PIN, v);
}


// Singleton instance of SleepMemory.
const alarm_sleep_memory_obj_t alarm_sleep_memory_obj = {
    .base = {
        .type = &alarm_sleep_memory_type,
    },
};

void alarm_reset(void) {
    //alarm_sleep_memory_reset();
    alarm_pin_pinalarm_reset();
    alarm_time_timealarm_reset();
    //alarm_touch_touchalarm_reset();
}

extern uint32_t reset_reason_saved;
STATIC uint32_t _get_wakeup_cause(void) {
    if (alarm_pin_pinalarm_woke_us_up()) {
        return NRF_SLEEP_WAKEUP_GPIO;
    }
    if (alarm_time_timealarm_woke_us_up()) {
        return NRF_SLEEP_WAKEUP_TIMER;
    }
#if 0
    if (alarm_touch_touchalarm_woke_us_up()) {
        return ESP_SLEEP_WAKEUP_TOUCHPAD;
    }
    return esp_sleep_get_wakeup_cause();
#endif
    if (reset_reason_saved & NRF_POWER_RESETREAS_RESETPIN_MASK) {
        return NRF_SLEEP_WAKEUP_RESETPIN;
    }
    else if (reset_reason_saved & NRF_POWER_RESETREAS_OFF_MASK) {
        return NRF_SLEEP_WAKEUP_GPIO;
    }
    else if (reset_reason_saved & NRF_POWER_RESETREAS_VBUS_MASK) {
        return NRF_SLEEP_WAKEUP_VBUS;
    }
    return NRF_SLEEP_WAKEUP_UNDEFINED;
}

bool alarm_woken_from_sleep(void) {
   uint32_t cause = _get_wakeup_cause();
   return (cause == NRF_SLEEP_WAKEUP_GPIO || cause == NRF_SLEEP_WAKEUP_TIMER
	   || cause == NRF_SLEEP_WAKEUP_TOUCHPAD
	   || cause == NRF_SLEEP_WAKEUP_RESETPIN);
}

STATIC mp_obj_t _get_wake_alarm(size_t n_alarms, const mp_obj_t *alarms) {
    uint32_t cause = _get_wakeup_cause();
    if (cause & 0x80000000) {
      printf("wakeup cause = 0x%08X\r\n", (int)cause);
    }
    switch (cause) {
      case NRF_SLEEP_WAKEUP_TIMER: {
	return alarm_time_timealarm_get_wakeup_alarm(n_alarms, alarms);
      }
      case NRF_SLEEP_WAKEUP_TOUCHPAD: {
	return mp_const_none;
      }
      case NRF_SLEEP_WAKEUP_GPIO: {
	return alarm_pin_pinalarm_get_wakeup_alarm(n_alarms, alarms);
      }
    }
    return mp_const_none;
}

mp_obj_t common_hal_alarm_get_wake_alarm(void) {
    mp_obj_t obj = _get_wake_alarm(0, NULL);
    //_xxx_dumpRTC();//XXX
    return obj;
}

// Set up light sleep or deep sleep alarms.
STATIC void _setup_sleep_alarms(bool deep_sleep, size_t n_alarms, const mp_obj_t *alarms) {
    alarm_pin_pinalarm_set_alarms(deep_sleep, n_alarms, alarms);
    alarm_time_timealarm_set_alarms(deep_sleep, n_alarms, alarms);
#if 0
    alarm_touch_touchalarm_set_alarm(deep_sleep, n_alarms, alarms);
#endif
    //_xxx_dumpRTC();
}

STATIC void _idle_until_alarm(void) {
    int ct = 40;
    // Poll for alarms.
    while (!mp_hal_is_interrupted()) {
        RUN_BACKGROUND_TASKS;
        // Allow ctrl-C interrupt.
        if (alarm_woken_from_sleep()) {
            alarm_save_wake_alarm();
	    int cause = _get_wakeup_cause();
	    printf("wakeup(%d)\r\n", cause); //XXX
            return;
        }
        port_idle_until_interrupt();
	if (ct > 0) {
	  printf("_");  --ct;
	}
    }
}

mp_obj_t common_hal_alarm_light_sleep_until_alarms(size_t n_alarms, const mp_obj_t *alarms) {
    mp_obj_t r_obj = mp_const_none;
    _setup_sleep_alarms(false, n_alarms, alarms);
    _debug_print("\r\nsleep...");

    _idle_until_alarm();

    if (mp_hal_is_interrupted()) {
        _debug_print("mp_hal_is_interrupted\r\n");
        r_obj = mp_const_none;
    }
    else {
      r_obj = _get_wake_alarm(n_alarms, alarms);
      alarm_reset();
    }
    return r_obj;
}

void common_hal_alarm_set_deep_sleep_alarms(size_t n_alarms, const mp_obj_t *alarms) {
    _setup_sleep_alarms(true, n_alarms, alarms);
}

void nrf_deep_sleep_start(void) {
    _debug_print("go system off..\r\n");
    sd_power_system_off();
}

void NORETURN alarm_enter_deep_sleep(void) {
    alarm_pin_pinalarm_prepare_for_deep_sleep();
    //alarm_touch_touchalarm_prepare_for_deep_sleep();

    nrf_deep_sleep_start();

    // should not reach here..
    while(1) ;
}

void common_hal_alarm_gc_collect(void) {
    void* p = alarm_get_wake_alarm();
    gc_collect_ptr(p);
}
