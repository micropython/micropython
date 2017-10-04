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

#ifndef HAL_RTC_H__
#define HAL_RTC_H__

#include "nrf.h"

#if NRF51
  #define RTC_BASE_POINTERS (const uint32_t[]){NRF_RTC0_BASE, \
                                               NRF_RTC1_BASE}
  #define RTC_IRQ_VALUES (const uint32_t[]){RTC0_IRQn, \
                                            RTC1_IRQn}
#endif

#if NRF52
  #define RTC_BASE_POINTERS (const uint32_t[]){NRF_RTC0_BASE, \
                                               NRF_RTC1_BASE, \
                                               NRF_RTC2_BASE}
  #define RTC_IRQ_VALUES (const uint32_t[]){RTC0_IRQn, \
                                            RTC1_IRQn, \
                                            RTC2_IRQn}
#endif

#define RTC_BASE(x) ((NRF_RTC_Type *)RTC_BASE_POINTERS[x])
#define RTC_IRQ_NUM(x) (RTC_IRQ_VALUES[x])

typedef void (*hal_rtc_app_callback)(uint8_t id);

/**
  * @brief  RTC Configuration Structure definition
  */
typedef struct {
    uint8_t  id;           /* RTC instance id */
    uint32_t period;       /* RTC period in ms */
    uint32_t irq_priority; /* RTC IRQ priority */
} hal_rtc_conf_t;

void hal_rtc_callback_set(hal_rtc_app_callback callback);

void hal_rtc_init(hal_rtc_conf_t const * p_rtc_config);

void hal_rtc_start(uint8_t id);

void hal_rtc_stop(uint8_t id);

#endif // HAL_RTC_H__
