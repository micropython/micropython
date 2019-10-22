/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Dan Halbert for Adafruit Industries
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
#include "py/runtime.h"
#include "common-hal/pulseio/PWMOut.h"
#include "shared-bindings/pulseio/PWMOut.h"
#include "supervisor/shared/translate.h"

void pwmout_reset(void) {
}

void common_hal_pulseio_pwmout_never_reset(pulseio_pwmout_obj_t *self) {
}

void common_hal_pulseio_pwmout_reset_ok(pulseio_pwmout_obj_t *self) {
}

pwmout_result_t common_hal_pulseio_pwmout_construct(pulseio_pwmout_obj_t* self,
                                                    const mcu_pin_obj_t* pin,
                                                    uint16_t duty,
                                                    uint32_t frequency,
                                                    bool variable_frequency) {

    __HAL_RCC_TIM4_CLK_ENABLE();

    TIM_HandleTypeDef tim = {0};
    tim.Instance = TIM4;
    tim.Init.Period = LED_PWM_TIM_PERIOD - 1;
    tim.Init.Prescaler = timer_get_source_freq(pwm_cfg->tim_id) / 1000000 - 1; // TIM runs at 1MHz
    tim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    tim.Init.CounterMode = TIM_COUNTERMODE_UP;
    tim.Init.RepetitionCounter = 0;
    HAL_TIM_PWM_Init(&tim);

    // PWM configuration
    TIM_OC_InitTypeDef oc_init;
    oc_init.OCMode = TIM_OCMODE_PWM1;
    oc_init.Pulse = 0; // off
    oc_init.OCPolarity = MICROPY_HW_LED_INVERTED ? TIM_OCPOLARITY_LOW : TIM_OCPOLARITY_HIGH;
    oc_init.OCFastMode = TIM_OCFAST_DISABLE;
    oc_init.OCNPolarity = TIM_OCNPOLARITY_HIGH; // needed for TIM1 and TIM8
    oc_init.OCIdleState = TIM_OCIDLESTATE_SET; // needed for TIM1 and TIM8
    oc_init.OCNIdleState = TIM_OCNIDLESTATE_SET; // needed for TIM1 and TIM8
    HAL_TIM_PWM_ConfigChannel(&tim, &oc_init, pwm_cfg->tim_channel);
    HAL_TIM_PWM_Start(&tim, pwm_cfg->tim_channel);
    return PWMOUT_OK;
}

bool common_hal_pulseio_pwmout_deinited(pulseio_pwmout_obj_t* self) {
    return true;
}

void common_hal_pulseio_pwmout_deinit(pulseio_pwmout_obj_t* self) {
}

void common_hal_pulseio_pwmout_set_duty_cycle(pulseio_pwmout_obj_t* self, uint16_t duty_cycle) {
}

uint16_t common_hal_pulseio_pwmout_get_duty_cycle(pulseio_pwmout_obj_t* self) {
    return 0;
}

void common_hal_pulseio_pwmout_set_frequency(pulseio_pwmout_obj_t* self, uint32_t frequency) {
}

uint32_t common_hal_pulseio_pwmout_get_frequency(pulseio_pwmout_obj_t* self) {
    return 0;
}

bool common_hal_pulseio_pwmout_get_variable_frequency(pulseio_pwmout_obj_t* self) {
    return 0;
}
