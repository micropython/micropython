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

#include <stdint.h>
#include <stdio.h>

#include "mphalport.h"
#include "hal_pwm.h"

#ifdef HAL_PWM_MODULE_ENABLED

#define PWM_COUNTER_TOP 16000 // 16MHz divided by 16000-> 1ms

volatile uint16_t g_pwm_seq[4];

void hal_pwm_init(NRF_PWM_Type * p_instance, hal_pwm_init_t const * p_pwm_init) {

    uint16_t duty_cycle = ((PWM_COUNTER_TOP*50)/100);

    g_pwm_seq[0] = duty_cycle;
    g_pwm_seq[1] = duty_cycle;
    g_pwm_seq[2] = 0;
    g_pwm_seq[3] = 0;

    p_instance->PSEL.OUT[0] = (p_pwm_init->pwm_pin << PWM_PSEL_OUT_PIN_Pos)
    		                | (PWM_PSEL_OUT_CONNECT_Connected << PWM_PSEL_OUT_CONNECT_Pos);

    p_instance->ENABLE      = (PWM_ENABLE_ENABLE_Enabled << PWM_ENABLE_ENABLE_Pos);
    p_instance->MODE        = (PWM_MODE_UPDOWN_Up << PWM_MODE_UPDOWN_Pos);
    p_instance->PRESCALER   = (PWM_PRESCALER_PRESCALER_DIV_1 << PWM_PRESCALER_PRESCALER_Pos);
    p_instance->COUNTERTOP  = (PWM_COUNTER_TOP << PWM_COUNTERTOP_COUNTERTOP_Pos); //1 msec
    p_instance->LOOP        = (PWM_LOOP_CNT_Disabled << PWM_LOOP_CNT_Pos);
    p_instance->DECODER     = (PWM_DECODER_LOAD_Individual << PWM_DECODER_LOAD_Pos)
                            | (PWM_DECODER_MODE_RefreshCount << PWM_DECODER_MODE_Pos);
    p_instance->SEQ[0].PTR  = ((uint32_t)(g_pwm_seq) << PWM_SEQ_PTR_PTR_Pos);
    p_instance->SEQ[0].CNT  = ((sizeof(g_pwm_seq) / sizeof(uint16_t)) << PWM_SEQ_CNT_CNT_Pos);

    p_instance->SEQ[0].REFRESH    = 0;
    p_instance->SEQ[0].ENDDELAY   = 0;
    p_instance->TASKS_SEQSTART[0] = 1;
}

#endif // HAL_PWM_MODULE_ENABLED
