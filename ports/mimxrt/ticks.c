/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Damien P. George
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

#include "py/mphal.h"
#include "ticks.h"
#include "fsl_gpt.h"

// General purpose timer for keeping microsecond and millisecond tick values.
#define GPTx GPT2
#define GPTx_IRQn GPT2_IRQn
#define GPTx_IRQHandler GPT2_IRQHandler

static uint32_t ticks_us64_upper;
static uint32_t ticks_ms_upper;

void ticks_init(void) {
    ticks_us64_upper = 0;
    ticks_ms_upper = 0;

    gpt_config_t config;
    config.clockSource = kGPT_ClockSource_Osc;
    config.divider = 24; // XTAL is 24MHz
    config.enableFreeRun = true;
    config.enableRunInWait = true;
    config.enableRunInStop = true;
    config.enableRunInDoze = true;
    config.enableRunInDbg = false;
    config.enableMode = true;
    GPT_Init(GPTx, &config);

    GPT_EnableInterrupts(GPTx, kGPT_RollOverFlagInterruptEnable);
    NVIC_SetPriority(GPTx_IRQn, 0); // highest priority
    NVIC_EnableIRQ(GPTx_IRQn);

    GPT_StartTimer(GPTx);
}

void GPTx_IRQHandler(void) {
    if (GPT_GetStatusFlags(GPTx, kGPT_OutputCompare1Flag)) {
        GPT_ClearStatusFlags(GPTx, kGPT_OutputCompare1Flag);
        GPT_DisableInterrupts(GPTx, kGPT_OutputCompare1InterruptEnable);
        __SEV();
    }
    if (GPT_GetStatusFlags(GPTx, kGPT_RollOverFlag)) {
        GPT_ClearStatusFlags(GPTx, kGPT_RollOverFlag);
        ++ticks_us64_upper;
        if (++ticks_ms_upper >= 1000) {
            // Wrap upper counter at a multiple of 1000 so that when mp_hal_ticks_ms()
            // wraps due to overflow it wraps smoothly.
            ticks_ms_upper = 0;
        }
    }
}

static void ticks_wake_after_us32(uint32_t us) {
    if (us < 2) {
        // Delay too short to guarantee that we won't miss it when setting the OCR below.
        __SEV();
    } else {
        // Disable IRQs so setting the OCR is done without any interruption.
        uint32_t irq_state = DisableGlobalIRQ();
        GPT_EnableInterrupts(GPTx, kGPT_OutputCompare1InterruptEnable);
        uint32_t oc = GPT_GetCurrentTimerCount(GPTx) + us;
        GPT_SetOutputCompareValue(GPTx, kGPT_OutputCompare_Channel1, oc);
        EnableGlobalIRQ(irq_state);
    }
}

static uint64_t ticks_us64_with(uint32_t *upper_ptr) {
    uint32_t irq_state = DisableGlobalIRQ();
    uint32_t lower = GPT_GetCurrentTimerCount(GPTx);
    uint32_t upper = *upper_ptr;
    uint32_t overflow = GPT_GetStatusFlags(GPTx, kGPT_RollOverFlag);
    EnableGlobalIRQ(irq_state);
    if (overflow && lower < 0x80000000) {
        // The timer counter overflowed before reading it but the IRQ handler
        // has not yet been called, so perform the IRQ arithmetic now.
        ++upper;
    }
    return (uint64_t)upper << 32 | (uint64_t)lower;
}

uint32_t ticks_us32(void) {
    return GPT_GetCurrentTimerCount(GPTx);
}

uint64_t ticks_us64(void) {
    return ticks_us64_with(&ticks_us64_upper);
}

uint32_t ticks_ms32(void) {
    // This will return a value that only has the lower 32-bits valid.
    return ticks_us64_with(&ticks_ms_upper) / 1000;
}

void ticks_delay_us64(uint64_t us) {
    uint64_t t0 = ticks_us64();
    for (;;) {
        uint64_t dt = ticks_us64() - t0;
        if (dt >= us) {
            return;
        }
        dt = us - dt;
        if (dt > 0xffffffff) {
            dt = 0xffffffff;
        }
        ticks_wake_after_us32((uint32_t)dt);
        if (dt < 50) {
            __WFE();
        } else {
            MICROPY_EVENT_POLL_HOOK
        }
    }
}
