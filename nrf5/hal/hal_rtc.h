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

#ifndef HAL_RTC_H__
#define HAL_RTC_H__

#include "nrf.h"

#if NRF51

#define RTC0 ((NRF_RTC_Type *) NRF_RTC0)
#define RTC0_IRQ_NUM RTC0_IRQn
#define RTC1 ((NRF_RTC_Type *) NRF_RTC1)
#define RTC1_IRQ_NUM RTC1_IRQn

#elif NRF52

#define RTC0 ((NRF_RTC_Type *) NRF_RTC0)
#define RTC0_IRQ_NUM RTC0_IRQn
#define RTC1 ((NRF_RTC_Type *) NRF_RTC1)
#define RTC1_IRQ_NUM RTC1_IRQn
#define RTC2 ((NRF_RTC_Type *) NRF_RTC2)
#define RTC2_IRQ_NUM RTC2_IRQn

#else
#error "Device not supported."
#endif

typedef void (*hal_rtc_app_callback)(NRF_RTC_Type * p_instance);

/**
  * @brief  RTC Configuration Structure definition
  */
typedef struct {
    NRF_RTC_Type     * p_instance;   /* RTC registers base address */
    uint32_t           irq_num;      /* RTC IRQ num */
    uint32_t           irq_priority; /* RTC IRQ priority */
    uint16_t           frequency;    /* RTC frequency in Hz */
} hal_rtc_conf_t;

/**
  * @brief  RTC handle Structure definition
  */
typedef struct __RTC_HandleTypeDef
{
    uint8_t            id;           /* RTC instance id */
    hal_rtc_conf_t     config;       /* RTC config */
} RTC_HandleTypeDef;

void hal_rtc_callback_set(hal_rtc_app_callback callback);

void hal_rtc_init(hal_rtc_conf_t const * p_rtc_config);

void hal_rtc_start(hal_rtc_conf_t const * p_rtc_conf, uint16_t period);

void hal_rtc_stop(hal_rtc_conf_t const * p_rtc_conf);

#endif // HAL_RTC_H__
