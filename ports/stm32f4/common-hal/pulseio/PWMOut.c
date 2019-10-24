/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Dan Halbert for Adafruit Industries
 * Copyright (c) 2019 Lucian Copeland for Adafruit Industries
 * Utilizes code from Micropython, Copyright (c) 2013-2016 Damien P. George
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
#include "stm32f4xx_hal.h"

#define PWM_MAX_FREQ 6000000

// Get the frequency (in Hz) of the source clock for the given timer.
// On STM32F405/407/415/417 there are 2 cases for how the clock freq is set.
// If the APB prescaler is 1, then the timer clock is equal to its respective
// APB clock.  Otherwise (APB prescaler > 1) the timer clock is twice its
// respective APB clock.  See DM00031020 Rev 4, page 115.
static uint32_t timer_get_source_freq(uint32_t tim_id) {
    uint32_t source, clk_div;
    if (tim_id == 1 || (8 <= tim_id && tim_id <= 11)) {
        // TIM{1,8,9,10,11} are on APB2
        source = HAL_RCC_GetPCLK2Freq();
        clk_div = RCC->CFGR & RCC_CFGR_PPRE2;
    } else {
        // TIM{2,3,4,5,6,7,12,13,14} are on APB1
        source = HAL_RCC_GetPCLK1Freq();
        clk_div = RCC->CFGR & RCC_CFGR_PPRE1;
    }
    if (clk_div != 0) {
        // APB prescaler for this timer is > 1
        source *= 2;
    }
    return source;
}

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
    //Using PB08: Tim4 Ch3 for testing
    int tim_num = 10; 
    int tim_chan = TIM_CHANNEL_1;

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin_mask(pin->number);
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = 3;//2; //2 is timer 4
    HAL_GPIO_Init(pin_port(pin->port), &GPIO_InitStruct);

    //__HAL_RCC_TIM4_CLK_ENABLE();
    __HAL_RCC_TIM10_CLK_ENABLE();

    uint32_t source_freq = timer_get_source_freq(tim_num);
    uint32_t period = PWM_MAX_FREQ/frequency;
    mp_printf(&mp_plat_print, "SysCoreClock: %d\n", SystemCoreClock);
    mp_printf(&mp_plat_print, "Source Freq: %d\n", source_freq);
    mp_printf(&mp_plat_print, "Timer Freq: %d\n", source_freq/(source_freq / PWM_MAX_FREQ));
    mp_printf(&mp_plat_print, "Actual Freq: %d\n", (source_freq/(source_freq / PWM_MAX_FREQ))/period);
    mp_printf(&mp_plat_print, "Period: %d\n", (PWM_MAX_FREQ/frequency));

    TIM_HandleTypeDef tim = {0};
    tim.Instance = TIM10;
    tim.Init.Period = period - 1;
    tim.Init.Prescaler = (source_freq / PWM_MAX_FREQ) - 1; // TIM runs at 16MHz
    tim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    tim.Init.CounterMode = TIM_COUNTERMODE_UP;
    tim.Init.RepetitionCounter = 0;
    if(HAL_TIM_PWM_Init(&tim) == HAL_OK) {
        mp_printf(&mp_plat_print, "Tim Init Success\n");
    }

    // PWM configuration
    TIM_OC_InitTypeDef oc_init;
    oc_init.OCMode = TIM_OCMODE_PWM1;
    oc_init.Pulse = (period*duty)/100 - 1;
    oc_init.OCPolarity = TIM_OCPOLARITY_LOW;
    oc_init.OCFastMode = TIM_OCFAST_DISABLE;
    oc_init.OCNPolarity = TIM_OCNPOLARITY_LOW; // needed for TIM1 and TIM8
    oc_init.OCIdleState = TIM_OCIDLESTATE_SET; // needed for TIM1 and TIM8
    oc_init.OCNIdleState = TIM_OCNIDLESTATE_SET; // needed for TIM1 and TIM8
    if(HAL_TIM_PWM_ConfigChannel(&tim, &oc_init, tim_chan) == HAL_OK) {
        mp_printf(&mp_plat_print, "Channel Config Success\n");
    }
    if(HAL_TIM_PWM_Start(&tim, tim_chan) == HAL_OK) {
        mp_printf(&mp_plat_print, "Start Success\n");
    }

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
