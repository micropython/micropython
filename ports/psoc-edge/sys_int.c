/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Infineon Technologies AG
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

#include "sys_int.h"
#include "py/runtime.h"

#define sys_int_assert_raise_val(msg, ret)   if (ret != CY_SYSINT_SUCCESS) { \
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT(msg), ret); \
}

void sys_int_init(sys_int_cfg_t *int_cfg) {
    NVIC_ClearPendingIRQ(int_cfg->irq_num);
    NVIC_DisableIRQ(int_cfg->irq_num);

    cy_stc_sysint_t intr_cfg = {
        int_cfg->irq_num,
        int_cfg->priority
    };

    cy_en_sysint_status_t rslt = Cy_SysInt_Init(&intr_cfg, int_cfg->handler);
    sys_int_assert_raise_val("System interrupt init failed (PSE PDL error code: %lx)", rslt);

    NVIC_EnableIRQ(int_cfg->irq_num);
}

void sys_int_deinit(sys_int_cfg_t *int_cfg) {
    NVIC_ClearPendingIRQ(int_cfg->irq_num);
    NVIC_DisableIRQ(int_cfg->irq_num);
    int_cfg->priority = SYS_INT_IRQ_LOWEST_PRIORITY;
}

void sys_int_enable(sys_int_cfg_t *int_cfg) {
    NVIC_EnableIRQ(int_cfg->irq_num);
}

void sys_int_disable(sys_int_cfg_t *int_cfg) {
    NVIC_DisableIRQ(int_cfg->irq_num);
}

void sys_int_clear(sys_int_cfg_t *int_cfg) {
    NVIC_ClearPendingIRQ(int_cfg->irq_num);
}
