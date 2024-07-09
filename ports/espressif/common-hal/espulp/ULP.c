// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 microDev
//
// SPDX-License-Identifier: MIT

#include "bindings/espulp/__init__.h"
#include "bindings/espulp/ULP.h"
#include "bindings/espidf/__init__.h"

#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "esp_sleep.h"

#if defined(CONFIG_IDF_TARGET_ESP32)
#include "esp32/ulp.h"
#define ULP_COPROC_RESERVE_MEM (CONFIG_ESP32_ULP_COPROC_RESERVE_MEM)
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
#include "esp32s2/ulp.h"
#include "ulp_riscv.h"
#define ULP_COPROC_RESERVE_MEM (CONFIG_ESP32S2_ULP_COPROC_RESERVE_MEM)
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
#include "esp32s3/ulp.h"
#include "ulp_riscv.h"
#define ULP_COPROC_RESERVE_MEM (CONFIG_ESP32S3_ULP_COPROC_RESERVE_MEM)
#endif

// To-do idf v5.0: remove following include
#include "soc/rtc_cntl_reg.h"

static bool ulp_used = false;
static uint32_t pins_used = 0;

void espulp_reset(void) {
    // NOTE: This *doesn't* disable the ULP. It'll keep running even when CircuitPython isn't.
    ulp_used = false;
}

void common_hal_espulp_ulp_set_wakeup_period(espulp_ulp_obj_t *self, size_t period_index, uint32_t period_us) {
    CHECK_ESP_RESULT(ulp_set_wakeup_period(period_index, period_us));
}

void common_hal_espulp_ulp_run(espulp_ulp_obj_t *self, uint32_t *program, size_t length, uint32_t entry_point, uint32_t pin_mask) {
    if (length > CONFIG_ULP_COPROC_RESERVE_MEM) {
        mp_raise_ValueError(MP_ERROR_TEXT("Program too long"));
    }

    if (
        #ifdef CONFIG_IDF_TARGET_ESP32
        GET_PERI_REG_MASK(RTC_CNTL_STATE0_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN)
        #else
        GET_PERI_REG_MASK(RTC_CNTL_ULP_CP_TIMER_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN)
        #endif
        ) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Already running"));
    }

    if (pin_mask >= (1 << 22)) {
        raise_ValueError_invalid_pin();
    }

    for (uint8_t i = 0; i < 32; i++) {
        if ((pin_mask & (1 << i)) != 0 && !pin_number_is_free(i)) {
            mp_raise_ValueError_varg(MP_ERROR_TEXT("%q in use"), MP_QSTR_Pin);
        }
    }

    for (uint8_t i = 0; i < 32; i++) {
        if ((pin_mask & (1 << i)) != 0) {
            claim_pin_number(i);
            never_reset_pin_number(i);
        }
    }
    pins_used = pin_mask;

    // Main purpose of ULP is to run while main cpu is in deep sleep, so
    // ensure GPIO Power Domain remains enabled during deep sleep,
    // if any GPIO were supplied here.
    if (pins_used > 0) {
        esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    }

    esp_err_t result;
    switch (self->arch) {
        #ifdef CONFIG_ULP_COPROC_TYPE_FSM
        case FSM:
            result = ulp_load_binary(0, (const uint8_t *)program, length / sizeof(uint32_t));
            if (result != ESP_OK) {
                mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q"), MP_QSTR_program);
            }
            CHECK_ESP_RESULT(ulp_run(entry_point / sizeof(uint32_t)));
            break;
        #endif
        #ifdef CONFIG_ULP_COPROC_TYPE_RISCV
        case RISCV:
            result = ulp_riscv_load_binary((const uint8_t *)program, length);
            if (result != ESP_OK) {
                mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q"), MP_QSTR_program);
            }
            CHECK_ESP_RESULT(ulp_riscv_run());
            break;
        #endif
        default:
            mp_raise_NotImplementedError(NULL);
            break;
    }
}

void common_hal_espulp_ulp_halt(espulp_ulp_obj_t *self) {
    switch (self->arch) {
        #ifdef CONFIG_ULP_COPROC_TYPE_FSM
        case FSM:
            ulp_timer_stop();
            break;
        #endif
        #ifdef CONFIG_ULP_COPROC_TYPE_RISCV
        case RISCV:
            ulp_riscv_timer_stop();
            ulp_riscv_halt();
            break;
        #endif
        default:
            mp_raise_NotImplementedError(NULL);
            break;
    }

    // Release pins we were using.
    for (uint8_t i = 0; i < 32; i++) {
        if ((pins_used & (1 << i)) != 0) {
            reset_pin_number(i);
        }
    }
}

void common_hal_espulp_ulp_construct(espulp_ulp_obj_t *self, espulp_architecture_t arch) {
    // Use a static variable to track ULP in use so that subsequent code runs can
    // use a running ULP. This is only to prevent multiple portions of user code
    // from using the ULP concurrently.
    if (ulp_used) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q in use"), MP_QSTR_ULP);
    }

    switch (arch) {
        #ifdef CONFIG_ULP_COPROC_TYPE_FSM
        case FSM:
            break;
        #endif
        #ifdef CONFIG_ULP_COPROC_TYPE_RISCV
        case RISCV:
            break;
        #endif
        default:
            mp_raise_NotImplementedError(NULL);
            break;
    }

    self->arch = arch;
    self->inited = true;
}

bool common_hal_espulp_ulp_deinited(espulp_ulp_obj_t *self) {
    return !self->inited;
}

void common_hal_espulp_ulp_deinit(espulp_ulp_obj_t *self) {
    if (common_hal_espulp_ulp_deinited(self)) {
        return;
    }
    common_hal_espulp_ulp_halt(self);
    self->inited = false;
    ulp_used = false;
}
