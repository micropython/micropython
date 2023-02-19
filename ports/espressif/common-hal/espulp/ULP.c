/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include "bindings/espulp/__init__.h"
#include "bindings/espulp/ULP.h"

#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"

#if defined(CONFIG_IDF_TARGET_ESP32)
#include "esp32/ulp.h"
#define ULP_COPROC_RESERVE_MEM (CONFIG_ESP32_ULP_COPROC_RESERVE_MEM)
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
#include "esp32s2/ulp.h"
#include "esp32s2/ulp_riscv.h"
#define ULP_COPROC_RESERVE_MEM (CONFIG_ESP32S2_ULP_COPROC_RESERVE_MEM)
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
#include "esp32s3/ulp.h"
#include "esp32s3/ulp_riscv.h"
#define ULP_COPROC_RESERVE_MEM (CONFIG_ESP32S3_ULP_COPROC_RESERVE_MEM)
#endif

// To-do idf v5.0: remove following include
#include "soc/rtc_cntl_reg.h"

STATIC bool ulp_used = false;
STATIC uint32_t pins_used = 0;

void espulp_reset(void) {
    // NOTE: This *doesn't* disable the ULP. It'll keep running even when CircuitPython isn't.
    ulp_used = false;
}

void common_hal_espulp_ulp_run(espulp_ulp_obj_t *self, uint32_t *program, size_t length, uint32_t pin_mask) {
    if (length > ULP_COPROC_RESERVE_MEM) {
        mp_raise_ValueError(translate("Program too long"));
    }

    if (
        #ifdef CONFIG_IDF_TARGET_ESP32
        GET_PERI_REG_MASK(RTC_CNTL_STATE0_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN)
        #else
        GET_PERI_REG_MASK(RTC_CNTL_ULP_CP_TIMER_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN)
        #endif
        ) {
        mp_raise_RuntimeError(translate("Already running"));
    }

    if (pin_mask >= (1 << 22)) {
        raise_ValueError_invalid_pin();
    }

    for (uint8_t i = 0; i < 32; i++) {
        if ((pin_mask & (1 << i)) != 0 && !pin_number_is_free(i)) {
            mp_raise_ValueError_varg(translate("%q in use"), MP_QSTR_Pin);
        }
    }

    for (uint8_t i = 0; i < 32; i++) {
        if ((pin_mask & (1 << i)) != 0) {
            claim_pin_number(i);
            never_reset_pin_number(i);
        }
    }
    pins_used = pin_mask;

    ulp_set_wakeup_period(0, 20000);

    switch (self->arch) {
        case FSM:
            ulp_load_binary(0, (const uint8_t *)program, length);
            ulp_run(0);
            break;
        case RISCV:
            #ifndef CONFIG_IDF_TARGET_ESP32
            ulp_riscv_load_binary((const uint8_t *)program, length);
            ulp_riscv_run();
            break;
            #endif
        default:
            mp_raise_NotImplementedError(NULL);
            break;
    }
}

void common_hal_espulp_ulp_halt(espulp_ulp_obj_t *self) {
    #ifdef CONFIG_IDF_TARGET_ESP32
    mp_raise_NotImplementedError(NULL);
    #else
    // To-do idf v5.0: use following functions
    // ulp_riscv_timer_stop();
    // ulp_riscv_halt();

    // stop the ulp timer so that it doesn't restart the cpu
    CLEAR_PERI_REG_MASK(RTC_CNTL_ULP_CP_TIMER_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN);

    // suspends the ulp operation
    SET_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_DONE);

    // resets the processor
    SET_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_SHUT_RESET_EN);
    #endif

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
        mp_raise_ValueError_varg(translate("%q in use"), MP_QSTR_ULP);
    }

    #ifdef CONFIG_IDF_TARGET_ESP32
    if (self->arch == RISCV) {
        mp_raise_NotImplementedError(NULL);
    }
    #endif

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
