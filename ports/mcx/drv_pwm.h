/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 NXP
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

#ifndef MP_PORT_MCX_DRV_PWM_H
#define MP_PORT_MCX_DRV_PWM_H

#include <stdbool.h>
#include <stdint.h>

typedef struct drv_pwm_type {
    uint8_t id;
    void *instance;

    uint8_t channel;
    bool inverted;
    uint32_t input_freq;
} drv_pwm_t;

int drv_pwm_init(drv_pwm_t *pwm, uint8_t id, uint8_t channel, uint32_t freq, bool inverted);
int drv_pwm_deinit(drv_pwm_t *pwm);
int drv_pwm_start(drv_pwm_t *pwm);
int drv_pwm_stop(drv_pwm_t *pwm);
int drv_pwm_freq_set(drv_pwm_t *pwm, uint32_t freq);
int drv_pwm_freq_get(drv_pwm_t *pwm, uint32_t *freq);
int drv_pwm_duty_set(drv_pwm_t *pwm, uint16_t duty_u16);
int drv_pwm_duty_get(drv_pwm_t *pwm, uint16_t *duty_u16);

#endif
