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

#include "bindings/espulp/ULPAlarm.h"

#include "common-hal/alarm/__init__.h"
#include "supervisor/port.h"

#include "esp_private/rtc_ctrl.h"
#include "soc/rtc_cntl_reg.h"

#include "esp_sleep.h"

static volatile bool woke_up = false;
static bool alarm_set = false;

void common_hal_espulp_ulpalarm_construct(espulp_ulpalarm_obj_t *self, espulp_ulp_obj_t *ulp) {
    self->ulp = ulp;
}

mp_obj_t espulp_ulpalarm_find_triggered_alarm(const size_t n_alarms, const mp_obj_t *alarms) {
    for (size_t i = 0; i < n_alarms; i++) {
        if (mp_obj_is_type(alarms[i], &espulp_ulpalarm_type)) {
            return alarms[i];
        }
    }
    return mp_const_none;
}

mp_obj_t espulp_ulpalarm_record_wake_alarm(void) {
    espulp_ulpalarm_obj_t *const alarm = &alarm_wake_alarm.ulp_alarm;
    alarm->base.type = &espulp_ulpalarm_type;
    return alarm;
}

// This is used to wake the main CircuitPython task.
STATIC void ulp_interrupt(void *arg) {
    (void)arg;
    woke_up = true;
    port_wake_main_task_from_isr();
}

void espulp_ulpalarm_set_alarm(const bool deep_sleep, const size_t n_alarms, const mp_obj_t *alarms) {
    espulp_ulpalarm_obj_t *alarm = MP_OBJ_NULL;

    for (size_t i = 0; i < n_alarms; i++) {
        if (mp_obj_is_type(alarms[i], &espulp_ulpalarm_type)) {
            if (alarm != MP_OBJ_NULL) {
                mp_raise_ValueError_varg(translate("Only one %q can be set."), MP_QSTR_ULPAlarm);
            }
            alarm = MP_OBJ_TO_PTR(alarms[i]);
        }
    }

    if (alarm == MP_OBJ_NULL) {
        return;
    }

    // enable ulp interrupt
    switch (alarm->ulp->arch) {
        #ifdef CONFIG_ULP_COPROC_TYPE_FSM
        case FSM:
            #ifdef CONFIG_IDF_TARGET_ESP32
            rtc_isr_register(&ulp_interrupt, NULL, RTC_CNTL_ULP_CP_INT_RAW, 0);
            #else
            rtc_isr_register(&ulp_interrupt, NULL, RTC_CNTL_ULP_CP_INT_ST, 0);
            #endif
            REG_SET_BIT(RTC_CNTL_INT_ENA_REG, RTC_CNTL_ULP_CP_INT_ENA);
            break;
        #endif
        #ifdef CONFIG_ULP_COPROC_TYPE_RISCV
        case RISCV:
            rtc_isr_register(&ulp_interrupt, NULL, RTC_CNTL_COCPU_INT_ST, 0);
            REG_SET_BIT(RTC_CNTL_INT_ENA_REG, RTC_CNTL_COCPU_INT_ENA);
            break;
        #endif
        default:
            mp_raise_NotImplementedError(NULL);
            break;
    }

    alarm_set = true;
}

void espulp_ulpalarm_prepare_for_deep_sleep(void) {
    if (!alarm_set) {
        return;
    }

    // disable ulp interrupt
    rtc_isr_deregister(&ulp_interrupt, NULL);
    #ifdef CONFIG_ULP_COPROC_TYPE_FSM
    REG_CLR_BIT(RTC_CNTL_INT_ENA_REG, RTC_CNTL_ULP_CP_INT_ENA);
    #endif
    #ifdef CONFIG_ULP_COPROC_TYPE_RISCV
    REG_CLR_BIT(RTC_CNTL_INT_ENA_REG, RTC_CNTL_COCPU_INT_ENA);
    #endif

    // enable ulp wakeup
    esp_sleep_enable_ulp_wakeup();
    #if defined(SOC_PM_SUPPORT_RTC_SLOW_MEM_PD) && SOC_PM_SUPPORT_RTC_SLOW_MEM_PD
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_ON);
    #endif
}

bool espulp_ulpalarm_woke_this_cycle(void) {
    return woke_up;
}

void espulp_ulpalarm_reset(void) {
    woke_up = false;
    alarm_set = false;
}
