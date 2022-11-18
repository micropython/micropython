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

#include "shared-bindings/coproc/__init__.h"

#include "py/runtime.h"

#if defined(CONFIG_IDF_TARGET_ESP32S2)
#include "esp32s2/ulp.h"
#include "esp32s2/ulp_riscv.h"
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
#include "esp32s3/ulp.h"
#include "esp32s3/ulp_riscv.h"
#endif

// To-do idf v5.0: remove following include
#include "soc/rtc_cntl_reg.h"

void common_hal_coproc_run(coproc_coproc_obj_t *self) {
    if (GET_PERI_REG_MASK(RTC_CNTL_ULP_CP_TIMER_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN)) {
        mp_raise_RuntimeError(translate("Already running"));
    }

    ulp_riscv_load_binary(self->buf, self->buf_len);
    m_free(self->buf);
    self->buf = (uint8_t *)RTC_SLOW_MEM;

    ulp_riscv_run();
}

void common_hal_coproc_halt(coproc_coproc_obj_t *self) {
    self->buf = (uint8_t *)m_malloc(self->buf_len, false);
    memcpy(self->buf, (uint8_t *)RTC_SLOW_MEM, self->buf_len);

    // To-do idf v5.0: use following functions
    // ulp_riscv_timer_stop();
    // ulp_riscv_halt();

    // stop the ulp timer
    CLEAR_PERI_REG_MASK(RTC_CNTL_ULP_CP_TIMER_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN);
    // suspends the ulp operation
    SET_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_DONE);
    // resets the processor
    SET_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_SHUT_RESET_EN);
}

mp_obj_t common_hal_coproc_memory(coproc_coproc_obj_t *self) {
    return (self->coproc_memory) ? MP_OBJ_FROM_PTR(self->coproc_memory) : mp_const_none;
}
