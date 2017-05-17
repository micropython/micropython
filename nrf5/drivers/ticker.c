/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Mark Shannon
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

#if MICROPY_PY_MACHINE_SOFT_PWM

#include "ticker.h"
#include "hal_irq.h"

#define FastTicker NRF_TIMER0
#define FastTicker_IRQn TIMER0_IRQn
#define FastTicker_IRQHandler TIMER0_IRQHandler

#define SlowTicker_IRQn SWI0_IRQn
#define SlowTicker_IRQHandler SWI0_IRQHandler

// Ticker callback function called every MACRO_TICK
static callback_ptr slow_ticker;

void ticker_init(callback_ptr slow_ticker_callback) {
    slow_ticker = slow_ticker_callback;

    NRF_TIMER_Type *ticker = FastTicker;
#if NRF51
    ticker->POWER = 1;
#endif
    __NOP();
    ticker_stop();
    ticker->TASKS_CLEAR = 1;
    ticker->CC[3] = MICROSECONDS_PER_MACRO_TICK;
    ticker->MODE = TIMER_MODE_MODE_Timer;
    ticker->BITMODE = TIMER_BITMODE_BITMODE_24Bit << TIMER_BITMODE_BITMODE_Pos;
    ticker->PRESCALER = 4; // 1 tick == 1 microsecond
    ticker->INTENSET = TIMER_INTENSET_COMPARE3_Msk;
    ticker->SHORTS = 0;

    hal_irq_priority(FastTicker_IRQn, 1);
    hal_irq_priority(SlowTicker_IRQn, 3);
    hal_irq_enable(SlowTicker_IRQn);
}

/* Start and stop timer 0 including workarounds for Anomaly 73 for Timer
* http://www.nordicsemi.com/eng/content/download/29490/494569/file/nRF51822-PAN%20v3.0.pdf
*/
void ticker_start(void) {
    hal_irq_enable(FastTicker_IRQn);
    *(uint32_t *)0x40008C0C = 1; //for Timer 0
    FastTicker->TASKS_START = 1;
}

void ticker_stop(void) {
    hal_irq_disable(FastTicker_IRQn);
    FastTicker->TASKS_STOP = 1;
    *(uint32_t *)0x40008C0C = 0; //for Timer 0
}

int32_t noop(void) {
    return -1;
}

uint32_t ticks;

static ticker_callback_ptr callbacks[3] = { noop, noop, noop };

void FastTicker_IRQHandler(void) {
    NRF_TIMER_Type *ticker = FastTicker;
    ticker_callback_ptr *call = callbacks;
    if (ticker->EVENTS_COMPARE[0]) {
        ticker->EVENTS_COMPARE[0] = 0;
        ticker->CC[0] += call[0]()*MICROSECONDS_PER_TICK;
    }
    if (ticker->EVENTS_COMPARE[1]) {
        ticker->EVENTS_COMPARE[1] = 0;
        ticker->CC[1] += call[1]()*MICROSECONDS_PER_TICK;
    }
    if (ticker->EVENTS_COMPARE[2]) {
        ticker->EVENTS_COMPARE[2] = 0;
        ticker->CC[2] += call[2]()*MICROSECONDS_PER_TICK;
    }
    if (ticker->EVENTS_COMPARE[3]) {
        ticker->EVENTS_COMPARE[3] = 0;
        ticker->CC[3] += MICROSECONDS_PER_MACRO_TICK;
        ticks += MILLISECONDS_PER_MACRO_TICK;
        hal_irq_pending(SlowTicker_IRQn);
    }
}


static const uint32_t masks[3] = {
    TIMER_INTENCLR_COMPARE0_Msk,
    TIMER_INTENCLR_COMPARE1_Msk,
    TIMER_INTENCLR_COMPARE2_Msk,
};

int set_ticker_callback(uint32_t index, ticker_callback_ptr func, int32_t initial_delay_us) {
    if (index > 3)
        return -1;
    NRF_TIMER_Type *ticker = FastTicker;
    callbacks[index] = noop;
    ticker->INTENCLR = masks[index];
    ticker->TASKS_CAPTURE[index] = 1;
    uint32_t t = FastTicker->CC[index];
    // Need to make sure that set tick is aligned to lastest tick
    // Use CC[3] as a reference, as that is always up-to-date.
    int32_t cc3 = FastTicker->CC[3];
    int32_t delta = t+initial_delay_us-cc3;
    delta = (delta/MICROSECONDS_PER_TICK+1)*MICROSECONDS_PER_TICK;
    callbacks[index] = func;
    ticker->INTENSET = masks[index];
    FastTicker->CC[index] = cc3 + delta;
    return 0;
}

int clear_ticker_callback(uint32_t index) {
    if (index > 3)
        return -1;
    FastTicker->INTENCLR = masks[index];
    callbacks[index] = noop;
    return 0;
}

void SlowTicker_IRQHandler(void)
{
    slow_ticker();
}

#if NRF52

#define LOW_PRIORITY_CALLBACK_LIMIT 4
callback_ptr low_priority_callbacks[LOW_PRIORITY_CALLBACK_LIMIT] = { NULL, NULL, NULL, NULL };

void LowPriority_IRQHandler(void)
{
    for (int id = 0; id < LOW_PRIORITY_CALLBACK_LIMIT; id++) {
        callback_ptr callback = low_priority_callbacks[id];
        if (callback != NULL) {
            low_priority_callbacks[id] = NULL;
            callback();
        }
    }
}

int set_low_priority_callback(callback_ptr callback, int id) {
    if (low_priority_callbacks[id] != NULL)
        return -1;
    low_priority_callbacks[id] = callback;
    NVIC_SetPendingIRQ(LowPriority_IRQn);
    return 0;
}

#endif // NRF52

#endif // MICROPY_PY_MACHINE_SOFT_PWM
