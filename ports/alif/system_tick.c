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

#define MIN(x, y) ((x) < (y) ? (x) : (y))

#if MICROPY_HW_SYSTEM_TICK_USE_SYSTICK

#include "shared/runtime/softtimer.h"
#include "pendsv.h"

volatile uint32_t system_tick_ms_counter;

void system_tick_init(void) {
    // Configure SysTick to run at 1kHz (1ms interval)
    SysTick_Config(SystemCoreClock / 1000);
    NVIC_SetPriority(SysTick_IRQn, IRQ_PRI_SYSTEM_TICK);
    NVIC_EnableIRQ(SysTick_IRQn);
}

void SysTick_Handler(void) {
    uint32_t uw_tick = system_tick_ms_counter + 1;
    system_tick_ms_counter = uw_tick;

    // Read the systick control register to clear the COUNTFLAG bit.
    SysTick->CTRL;

    if (soft_timer_next == uw_tick) {
        pendsv_schedule_dispatch(PENDSV_DISPATCH_SOFT_TIMER, soft_timer_handler);
    }
}

uint32_t system_tick_get_u32(void) {
    return system_tick_get_u64();
}

uint64_t system_tick_get_u64(void) {
    mp_uint_t irq_state = disable_irq();
    uint32_t counter = SysTick->VAL;
    uint32_t milliseconds = system_tick_ms_counter;
    uint32_t status = SysTick->CTRL;
    enable_irq(irq_state);

    // It's still possible for the COUNTFLAG bit to get set if the counter was
    // reloaded between reading VAL and reading CTRL.  With interrupts disabled
    // it definitely takes less than 50 cycles between reading VAL and
    // reading CTRL, so the test (counter > 50) is to cover the case where VAL
    // is +ve and very close to zero, and the COUNTFLAG bit is also set.
    if ((status & SysTick_CTRL_COUNTFLAG_Msk) && counter > 50) {
        // This means that the HW reloaded VAL between the time we read VAL and the
        // time we read CTRL, which implies that there is an interrupt pending
        // to increment the tick counter.
        milliseconds++;
    }
    uint32_t load = SysTick->LOAD;
    counter = load - counter; // Convert from decrementing to incrementing

    // Calculate 64-bit microsecond counter.
    return (uint64_t)milliseconds * 1000ULL + (uint64_t)((counter * 1000) / (load + 1));
}

void system_tick_wfe_with_timeout_us(uint32_t timeout_us) {
    if (timeout_us > 1000) {
        // SysTick will wake us in at most 1ms.
        __WFI();
    }
}

#elif MICROPY_HW_SYSTEM_TICK_USE_LPTIMER

#include "lptimer.h"
#include "sys_ctrl_lptimer.h"

// Channel 0 and 1 are cascaded to make a 64-bit counter.
// Channel 2 is used for system_tick_wfe_with_timeout_us.
// Channel 3 is used for system_tick_schedule_after_us.
#define LPTIMER ((LPTIMER_Type *)LPTIMER_BASE)
#define LPTIMER_CH_A (0)
#define LPTIMER_CH_B (1)
#define LPTIMER_CH_C (2)
#define LPTIMER_CH_D (3)

uint64_t system_tick_source_hz;

void system_tick_init(void) {
    lptimer_disable_counter(LPTIMER, LPTIMER_CH_A);
    lptimer_disable_counter(LPTIMER, LPTIMER_CH_B);

    ANA_REG->MISC_CTRL |= 1 << 0; // SEL_32K, select LXFO

    select_lptimer_clk(LPTIMER_CLK_SOURCE_32K, LPTIMER_CH_A);
    select_lptimer_clk(LPTIMER_CLK_SOURCE_CASCADE, LPTIMER_CH_B);
    select_lptimer_clk(LPTIMER_CLK_SOURCE_32K, LPTIMER_CH_C);
    select_lptimer_clk(LPTIMER_CLK_SOURCE_32K, LPTIMER_CH_D);

    lptimer_load_max_count(LPTIMER, LPTIMER_CH_A);
    lptimer_set_mode_freerunning(LPTIMER, LPTIMER_CH_A);

    lptimer_load_max_count(LPTIMER, LPTIMER_CH_B);
    lptimer_set_mode_freerunning(LPTIMER, LPTIMER_CH_B);

    lptimer_enable_counter(LPTIMER, LPTIMER_CH_B);
    lptimer_enable_counter(LPTIMER, LPTIMER_CH_A);

    system_tick_source_hz = 32768;

    NVIC_ClearPendingIRQ(LPTIMER2_IRQ_IRQn);
    NVIC_SetPriority(LPTIMER2_IRQ_IRQn, IRQ_PRI_SYSTEM_TICK);
    NVIC_EnableIRQ(LPTIMER2_IRQ_IRQn);

    NVIC_ClearPendingIRQ(LPTIMER3_IRQ_IRQn);
    NVIC_SetPriority(LPTIMER3_IRQ_IRQn, IRQ_PRI_SYSTEM_TICK);
    NVIC_EnableIRQ(LPTIMER3_IRQ_IRQn);
}

void LPTIMER2_IRQHandler(void) {
    lptimer_clear_interrupt(LPTIMER, LPTIMER_CH_C);
    __SEV();
}

void LPTIMER3_IRQHandler(void) {
    lptimer_clear_interrupt(LPTIMER, LPTIMER_CH_D);
    lptimer_mask_interrupt(LPTIMER, LPTIMER_CH_D);
    lptimer_disable_counter(LPTIMER, LPTIMER_CH_D);
    system_tick_schedule_callback();
    __SEV();
}

uint32_t system_tick_get_u32(void) {
    return 0xffffffff - lptimer_get_count(LPTIMER, LPTIMER_CH_A);
}

uint64_t system_tick_get_u64(void) {
    // Get 64-bit counter value from the hardware timer.
    // Sample it twice in case the low counter wraps around while sampling.
    uint32_t irq_state = disable_irq();
    uint32_t lo0 = lptimer_get_count(LPTIMER, LPTIMER_CH_A);
    uint32_t hi0 = lptimer_get_count(LPTIMER, LPTIMER_CH_B);
    uint32_t lo1 = lptimer_get_count(LPTIMER, LPTIMER_CH_A);
    uint32_t hi1 = lptimer_get_count(LPTIMER, LPTIMER_CH_B);
    enable_irq(irq_state);

    if (hi0 == hi1) {
        // Low counter may have wrapped around between sampling of lo0 and hi0, so prefer second sampling.
        lo0 = lo1;
        hi0 = hi1;
    } else {
        // Low counter wrapped around either between sampling of hi0 and lo1, or sampling of lo1 and hi1.
        // In either case use the first sampling.
    }

    // Convert from descending count to ascending.
    lo0 = 0xffffffff - lo0;
    hi0 = 0xffffffff - hi0;

    // Return a 64-bit value.
    return ((uint64_t)hi0 << 32) | (uint64_t)lo0;
}

void system_tick_wfe_with_timeout_us(uint32_t timeout_us) {
    // Maximum 131 second timeout, to not overflow 32-bit ticks when
    // LPTIMER is clocked at 32768Hz.
    uint32_t timeout_ticks = (uint64_t)MIN(timeout_us, 131000000) * system_tick_source_hz / 1000000;

    // Set up the LPTIMER interrupt to fire after the given timeout.
    lptimer_disable_counter(LPTIMER, LPTIMER_CH_C);
    lptimer_set_mode_userdefined(LPTIMER, LPTIMER_CH_C);
    lptimer_load_count(LPTIMER, LPTIMER_CH_C, &timeout_ticks);
    lptimer_clear_interrupt(LPTIMER, LPTIMER_CH_C);
    lptimer_unmask_interrupt(LPTIMER, LPTIMER_CH_C);
    lptimer_enable_counter(LPTIMER, LPTIMER_CH_C);

    // Wait for an event.
    __WFE();

    // Disable the LPTIMER interrupt (in case a different interrupt woke the WFE).
    lptimer_mask_interrupt(LPTIMER, LPTIMER_CH_C);
    lptimer_disable_counter(LPTIMER, LPTIMER_CH_C);
}

void system_tick_schedule_after_us(uint32_t ticks_us) {
    // Disable the interrupt in case it's still active.
    lptimer_mask_interrupt(LPTIMER, LPTIMER_CH_D);

    // Maximum 131 second timeout, to not overflow 32-bit ticks when
    // LPTIMER is clocked at 32768Hz.
    uint32_t timeout_ticks = (uint64_t)MIN(ticks_us, 131000000) * system_tick_source_hz / 1000000;

    // Set up the LPTIMER interrupt to fire after the given timeout.
    lptimer_disable_counter(LPTIMER, LPTIMER_CH_D);
    lptimer_set_mode_userdefined(LPTIMER, LPTIMER_CH_D);
    lptimer_load_count(LPTIMER, LPTIMER_CH_D, &timeout_ticks);
    lptimer_clear_interrupt(LPTIMER, LPTIMER_CH_D);
    lptimer_unmask_interrupt(LPTIMER, LPTIMER_CH_D);
    lptimer_enable_counter(LPTIMER, LPTIMER_CH_D);
}

#elif MICROPY_HW_SYSTEM_TICK_USE_UTIMER

#include "utimer.h"

#define UTIMER ((UTIMER_Type *)UTIMER_BASE)
#define UTIMER_CHANNEL (11)

uint64_t system_core_clock_mhz;
static volatile uint32_t system_tick_hi;

static void system_tick_nvic_config(unsigned int index) {
    NVIC_ClearPendingIRQ(UTIMER_IRQ0_IRQn + UTIMER_CHANNEL * 8 + index);
    NVIC_SetPriority(UTIMER_IRQ0_IRQn + UTIMER_CHANNEL * 8 + index, IRQ_PRI_SYSTEM_TICK);
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

    // Set up the UTIMER compare A interrupt, to be used by system_tick_wfe_with_timeout_us.
    system_tick_nvic_config(2);
    UTIMER->UTIMER_CHANNEL_CFG[UTIMER_CHANNEL].UTIMER_COMPARE_CTRL_A |= COMPARE_CTRL_DRV_COMPARE_EN;

    // Set up the UTIMER compare B interrupt, to be used by soft-timer.
    system_tick_nvic_config(4);
    UTIMER->UTIMER_CHANNEL_CFG[UTIMER_CHANNEL].UTIMER_COMPARE_CTRL_B |= COMPARE_CTRL_DRV_COMPARE_EN;
}

// COMPARE_A_BUF1
void UTIMER_IRQ90Handler(void) {
    uint32_t chan_interrupt = UTIMER->UTIMER_CHANNEL_CFG[UTIMER_CHANNEL].UTIMER_CHAN_INTERRUPT;
    if (chan_interrupt & CHAN_INTERRUPT_COMPARE_A_BUF1_MASK) {
        utimer_clear_interrupt(UTIMER, UTIMER_CHANNEL, CHAN_INTERRUPT_COMPARE_A_BUF1_MASK);
        __SEV();
    }
}

// COMPARE_B_BUF1
void UTIMER_IRQ92Handler(void) {
    uint32_t chan_interrupt = UTIMER->UTIMER_CHANNEL_CFG[UTIMER_CHANNEL].UTIMER_CHAN_INTERRUPT;
    if (chan_interrupt & CHAN_INTERRUPT_COMPARE_B_BUF1_MASK) {
        utimer_clear_interrupt(UTIMER, UTIMER_CHANNEL, CHAN_INTERRUPT_COMPARE_B_BUF1_MASK);
        utimer_mask_interrupt(UTIMER, UTIMER_CHANNEL, CHAN_INTERRUPT_COMPARE_B_BUF1_MASK);
        system_tick_schedule_callback();
        __SEV();
    }
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

void system_tick_wfe_with_timeout_us(uint32_t timeout_us) {
    // Maximum 10 second timeout, to not overflow 32-bit ticks when
    // system_core_clock_mhz==400.
    uint32_t timeout_ticks = MIN(timeout_us, 10000000) * system_core_clock_mhz;

    // Set up the UTIMER compare interrupt to fire after the given timeout.
    uint32_t cntr = utimer_get_count(UTIMER, UTIMER_CHANNEL, UTIMER_CNTR);
    utimer_set_count(UTIMER, UTIMER_CHANNEL, UTIMER_COMPARE_A_BUF1, cntr + timeout_ticks);
    utimer_clear_interrupt(UTIMER, UTIMER_CHANNEL, CHAN_INTERRUPT_COMPARE_A_BUF1_MASK);
    utimer_unmask_interrupt(UTIMER, UTIMER_CHANNEL, CHAN_INTERRUPT_COMPARE_A_BUF1_MASK);

    // Wait for an event (or compare timeout event) if the timeout hasn't expired yet
    // (this check handles the case of short timeouts).
    uint32_t cntr2 = utimer_get_count(UTIMER, UTIMER_CHANNEL, UTIMER_CNTR);
    if ((uint32_t)(cntr2 - cntr) < timeout_ticks) {
        __WFE();
    }

    // Disable the UTIMER compare interrupt.
    utimer_mask_interrupt(UTIMER, UTIMER_CHANNEL, CHAN_INTERRUPT_COMPARE_A_BUF1_MASK);
}

void system_tick_schedule_after_us(uint32_t ticks_us) {
    // Disable the interrupt in case it's still active.
    utimer_mask_interrupt(UTIMER, UTIMER_CHANNEL, CHAN_INTERRUPT_COMPARE_B_BUF1_MASK);

    // Maximum 10 second timeout, to not overflow 32-bit ticks when
    // system_core_clock_mhz==400.
    uint32_t timeout_ticks = MIN(ticks_us, 10000000) * system_core_clock_mhz;

    // Set up the UTIMER compare interrupt to fire after the given timeout.
    uint32_t cntr = utimer_get_count(UTIMER, UTIMER_CHANNEL, UTIMER_CNTR);
    utimer_set_count(UTIMER, UTIMER_CHANNEL, UTIMER_COMPARE_B_BUF1, cntr + timeout_ticks);
    utimer_clear_interrupt(UTIMER, UTIMER_CHANNEL, CHAN_INTERRUPT_COMPARE_B_BUF1_MASK);
    utimer_unmask_interrupt(UTIMER, UTIMER_CHANNEL, CHAN_INTERRUPT_COMPARE_B_BUF1_MASK);

    // Handle the case of short timeouts.
    uint32_t cntr2 = utimer_get_count(UTIMER, UTIMER_CHANNEL, UTIMER_CNTR);
    if ((uint32_t)(cntr2 - cntr) >= timeout_ticks) {
        if (!(UTIMER->UTIMER_CHANNEL_CFG[UTIMER_CHANNEL].UTIMER_CHAN_INTERRUPT_MASK & CHAN_INTERRUPT_COMPARE_B_BUF1_MASK)) {
            // Interrupt is still enabled, so disable it and manually call the callback.
            utimer_mask_interrupt(UTIMER, UTIMER_CHANNEL, CHAN_INTERRUPT_COMPARE_B_BUF1_MASK);
            system_tick_schedule_callback();
        }
    }
}

#endif
