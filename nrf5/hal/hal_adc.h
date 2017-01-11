/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#ifndef HAL_ADC_H__
#define HAL_ADC_H__

#include <stdint.h>

#include "nrf.h"

#if NRF51

#define ADC_IRQ_NUM ADC_IRQn
#define ADC_BASE ((NRF_ADC_Type *)NRF_ADC_BASE)
#define HAL_ADC_Type NRF_ADC_Type

#else

#define ADC_IRQ_NUM SAADC_IRQn
#define ADC_BASE ((NRF_SAADC_Type *)NRF_SAADC_BASE)
#define HAL_ADC_Type NRF_SAADC_Type

#endif

typedef enum {
    HAL_ADC_CHANNEL_2 = 2,
    HAL_ADC_CHANNEL_3,
    HAL_ADC_CHANNEL_4,
    HAL_ADC_CHANNEL_5,
    HAL_ADC_CHANNEL_6,
    HAL_ADC_CHANNEL_7,
} hal_adc_channel_t;

/**
  * @brief  ADC Configuration Structure definition
  */
typedef struct {
    hal_adc_channel_t channel;
} hal_adc_init_t;

/**
  * @brief  ADC handle Structure definition
  */
typedef struct __ADC_HandleTypeDef
{
    void                      *instance;     /* ADC register base address */
    hal_adc_init_t             init;         /* ADC initialization parameters */
} ADC_HandleTypeDef;

void hal_adc_init(HAL_ADC_Type * p_instance, hal_adc_init_t const * p_adc_init);

void hal_adc_start(HAL_ADC_Type * p_instance);

void hal_adc_stop(HAL_ADC_Type * p_instance);

uint8_t hal_adc_value(HAL_ADC_Type * p_instance);

uint8_t hal_adc_battery_level(HAL_ADC_Type * p_instance);

#endif // HAL_ADC_H__
