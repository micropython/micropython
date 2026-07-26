/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Ibrahim Abdelkader <iabdalkader@openmv.io>
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

#include <stdint.h>

#include "py/mphal.h"
#include "shared/runtime/softtimer.h"

// CPU frequency
#ifndef CPU_FREQ_HZ
#define CPU_FREQ_HZ             25000000u
#endif

// SysTick registers
#define SYSTICK_BASE            0xE000E010
#define SYSTICK_CSR             (*(volatile uint32_t *)(SYSTICK_BASE + 0x00))
#define SYSTICK_RVR             (*(volatile uint32_t *)(SYSTICK_BASE + 0x04))
#define SYSTICK_CVR             (*(volatile uint32_t *)(SYSTICK_BASE + 0x08))

// SysTick config bits
#define SYSTICK_CSR_ENABLE      (1 << 0)
#define SYSTICK_CSR_TICKINT     (1 << 1)
#define SYSTICK_CSR_CLKSOURCE   (1 << 2)

// System handler priority register 3 (PendSV = PRI_14, SysTick = PRI_15).
#define SCB_SHPR3               (*(volatile uint32_t *)0xE000ED20)

// Exposed to the soft timer as MICROPY_SOFT_TIMER_TICKS_MS.
volatile uint32_t _ticks_ms = 0;
#if defined(__ARM_ARCH_ISA_ARM)
static volatile uint32_t _ticks_us = 0;
#endif

void ticks_init(void) {
    #if !defined(__ARM_ARCH_ISA_ARM)
    // Configure SysTick: reload at 1ms intervals (Cortex-M)
    SYSTICK_CVR = 0;
    SYSTICK_RVR = (CPU_FREQ_HZ / 1000) - 1;
    SYSTICK_CSR = SYSTICK_CSR_ENABLE | SYSTICK_CSR_TICKINT | SYSTICK_CSR_CLKSOURCE;
    // Set PendSV interrupt at the lowest priority, and keep SysTick at the
    // highest (its reset default), so that raising BASEPRI to mask PendSV
    // (MICROPY_PY_PENDSV_ENTER) does not also mask SysTick.  This port has no
    // CMSIS NVIC_SetPriority(), so write the system handler priority register
    // directly; the reset priority grouping (all bits are group priority) is
    // used.
    SCB_SHPR3 = (SCB_SHPR3 & ~((0xffu << 16) | (0xffu << 24))) | (IRQ_PRI_PENDSV << 16);
    #endif
}

uintptr_t ticks_ms(void) {
    #if defined(__ARM_ARCH_ISA_ARM)
    return _ticks_us++ / 1000;
    #else
    return _ticks_ms;
    #endif
}

uintptr_t ticks_us(void) {
    #if defined(__ARM_ARCH_ISA_ARM)
    return _ticks_us++;
    #else
    // Get current values atomically
    uint32_t ms = _ticks_ms;
    uint32_t cvr = SYSTICK_CVR;

    // Calculate microseconds from the down-counter
    uint32_t elapsed_cycles = SYSTICK_RVR - cvr;
    uint32_t us_fraction = (elapsed_cycles * 1000) / (CPU_FREQ_HZ / 1000);

    return (ms * 1000) + us_fraction;
    #endif
}

#ifdef MICROPY_SOFT_TIMER_TICKS_MS
// Interrupt Control and State Register (ICSR), used to pend a PendSV.
#define SCB_ICSR            (*(volatile uint32_t *)0xE000ED04)
#define SCB_ICSR_PENDSVSET  (1 << 28)

// Run the soft-timer handler from PendSV, pended by SysTick below.  This
// overrides the default (fault-catching) PendSV_Handler from errorhandler.c.
void PendSV_Handler(void) {
    soft_timer_handler();
}
#endif

void SysTick_Handler(void) {
    uint32_t tick = _ticks_ms + 1;
    _ticks_ms = tick;

    #ifdef MICROPY_SOFT_TIMER_TICKS_MS
    if (soft_timer_next == tick) {
        // Pend a PendSV to run the soft-timer handler.
        SCB_ICSR = SCB_ICSR_PENDSVSET;
    }
    #endif
}
