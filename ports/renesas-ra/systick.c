/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2021 Renesas Electronics Corporation
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

#include "py/runtime.h"
#include "py/mphal.h"
#include "shared/runtime/softtimer.h"
#include "irq.h"
#include "pendsv.h"
#include "systick.h"
#include "pybthread.h"
#include "hal_data.h"

volatile uint32_t uwTick;

uint32_t HAL_GetTick(void) {
    return uwTick;
}

systick_dispatch_t systick_dispatch_table[SYSTICK_DISPATCH_NUM_SLOTS];

void SysTick_Handler(void) {
    // Instead of calling HAL_IncTick we do the increment here of the counter.
    // This is purely for efficiency, since SysTick is called 1000 times per
    // second at the highest interrupt priority.
    uint32_t uw_tick = uwTick + 1;
    uwTick = uw_tick;

    // Read the systick control register. This has the side effect of clearing
    // the COUNTFLAG bit, which makes the logic in mp_hal_ticks_us
    // work properly.
    SysTick->CTRL;

    // Dispatch to any registered handlers in a cycle
    systick_dispatch_t f = systick_dispatch_table[uw_tick & (SYSTICK_DISPATCH_NUM_SLOTS - 1)];
    if (f != NULL) {
        f(uw_tick);
    }

    if (soft_timer_next == uw_tick) {
        pendsv_schedule_dispatch(PENDSV_DISPATCH_SOFT_TIMER, soft_timer_handler);
    }

    #if MICROPY_PY_THREAD
    if (pyb_thread_enabled) {
        if (pyb_thread_cur->timeslice == 0) {
            if (pyb_thread_cur->run_next != pyb_thread_cur) {
                SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
            }
        } else {
            --pyb_thread_cur->timeslice;
        }
    }
    #endif
}

// We provide our own version of HAL_Delay that calls __WFI while waiting,
// and works when interrupts are disabled.  This function is intended to be
// used only by the ST HAL functions.
void HAL_Delay(uint32_t Delay) {
    if (query_irq() == IRQ_STATE_ENABLED) {
        // IRQs enabled, so can use systick counter to do the delay
        uint32_t start = uwTick;
        // Wraparound of tick is taken care of by 2's complement arithmetic.
        while (uwTick - start < Delay) {
            // Enter sleep mode, waiting for (at least) the SysTick interrupt.
            __WFI();
        }
    } else {
        // IRQs disabled, use mp_hal_delay_ms routine.
        mp_hal_delay_ms(Delay);
    }
}

// Core delay function that does an efficient sleep and may switch thread context.
// If IRQs are enabled then we must have the GIL.
void mp_hal_delay_ms(mp_uint_t Delay) {
    if (query_irq() == IRQ_STATE_ENABLED) {
        // IRQs enabled, so can use systick counter to do the delay
        uint32_t start = uwTick;
        // Wraparound of tick is taken care of by 2's complement arithmetic.
        do {
            // This macro will execute the necessary idle behaviour.  It may
            // raise an exception, switch threads or enter sleep mode (waiting for
            // (at least) the SysTick interrupt).
            MICROPY_EVENT_POLL_HOOK
        } while (uwTick - start < Delay);
    } else {
        // IRQs disabled, so need to use a busy loop for the delay.
        // To prevent possible overflow of the counter we use a double loop.
        volatile uint32_t count_1ms;
        while (Delay-- > 0) {
            count_1ms = (MICROPY_HW_MCU_PCLK / 1000 / 10);
            while (count_1ms-- > 0) {
                __asm__ __volatile__ ("nop");
            }
        }
    }
}

// delay for given number of microseconds
void mp_hal_delay_us(mp_uint_t usec) {
    if (query_irq() == IRQ_STATE_ENABLED) {
        // IRQs enabled, so can use systick counter to do the delay
        uint32_t start = mp_hal_ticks_us();
        while (mp_hal_ticks_us() - start < usec) {
        }
    } else {
        // IRQs disabled, so need to use a busy loop for the delay
        volatile uint32_t ucount = (MICROPY_HW_MCU_PCLK / 1000000 / 10) * usec;
        while (ucount-- > 0) {
            __asm__ __volatile__ ("nop");
        }
    }
}

bool systick_has_passed(uint32_t start_tick, uint32_t delay_ms) {
    return HAL_GetTick() - start_tick >= delay_ms;
}

// waits until at least delay_ms milliseconds have passed from the sampling of
// startTick. Handles overflow properly. Assumes stc was taken from
// HAL_GetTick() some time before calling this function.
void systick_wait_at_least(uint32_t start_tick, uint32_t delay_ms) {
    while (!systick_has_passed(start_tick, delay_ms)) {
        __WFI(); // enter sleep mode, waiting for interrupt
    }
}

mp_uint_t mp_hal_ticks_ms(void) {
    return uwTick;
}

// The SysTick timer counts down at 168 MHz, so we can use that knowledge
// to grab a microsecond counter.
//
// We assume that HAL_GetTickis returns milliseconds.
mp_uint_t mp_hal_ticks_us(void) {
    mp_uint_t irq_state = disable_irq();
    uint32_t counter = SysTick->VAL;
    uint32_t milliseconds = HAL_GetTick();
    uint32_t status = SysTick->CTRL;
    enable_irq(irq_state);

    // It's still possible for the countflag bit to get set if the counter was
    // reloaded between reading VAL and reading CTRL. With interrupts  disabled
    // it definitely takes less than 50 HCLK cycles between reading VAL and
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

    // ((load + 1) / 1000) is the number of counts per microsecond.
    //
    // counter / ((load + 1) / 1000) scales from the systick clock to microseconds
    // and is the same thing as (counter * 1000) / (load + 1)
    return milliseconds * 1000 + (counter * 1000) / (load + 1);
}
