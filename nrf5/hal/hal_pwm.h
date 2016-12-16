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

#ifndef HAL_PWM_H__
#define HAL_PWM_H__

#include <stdint.h>

#include "nrf.h"

#if NRF51

// TODO: create software PWM.

#elif NRF52

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


typedef struct {
    uint8_t pwm_pin;
} hal_pwm_init_t;

#endif // HAL_PWM_H__
