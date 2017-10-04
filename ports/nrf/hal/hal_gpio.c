/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Glenn Ruben Bakke
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

#include "hal_gpio.h"
#include "mphalport.h"
#include "hal_irq.h"

#define GPIOTE_IRQ_NUM GPIOTE_IRQn
#define GPIOTE_BASE ((NRF_GPIOTE_Type *)NRF_GPIOTE_BASE)
#define HAL_GPIOTE_Type NRF_GPIOTE_Type

static hal_gpio_event_callback_t m_callback;

void hal_gpio_register_callback(hal_gpio_event_callback_t cb) {
    m_callback = cb;

#if 0
    hal_gpio_event_config_t config;
    config.channel    = HAL_GPIO_EVENT_CHANNEL_0;
    config.event      = HAL_GPIO_POLARITY_EVENT_HIGH_TO_LOW;
    config.init_level = 1;
    config.pin        = 13;
    config.port       = 0;

    // start LFCLK if not already started
    if (NRF_CLOCK->LFCLKSTAT == 0) {
        NRF_CLOCK->TASKS_LFCLKSTART = 1;
        while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0);
        NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    }

	hal_irq_enable(GPIOTE_IRQ_NUM);
    hal_irq_priority(GPIOTE_IRQ_NUM, 3);

    hal_gpio_event_config(&config);
#endif
}

void hal_gpio_event_config(hal_gpio_event_config_t const * p_config) {
#if 0
    hal_gpio_cfg_pin(p_config->port, p_config->pin, HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_UP);

    uint8_t channel = (uint8_t)p_config->channel;
    GPIOTE_BASE->CONFIG[channel] = \
          GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos \
        | p_config->pin << GPIOTE_CONFIG_PSEL_Pos \
        | p_config->event \
        | p_config->init_level << GPIOTE_CONFIG_OUTINIT_Pos;

    GPIOTE_BASE->INTENSET = 1 << channel;
    GPIOTE_BASE->EVENTS_IN[channel] = 0;
#endif
}

#if 0

void GPIOTE_IRQHandler(void) {
    if (GPIOTE_BASE->EVENTS_IN[0]) {
        GPIOTE_BASE->EVENTS_IN[0] = 0;
        m_callback(HAL_GPIO_EVENT_CHANNEL_0);
    }
    if (GPIOTE_BASE->EVENTS_IN[1]) {
        GPIOTE_BASE->EVENTS_IN[1] = 0;
        m_callback(HAL_GPIO_EVENT_CHANNEL_1);
    }
    if (GPIOTE_BASE->EVENTS_IN[2]) {
        GPIOTE_BASE->EVENTS_IN[2] = 0;
        m_callback(HAL_GPIO_EVENT_CHANNEL_2);
    }
    if (GPIOTE_BASE->EVENTS_IN[3]) {
        GPIOTE_BASE->EVENTS_IN[3] = 0;
        m_callback(HAL_GPIO_EVENT_CHANNEL_3);
    }
#if NRF52
    if (GPIOTE_BASE->EVENTS_IN[4]) {
        GPIOTE_BASE->EVENTS_IN[4] = 0;
        m_callback(HAL_GPIO_EVENT_CHANNEL_4);
    }
    if (GPIOTE_BASE->EVENTS_IN[5]) {
        GPIOTE_BASE->EVENTS_IN[5] = 0;
        m_callback(HAL_GPIO_EVENT_CHANNEL_5);
    }
    if (GPIOTE_BASE->EVENTS_IN[6]) {
        GPIOTE_BASE->EVENTS_IN[6] = 0;
        m_callback(HAL_GPIO_EVENT_CHANNEL_6);
    }
    if (GPIOTE_BASE->EVENTS_IN[7]) {
        GPIOTE_BASE->EVENTS_IN[7] = 0;
        m_callback(HAL_GPIO_EVENT_CHANNEL_7);
    }
#endif
}

#endif // if 0
