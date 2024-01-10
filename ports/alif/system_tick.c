/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 OpenMV LLC.
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
#include "system_tick.h"

#include "utimer.h"

#define UTIMER ((UTIMER_Type *)UTIMER_BASE)
#define UTIMER_CHANNEL (11)

uint64_t system_core_clock_mhz;
static volatile uint32_t system_tick_hi;

static void system_tick_nvic_config(unsigned int index) {
    NVIC_ClearPendingIRQ(UTIMER_IRQ0_IRQn + UTIMER_CHANNEL * 8 + index);
    NVIC_SetPriority(UTIMER_IRQ0_IRQn + UTIMER_CHANNEL * 8 + index, 0);
    NVIC_EnableIRQ(UTIMER_IRQ0_IRQn + UTIMER_CHANNEL * 8 + index);
}

void system_tick_init(void) {
    system_tick_hi = 0;
    system_core_clock_mhz = SystemCoreClock / 1000000;

    // Configure NVIC OVER_FLOW interrupt.
    system_tick_nvic_config(7);

    utimer_clock_enable(UTIMER, UTIMER_CHANNEL);
    utimer_channel_config cfg = { 0 };
    cfg.fixed_buffer = false;
    utimer_config_direction(UTIMER, UTIMER_CHANNEL, UTIMER_COUNTER_UP, &cfg);
    utimer_set_count(UTIMER, UTIMER_CHANNEL, UTIMER_CNTR_PTR, 0xffffffff);
    utimer_unmask_interrupt(UTIMER, UTIMER_CHANNEL, CHAN_INTERRUPT_OVER_FLOW_MASK);
    utimer_control_enable(UTIMER, UTIMER_CHANNEL);
    utimer_counter_start(UTIMER, UTIMER_CHANNEL);
}

// OVER_FLOW
void UTIMER_IRQ95Handler(void) {
    uint32_t chan_interrupt = UTIMER->UTIMER_CHANNEL_CFG[UTIMER_CHANNEL].UTIMER_CHAN_INTERRUPT;
    if (chan_interrupt & CHAN_INTERRUPT_OVER_FLOW_MASK) {
        utimer_clear_interrupt(UTIMER, UTIMER_CHANNEL, CHAN_INTERRUPT_OVER_FLOW_MASK);
        ++system_tick_hi;
    }
}

uint32_t system_tick_get_u32(void) {
    return utimer_get_count(UTIMER, UTIMER_CHANNEL, UTIMER_CNTR);
}

uint64_t system_tick_get_u64(void) {
    uint32_t irq_state = disable_irq();
    uint32_t ticks_lo = utimer_get_count(UTIMER, UTIMER_CHANNEL, UTIMER_CNTR);
    uint32_t ticks_hi = system_tick_hi;
    uint32_t chan_interrupt = UTIMER->UTIMER_CHANNEL_CFG[UTIMER_CHANNEL].UTIMER_CHAN_INTERRUPT;
    enable_irq(irq_state);

    if (chan_interrupt & CHAN_INTERRUPT_OVER_FLOW_MASK) {
        // The timer had an overflow while interrupts were disabled.
        if (ticks_lo < 0x80000000) {
            // The timer had an overflow just before we sampled it.
            ++ticks_hi;
        }
    }

    // This ticks runs at SystemCoreClock.
    return (uint64_t)ticks_hi << 32 | ticks_lo;
}
