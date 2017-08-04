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

#include <stdint.h>
#include <stdio.h>

#include "mphalport.h"
#include "hal_pwm.h"

#ifdef HAL_PWM_MODULE_ENABLED

#define PWM_COUNTER_TOP 16000 // 16MHz divided by 16000-> 1ms

volatile uint16_t g_pwm_seq[4];
volatile uint16_t g_pwm_period;

static const uint32_t hal_pwm_frequency_lookup[] = {
    PWM_PRESCALER_PRESCALER_DIV_1, // 16MHz
    PWM_PRESCALER_PRESCALER_DIV_2, // 8MHz
    PWM_PRESCALER_PRESCALER_DIV_4, // 4MHz
    PWM_PRESCALER_PRESCALER_DIV_8, // 2MHz
    PWM_PRESCALER_PRESCALER_DIV_16, // 1MHz
    PWM_PRESCALER_PRESCALER_DIV_32, // 500kHz
    PWM_PRESCALER_PRESCALER_DIV_64, // 250kHz
    PWM_PRESCALER_PRESCALER_DIV_128 // 125kHz
};

void hal_pwm_init(NRF_PWM_Type * p_instance, hal_pwm_init_t const * p_pwm_init) {
    g_pwm_period = p_pwm_init->period;
    uint16_t pulse_width = ((g_pwm_period * p_pwm_init->duty)/100);

    if (p_pwm_init->pulse_width > 0) {
        pulse_width = p_pwm_init->pulse_width;
    }

    if (p_pwm_init->mode == HAL_PWM_MODE_HIGH_LOW) {
        g_pwm_seq[0] = g_pwm_period - pulse_width;
        g_pwm_seq[1] = g_pwm_period - pulse_width;
    } else {
        g_pwm_seq[0] = pulse_width;
        g_pwm_seq[1] = pulse_width;
    }

    g_pwm_seq[2] = 0;
    g_pwm_seq[3] = 0;

    p_instance->PSEL.OUT[0] = (p_pwm_init->pwm_pin << PWM_PSEL_OUT_PIN_Pos)
    		                | (PWM_PSEL_OUT_CONNECT_Connected << PWM_PSEL_OUT_CONNECT_Pos);

    p_instance->ENABLE      = (PWM_ENABLE_ENABLE_Enabled << PWM_ENABLE_ENABLE_Pos);
    p_instance->MODE        = (PWM_MODE_UPDOWN_Up << PWM_MODE_UPDOWN_Pos);
    p_instance->PRESCALER   = (hal_pwm_frequency_lookup[p_pwm_init->freq] << PWM_PRESCALER_PRESCALER_Pos);
    p_instance->COUNTERTOP  = (p_pwm_init->period << PWM_COUNTERTOP_COUNTERTOP_Pos);
    p_instance->LOOP        = (PWM_LOOP_CNT_Disabled << PWM_LOOP_CNT_Pos);
    p_instance->DECODER     = (PWM_DECODER_LOAD_Individual << PWM_DECODER_LOAD_Pos)
                            | (PWM_DECODER_MODE_RefreshCount << PWM_DECODER_MODE_Pos);
    p_instance->SEQ[0].PTR  = ((uint32_t)(g_pwm_seq) << PWM_SEQ_PTR_PTR_Pos);
    p_instance->SEQ[0].CNT  = ((sizeof(g_pwm_seq) / sizeof(uint16_t)) << PWM_SEQ_CNT_CNT_Pos);

    p_instance->SEQ[0].REFRESH    = 0;
    p_instance->SEQ[0].ENDDELAY   = 0;
}

void hal_pwm_start(NRF_PWM_Type * p_instance) {
    p_instance->TASKS_SEQSTART[0] = 1;
}

void hal_pwm_stop(NRF_PWM_Type * p_instance) {
    p_instance->TASKS_SEQSTART[0] = 0;
    p_instance->ENABLE            = (PWM_ENABLE_ENABLE_Disabled << PWM_ENABLE_ENABLE_Pos);
}

void hal_pwm_freq_set(NRF_PWM_Type * p_instance, uint16_t freq) {
#if 0
    p_instance->PRESCALER = (hal_pwm_frequency_lookup[freq] << PWM_PRESCALER_PRESCALER_Pos);
#endif
}

void hal_pwm_period_set(NRF_PWM_Type * p_instance, uint16_t period) {
#if 0
    g_pwm_period = period;
    p_instance->COUNTERTOP = (g_pwm_period << PWM_COUNTERTOP_COUNTERTOP_Pos);
#endif
}

void hal_pwm_duty_set(NRF_PWM_Type * p_instance, uint8_t duty) {
#if 0
    uint16_t duty_cycle = ((g_pwm_period * duty)/100);

    g_pwm_seq[0] = duty_cycle;
    g_pwm_seq[1] = duty_cycle;
#endif
}

#endif // HAL_PWM_MODULE_ENABLED
