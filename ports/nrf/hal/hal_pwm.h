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

#ifndef HAL_PWM_H__
#define HAL_PWM_H__

#include <stdint.h>

#include "nrf.h"

// TODO: nrf51 series need Soft PWM. Not part of HAL.

#if NRF52

#define PWM0  ((NRF_PWM_Type *)NRF_PWM0_BASE)
#define PWM0_IRQ_NUM PWM1_IRQn
#define PWM1  ((NRF_PWM_Type *)NRF_PWM1_BASE)
#define PWM1_IRQ_NUM PWM1_IRQn
#define PWM2  ((NRF_PWM_Type *)NRF_PWM2_BASE)
#define PWM2_IRQ_NUM PWM2_IRQn

#if 0 // TODO: nrf52840
#define PWM3  ((NRF_PWM_Type *)NRF_PWM3_BASE)
#define PWM3_IRQ_NUM PWM3_IRQn
#endif

#else
#error "Device not supported."
#endif

/**
  * @brief  PWM frequency type definition
  */
typedef enum {
    HAL_PWM_FREQ_16Mhz = 0,
    HAL_PWM_FREQ_8Mhz,
    HAL_PWM_FREQ_4Mhz,
    HAL_PWM_FREQ_2Mhz,
    HAL_PWM_FREQ_1Mhz,
    HAL_PWM_FREQ_500khz,
    HAL_PWM_FREQ_250khz,
    HAL_PWM_FREQ_125khz
} hal_pwm_freq_t;

/**
  * @brief  PWM mode type definition
  */
typedef enum {
    HAL_PWM_MODE_LOW_HIGH = 0,
    HAL_PWM_MODE_HIGH_LOW
} hal_pwm_mode_t;


typedef struct {
    uint8_t pwm_pin;
    hal_pwm_freq_t freq;
    uint8_t duty;
    uint16_t pulse_width;
    uint16_t period;
    hal_pwm_mode_t mode;
} hal_pwm_init_t;

/**
  * @brief  PWM handle Structure definition
  */
typedef struct __PWM_HandleTypeDef
{
    NRF_PWM_Type               *instance;    /* PWM registers base address */
    hal_pwm_init_t             init;         /* PWM initialization parameters */
} PWM_HandleTypeDef;


void hal_pwm_init(NRF_PWM_Type * p_instance, hal_pwm_init_t const * p_pwm_init);

void hal_pwm_freq_set(NRF_PWM_Type * p_instance, uint16_t freq);

void hal_pwm_period_set(NRF_PWM_Type * p_instance, uint16_t period);

void hal_pwm_duty_set(NRF_PWM_Type * p_instance, uint8_t duty);

void hal_pwm_start(NRF_PWM_Type * p_instance);

void hal_pwm_stop(NRF_PWM_Type * p_instance);

#endif // HAL_PWM_H__
