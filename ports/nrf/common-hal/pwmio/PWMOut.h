/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_NRF_COMMON_HAL_PWMIO_PWMOUT_H
#define MICROPY_INCLUDED_NRF_COMMON_HAL_PWMIO_PWMOUT_H

#include "nrfx_pwm.h"
#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    NRF_PWM_Type *pwm;
    uint8_t pin_number;
    uint8_t channel : 7;
    bool variable_frequency : 1;
    uint16_t duty_cycle;
    uint32_t frequency;
} pwmio_pwmout_obj_t;

void pwmout_reset(void);
NRF_PWM_Type *pwmout_allocate(uint16_t countertop, nrf_pwm_clk_t base_clock,
    bool variable_frequency, int8_t *channel_out, bool *pwm_already_in_use_out,
    IRQn_Type *irq);
void pwmout_free_channel(NRF_PWM_Type *pwm, int8_t channel);

#endif // MICROPY_INCLUDED_NRF_COMMON_HAL_PWMIO_PWMOUT_H
