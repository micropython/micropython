/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 microDev
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

#include "shared-bindings/alarm/coproc/CoprocAlarm.h"
#include "shared-bindings/coproc/__init__.h"

#if CIRCUITPY_COPROC

#include "supervisor/port.h"

#include "driver/rtc_cntl.h"
#include "soc/rtc_cntl_reg.h"

#include "esp_sleep.h"

static volatile bool woke_up = false;

mp_obj_t alarm_coproc_coprocalarm_find_triggered_alarm(const size_t n_alarms, const mp_obj_t *alarms) {
    for (size_t i = 0; i < n_alarms; i++) {
        if (mp_obj_is_type(alarms[i], &alarm_coproc_coprocalarm_type)) {
            return alarms[i];
        }
    }
    return mp_const_none;
}

mp_obj_t alarm_coproc_coprocalarm_create_wakeup_alarm(void) {
    // Create CoprocAlarm object.
    alarm_coproc_coprocalarm_obj_t *alarm = m_new_obj(alarm_coproc_coprocalarm_obj_t);
    alarm->base.type = &alarm_coproc_coprocalarm_type;
    return alarm;
}

// This is used to wake the main CircuitPython task.
STATIC void coproc_interrupt(void *arg) {
    (void)arg;
    woke_up = true;
    port_wake_main_task_from_isr();
}

void alarm_coproc_coprocalarm_set_alarm(const bool deep_sleep, const size_t n_alarms, const mp_obj_t *alarms) {
    bool coproc_alarm_set = false;
    alarm_coproc_coprocalarm_obj_t *coproc_alarm = MP_OBJ_NULL;

    for (size_t i = 0; i < n_alarms; i++) {
        if (mp_obj_is_type(alarms[i], &alarm_coproc_coprocalarm_type)) {
            if (deep_sleep && coproc_alarm_set) {
                mp_raise_ValueError_varg(translate("Only one %q can be set in deep sleep."), MP_QSTR_CoprocAlarm);
            }
            coproc_alarm = MP_OBJ_TO_PTR(alarms[i]);
            coproc_alarm_set = true;
        }
    }

    if (!coproc_alarm_set) {
        return;
    }

    // enable coproc interrupt
    rtc_isr_register(&coproc_interrupt, NULL, RTC_CNTL_COCPU_INT_ST);
    REG_SET_BIT(RTC_CNTL_INT_ENA_REG, RTC_CNTL_COCPU_INT_ENA);

    // start coproc program
    common_hal_coproc_run(coproc_alarm->coproc);
}

void alarm_coproc_coprocalarm_prepare_for_deep_sleep(void) {
    // disbale coproc interrupt
    rtc_isr_deregister(&coproc_interrupt, NULL);
    REG_CLR_BIT(RTC_CNTL_INT_ENA_REG, RTC_CNTL_COCPU_INT_ENA);

    // enable coproc wakeup
    esp_sleep_enable_ulp_wakeup();
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_ON);
}

bool alarm_coproc_coprocalarm_woke_this_cycle(void) {
    return woke_up;
}

void alarm_coproc_coprocalarm_reset(void) {
    woke_up = false;
}

#else // CIRCUITPY_COPROC

mp_obj_t alarm_coproc_coprocalarm_find_triggered_alarm(const size_t n_alarms, const mp_obj_t *alarms) {
    return mp_const_none;
}

mp_obj_t alarm_coproc_coprocalarm_create_wakeup_alarm(void) {
    return mp_const_none;
}

void alarm_coproc_coprocalarm_set_alarm(const bool deep_sleep, const size_t n_alarms, const mp_obj_t *alarms) {
}

void alarm_coproc_coprocalarm_prepare_for_deep_sleep(void) {
}

bool alarm_coproc_coprocalarm_woke_this_cycle(void) {
    return false;
}

void alarm_coproc_coprocalarm_reset(void) {
}

#endif // CIRCUITPY_COPROC
