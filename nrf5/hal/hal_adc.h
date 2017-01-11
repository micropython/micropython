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

#include "nrf.h"

#if NRF51

#define ADC_IRQ_NUM ADC_IRQn
#define ADC_BASE(x) ((NRF_ADC_Type *)NRF_ADC_BASE)
#define HAL_ADC_Type NRF_ADC_Type

#else

#define ADC_IRQ_NUM SAADC_IRQn
#define ADC_BASE(x) ((NRF_SAADC_Type *)NRF_SAADC_BASE)
#define HAL_ADC_Type NRF_ADC_Type

#endif

/**
  * @brief  ADC Configuration Structure definition
  */
typedef struct {
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

#endif // HAL_ADC_H__
