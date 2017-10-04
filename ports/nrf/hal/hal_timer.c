/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
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

#include "mphalport.h"
#include "hal_timer.h"
#include "hal_irq.h"

#ifdef HAL_TIMER_MODULE_ENABLED

static hal_timer_app_callback m_callback;

void hal_timer_callback_set(hal_timer_app_callback callback) {
    m_callback = callback;
}

void hal_timer_init(hal_timer_conf_t const * p_timer_conf) {
    NRF_TIMER_Type * p_timer = TIMER_BASE(p_timer_conf->id);

    p_timer->CC[0]       = 1000 * p_timer_conf->period;
    p_timer->MODE        = TIMER_MODE_MODE_Timer;
    p_timer->BITMODE     = TIMER_BITMODE_BITMODE_24Bit << TIMER_BITMODE_BITMODE_Pos;
    p_timer->PRESCALER   = 4; // 1 us
    p_timer->INTENSET    = TIMER_INTENSET_COMPARE0_Msk;
    p_timer->SHORTS      = (TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos);
    p_timer->TASKS_CLEAR = 1;

    hal_irq_priority(TIMER_IRQ_NUM(p_timer_conf->id), p_timer_conf->irq_priority);
}

void hal_timer_start(uint8_t id) {
    NRF_TIMER_Type * p_timer = TIMER_BASE(id);

    p_timer->TASKS_CLEAR = 1;
    hal_irq_enable(TIMER_IRQ_NUM(id));
    p_timer->TASKS_START = 1;
}

void hal_timer_stop(uint8_t id) {
    NRF_TIMER_Type * p_timer = TIMER_BASE(id);

    hal_irq_disable(TIMER_IRQ_NUM(id));
    p_timer->TASKS_STOP = 1;
}

static void common_irq_handler(uint8_t id) {
    NRF_TIMER_Type * p_timer = TIMER_BASE(id);

    if (p_timer->EVENTS_COMPARE[0]) {
        p_timer->EVENTS_COMPARE[0] = 0;
            m_callback(id);
    }
}

void TIMER0_IRQHandler(void) {
    common_irq_handler(0);
}

#if (MICROPY_PY_MACHINE_SOFT_PWM != 1)
void TIMER1_IRQHandler(void) {
    common_irq_handler(1);
}
#endif

void TIMER2_IRQHandler(void) {
    common_irq_handler(2);
}

#if NRF52

void TIMER3_IRQHandler(void) {
    common_irq_handler(3);
}

void TIMER4_IRQHandler(void) {
    common_irq_handler(4);
}

#endif

#endif // HAL_TIMER_MODULE_ENABLED
