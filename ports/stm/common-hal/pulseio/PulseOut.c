/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Lucian Copeland for Adafruit Industries
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

#include "common-hal/pulseio/PulseOut.h"

#include <stdint.h>

#include "py/mpconfig.h"
#include "py/gc.h"
#include "py/runtime.h"
#include "shared-bindings/pulseio/PulseOut.h"
#include "shared-bindings/pulseio/PWMOut.h"
#include "supervisor/shared/translate.h"

#include STM32_HAL_H
#include "common-hal/microcontroller/Pin.h"

// A single timer is shared amongst all PulseOut objects under the assumption that
// the code is single threaded.
STATIC uint8_t refcount = 0;

STATIC uint16_t *pulse_array = NULL;
STATIC volatile uint16_t pulse_array_index = 0;
STATIC uint16_t pulse_array_length;

//Timer is shared, must be accessible by interrupt
STATIC TIM_HandleTypeDef t7_handle;
pulseio_pulseout_obj_t *curr_pulseout = NULL;

STATIC void turn_on(pulseio_pulseout_obj_t *pulseout) {
    // Turn on PWM
    HAL_TIM_PWM_Start(&(pulseout->pwmout->handle), pulseout->pwmout->channel);
}

STATIC void turn_off(pulseio_pulseout_obj_t *pulseout) {
    // Turn off PWM
    HAL_TIM_PWM_Stop(&(pulseout->pwmout->handle), pulseout->pwmout->channel);
    // Make sure pin is low.
    HAL_GPIO_WritePin(pin_port(pulseout->pwmout->tim->pin->port),
                      pin_mask(pulseout->pwmout->tim->pin->number), 0);
}

STATIC void start_timer(void) {
    // Set the new period
    t7_handle.Init.Period = pulse_array[pulse_array_index] - 1;
    HAL_TIM_Base_Init(&t7_handle);

    // TIM7 has limited HAL support, set registers manually
    t7_handle.Instance->SR = 0; // Prevent the SR from triggering an interrupt
    t7_handle.Instance->CR1 |= TIM_CR1_CEN; // Resume timer
    t7_handle.Instance->CR1 |= TIM_CR1_URS; // Disable non-overflow interrupts
    __HAL_TIM_ENABLE_IT(&t7_handle, TIM_IT_UPDATE);

}

STATIC void pulseout_event_handler(void) {
    if (curr_pulseout->pwmout == NULL) {
        return; //invalid interrupt
    }

    HAL_GPIO_WritePin(pin_port(2),pin_mask(6), 1);
    HAL_GPIO_WritePin(pin_port(2),pin_mask(6), 0);

    pulse_array_index++;

    // No more pulses. Turn off output and don't restart.
    if (pulse_array_index >= pulse_array_length) {
        turn_off(curr_pulseout);
        return;
    }

    // Alternate on and off, starting with on.
    if (pulse_array_index % 2 == 0) {
        turn_on(curr_pulseout);
    } else {
        turn_off(curr_pulseout);
    }

    // Count up to the next given value.
    start_timer();
}

void pulseout_reset() {
    #if HAS_BASIC_TIM
    __HAL_RCC_TIM7_CLK_DISABLE();
    refcount = 0;
    #endif
}

void common_hal_pulseio_pulseout_construct(pulseio_pulseout_obj_t* self,
                                           const pulseio_pwmout_obj_t* carrier) {
#if !(HAS_BASIC_TIM)
    mp_raise_NotImplementedError(translate("PulseOut not supported on this chip"));
#else
    // Add to active PulseOuts
    refcount++;

    // Calculate a 1 ms period
    uint32_t source, clk_div;
    source = HAL_RCC_GetPCLK1Freq(); // TIM7 is on APB1
    clk_div = RCC->CFGR & RCC_CFGR_PPRE1;
    // APB quirk, see See DM00031020 Rev 4, page 115.
    if (clk_div != 0) {
        // APB prescaler for this timer is > 1
        source *= 2;
    }

    uint32_t prescaler = source/1000000; //1us intervals

    __HAL_RCC_TIM7_CLK_ENABLE();
    HAL_NVIC_SetPriority(TIM7_IRQn, 4, 0);
    HAL_NVIC_EnableIRQ(TIM7_IRQn);

    // Timers 6 and 7 have no pins, so using them doesn't affect PWM availability
    t7_handle.Instance = TIM7;
    t7_handle.Init.Period = 100; //immediately replaced.
    t7_handle.Init.Prescaler = prescaler - 1;
    t7_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    t7_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    t7_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    HAL_TIM_Base_Init(&t7_handle);
    t7_handle.Instance->SR = 0;

    // The HAL can't work with const, recast required.
    self->pwmout = (pulseio_pwmout_obj_t*)carrier;

    turn_off(self);
#endif
}

bool common_hal_pulseio_pulseout_deinited(pulseio_pulseout_obj_t* self) {
    return self->pwmout == NULL;
}

void common_hal_pulseio_pulseout_deinit(pulseio_pulseout_obj_t* self) {
    if (common_hal_pulseio_pulseout_deinited(self)) {
        return;
    }
    turn_on(self);
    self->pwmout = NULL;

    refcount--;
    if (refcount == 0) {
        #if HAS_BASIC_TIM
        __HAL_RCC_TIM7_CLK_DISABLE();
        #endif
    }
}

void common_hal_pulseio_pulseout_send(pulseio_pulseout_obj_t* self, uint16_t* pulses, uint16_t length) {
    pulse_array = pulses;
    pulse_array_index = 0;
    pulse_array_length = length;
    curr_pulseout = self;

    turn_on(self);

    // Count up to the next given value.
    start_timer();

    // Use when debugging, or issues are irrecoverable
    // uint32_t starttime = supervisor_ticks_ms64();
    // uint32_t timeout = 10000;
    while(pulse_array_index < length) {
        // Do other things while we wait. The interrupts will handle sending the
        // signal.
        RUN_BACKGROUND_TASKS;

        // Use when debugging, or issues are irrecoverable
        // if ((supervisor_ticks_ms64() - starttime ) > timeout ) {
        //    mp_raise_RuntimeError(translate("Error: Send Timeout"));
        // }
    }
    //turn off timer counter.
    t7_handle.Instance->CR1 &= ~TIM_CR1_CEN;
}

void TIM7_IRQHandler(void)
{
    // Detect TIM Update event
    if (__HAL_TIM_GET_FLAG(&t7_handle, TIM_FLAG_UPDATE) != RESET)
    {
        if (__HAL_TIM_GET_IT_SOURCE(&t7_handle, TIM_IT_UPDATE) != RESET)
        {
            __HAL_TIM_CLEAR_IT(&t7_handle, TIM_IT_UPDATE);
            pulseout_event_handler();
        }
    }
}
