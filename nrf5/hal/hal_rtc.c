/*
 * This file is part of the Micro Python project, http://micropython.org/
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

static hal_rtc_app_callback m_callback;

void hal_rtc_callback_set(hal_rtc_app_callback callback) {
    m_callback = callback;
}

void hal_rtc_init(hal_rtc_conf_t const * p_rtc_conf) {
    p_rtc_conf->p_instance->PRESCALER = (32768 / p_rtc_conf->frequency) - 1; // approx correct.
    hal_irq_priority(p_rtc_conf->irq_num, p_rtc_conf->irq_priority);

    // start LFCLK if not already started
    if (NRF_CLOCK->LFCLKSTAT == 0) {
        NRF_CLOCK->TASKS_LFCLKSTART = 1;
        while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0);
        NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    }
}

void hal_rtc_start(hal_rtc_conf_t const * p_rtc_conf, uint16_t period) {
    uint32_t counter = p_rtc_conf->p_instance->COUNTER;

    p_rtc_conf->p_instance->CC[0] = counter + period;

    p_rtc_conf->p_instance->EVTENSET = RTC_EVTEN_COMPARE0_Msk;
    p_rtc_conf->p_instance->INTENSET = RTC_INTENSET_COMPARE0_Msk;

    hal_irq_clear(p_rtc_conf->irq_num);
    hal_irq_enable(p_rtc_conf->irq_num);

    p_rtc_conf->p_instance->TASKS_START = 1;
}

void hal_rtc_stop(hal_rtc_conf_t const * p_rtc_conf) {
    p_rtc_conf->p_instance->EVTENCLR = RTC_EVTEN_COMPARE0_Msk;
    p_rtc_conf->p_instance->INTENCLR = RTC_INTENSET_COMPARE0_Msk;

    hal_irq_disable(p_rtc_conf->irq_num);

    p_rtc_conf->p_instance->TASKS_STOP = 1;
}

void RTC0_IRQHandler(void)
{
    // clear all events
    NRF_RTC0->EVENTS_COMPARE[0] = 0;
    NRF_RTC0->EVENTS_COMPARE[1] = 0;
    NRF_RTC0->EVENTS_COMPARE[2] = 0;
    NRF_RTC0->EVENTS_COMPARE[3] = 0;
    NRF_RTC0->EVENTS_TICK       = 0;
    NRF_RTC0->EVENTS_OVRFLW     = 0;

    m_callback(NRF_RTC0);
}

void RTC1_IRQHandler(void)
{
    // clear all events
    NRF_RTC1->EVENTS_COMPARE[0] = 0;
    NRF_RTC1->EVENTS_COMPARE[1] = 0;
    NRF_RTC1->EVENTS_COMPARE[2] = 0;
    NRF_RTC1->EVENTS_COMPARE[3] = 0;
    NRF_RTC1->EVENTS_TICK       = 0;
    NRF_RTC1->EVENTS_OVRFLW     = 0;

    m_callback(NRF_RTC1);
}

#if NRF52

void RTC2_IRQHandler(void)
{
    // clear all events
    NRF_RTC2->EVENTS_COMPARE[0] = 0;
    NRF_RTC2->EVENTS_COMPARE[1] = 0;
    NRF_RTC2->EVENTS_COMPARE[2] = 0;
    NRF_RTC2->EVENTS_COMPARE[3] = 0;
    NRF_RTC2->EVENTS_TICK       = 0;
    NRF_RTC2->EVENTS_OVRFLW     = 0;

    m_callback(NRF_RTC2);
}

#endif // NRF52

#endif // HAL_RTC_MODULE_ENABLED

