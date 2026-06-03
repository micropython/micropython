/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 OpenMV LLC.
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

#include "irq.h"
#include "lptimer.h"
#include "lptimer_ext.h"
#include "sys_ctrl_lptimer.h"

#define LPTIMER ((LPTIMER_Type *)LPTIMER_BASE)
#define LPTIMER_CH_A (0)

void LPTIMER0_IRQHandler(void) {
    lptimer_clear_interrupt(LPTIMER, LPTIMER_CH_A);
}

void lptimer_init(void) {
    // LPTIMER is not reset on CPU reset and may still be active, so reset it now.
    lptimer_cancel_wakeup();
}

void lptimer_set_wakeup(uint64_t timeout_us) {
    lptimer_disable_counter(LPTIMER, LPTIMER_CH_A);

    ANA_REG->MISC_CTRL |= 1U << 0; // SEL_32K, select LXFO

    select_lptimer_clk(LPTIMER_CLK_SOURCE_32K, LPTIMER_CH_A);

    // Maximum 131 second timeout, to not overflow 32-bit ticks when
    // LPTIMER is clocked at 32768Hz.
    if (timeout_us > 131000000ULL) {
        timeout_us = 131000000ULL;
    }
    uint32_t timeout_ticks = timeout_us * 32768ULL / 1000000ULL;

    // Set up the LPTIMER interrupt to fire after the given timeout.
    lptimer_set_mode_userdefined(LPTIMER, LPTIMER_CH_A);
    lptimer_load_count(LPTIMER, LPTIMER_CH_A, &timeout_ticks);
    lptimer_clear_interrupt(LPTIMER, LPTIMER_CH_A);
    lptimer_unmask_interrupt(LPTIMER, LPTIMER_CH_A);

    NVIC_SetPriority(LPTIMER0_IRQ_IRQn, IRQ_PRI_RTC);
    NVIC_ClearPendingIRQ(LPTIMER0_IRQ_IRQn);
    NVIC_EnableIRQ(LPTIMER0_IRQ_IRQn);

    lptimer_enable_counter(LPTIMER, LPTIMER_CH_A);
}

void lptimer_cancel_wakeup(void) {
    lptimer_disable_counter(LPTIMER, LPTIMER_CH_A);
    lptimer_mask_interrupt(LPTIMER, LPTIMER_CH_A);
    lptimer_clear_interrupt(LPTIMER, LPTIMER_CH_A);
}
