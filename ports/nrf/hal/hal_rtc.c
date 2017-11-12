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
#include "hal_rtc.h"
#include "hal_irq.h"

#ifdef HAL_RTC_MODULE_ENABLED

#define HAL_LFCLK_FREQ (32768UL)
#define HAL_RTC_FREQ (10UL)
#define HAL_RTC_COUNTER_PRESCALER ((HAL_LFCLK_FREQ/HAL_RTC_FREQ)-1)

static hal_rtc_app_callback m_callback;

static uint32_t m_period[sizeof(RTC_BASE_POINTERS) / sizeof(uint32_t)];

void hal_rtc_callback_set(hal_rtc_app_callback callback) {
    m_callback = callback;
}

void hal_rtc_init(hal_rtc_conf_t const * p_rtc_conf) {
    NRF_RTC_Type * p_rtc = RTC_BASE(p_rtc_conf->id);

    // start LFCLK if not already started
    if (NRF_CLOCK->LFCLKSTAT == 0) {
        NRF_CLOCK->TASKS_LFCLKSTART = 1;
        while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0);
        NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    }

    m_period[p_rtc_conf->id] = p_rtc_conf->period;

    p_rtc->PRESCALER = HAL_RTC_COUNTER_PRESCALER;
    hal_irq_priority(RTC_IRQ_NUM(p_rtc_conf->id), p_rtc_conf->irq_priority);
}

void hal_rtc_start(uint8_t id) {
    NRF_RTC_Type * p_rtc = RTC_BASE(id);

    uint32_t period  = HAL_RTC_FREQ * m_period[id];
    uint32_t counter = p_rtc->COUNTER;

    p_rtc->CC[0] = counter + period;

    p_rtc->EVTENSET = RTC_EVTEN_COMPARE0_Msk;
    p_rtc->INTENSET = RTC_INTENSET_COMPARE0_Msk;

    hal_irq_clear(RTC_IRQ_NUM(id));
    hal_irq_enable(RTC_IRQ_NUM(id));

    p_rtc->TASKS_START = 1;
}

void hal_rtc_stop(uint8_t id) {
    NRF_RTC_Type * p_rtc = RTC_BASE(id);

    p_rtc->EVTENCLR = RTC_EVTEN_COMPARE0_Msk;
    p_rtc->INTENCLR = RTC_INTENSET_COMPARE0_Msk;

    hal_irq_disable(RTC_IRQ_NUM(id));

    p_rtc->TASKS_STOP = 1;
}

static void common_irq_handler(uint8_t id) {
    NRF_RTC_Type * p_rtc = RTC_BASE(id);

    // clear all events
    p_rtc->EVENTS_COMPARE[0] = 0;
    p_rtc->EVENTS_COMPARE[1] = 0;
    p_rtc->EVENTS_COMPARE[2] = 0;
    p_rtc->EVENTS_COMPARE[3] = 0;
    p_rtc->EVENTS_TICK       = 0;
    p_rtc->EVENTS_OVRFLW     = 0;

    m_callback(id);
}

void RTC0_IRQHandler(void)
{
    common_irq_handler(0);
}

void RTC1_IRQHandler(void)
{
    common_irq_handler(1);
}

#if NRF52

void RTC2_IRQHandler(void)
{
    common_irq_handler(2);
}

#endif // NRF52

#endif // HAL_RTC_MODULE_ENABLED

