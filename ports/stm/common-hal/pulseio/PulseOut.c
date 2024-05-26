// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "common-hal/pulseio/PulseOut.h"

#include <stdint.h>

#include "py/mpconfig.h"
#include "shared-bindings/pulseio/PulseOut.h"
#include "shared-bindings/pwmio/PWMOut.h"

#include STM32_HAL_H
#include "timers.h"

// A single timer is shared amongst all PulseOut objects under the assumption that
// the code is single threaded.
static uint8_t refcount = 0;
static uint16_t *pulse_array = NULL;
static volatile uint16_t pulse_array_index = 0;
static uint16_t pulse_array_length;
// Timer is shared, must be accessible by interrupt
static TIM_HandleTypeDef tim_handle;
static pulseio_pulseout_obj_t *curr_pulseout = NULL;


static void turn_on(pulseio_pulseout_obj_t *pulseout) {
    // Turn on PWM
    HAL_TIM_PWM_Start(&(pulseout->pwmout.handle), pulseout->pwmout.channel);
}

static void turn_off(pulseio_pulseout_obj_t *pulseout) {
    // Turn off PWM
    HAL_TIM_PWM_Stop(&(pulseout->pwmout.handle), pulseout->pwmout.channel);
    // Make sure pin is low.
    HAL_GPIO_WritePin(pin_port(pulseout->pwmout.tim->pin->port),
        pin_mask(pulseout->pwmout.tim->pin->number), 0);
}

static void start_timer(void) {
    // Set the new period
    tim_handle.Init.Period = pulse_array[pulse_array_index] - 1;
    HAL_TIM_Base_Init(&tim_handle);

    // TIM7 has limited HAL support, set registers manually
    tim_handle.Instance->SR = 0; // Prevent the SR from triggering an interrupt
    tim_handle.Instance->CR1 |= TIM_CR1_CEN; // Resume timer
    tim_handle.Instance->CR1 |= TIM_CR1_URS; // Disable non-overflow interrupts
    __HAL_TIM_ENABLE_IT(&tim_handle, TIM_IT_UPDATE);
}

static void pulseout_event_handler(void) {
    // Detect TIM Update event
    if (__HAL_TIM_GET_FLAG(&tim_handle, TIM_FLAG_UPDATE) != RESET) {
        if (__HAL_TIM_GET_IT_SOURCE(&tim_handle, TIM_IT_UPDATE) != RESET) {
            __HAL_TIM_CLEAR_IT(&tim_handle, TIM_IT_UPDATE);
            if (common_hal_pulseio_pulseout_deinited(curr_pulseout)) {
                return; // invalid interrupt
            }

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
    }
}

void common_hal_pulseio_pulseout_construct(pulseio_pulseout_obj_t *self,
    const mcu_pin_obj_t *pin, uint32_t frequency, uint16_t duty_cycle) {
    pwmout_result_t result = common_hal_pwmio_pwmout_construct(
        &self->pwmout, pin, duty_cycle, frequency, false);

    // This will raise an exception and not return if needed.
    common_hal_pwmio_pwmout_raise_error(result);

    // Add to active PulseOuts
    refcount++;
    TIM_TypeDef *tim_instance = stm_peripherals_find_timer();
    stm_peripherals_timer_reserve(tim_instance);

    // calculate a 1ms period
    uint32_t source = stm_peripherals_timer_get_source_freq(tim_instance);
    uint32_t prescaler = source / 1000000; // 1us intervals

    stm_peripherals_timer_preinit(tim_instance, 4, pulseout_event_handler);
    tim_handle.Instance = tim_instance;
    tim_handle.Init.Period = 100; // immediately replaced.
    tim_handle.Init.Prescaler = prescaler - 1;
    tim_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    tim_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    tim_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    HAL_TIM_Base_Init(&tim_handle);
    tim_handle.Instance->SR = 0;


    turn_off(self);
}

bool common_hal_pulseio_pulseout_deinited(pulseio_pulseout_obj_t *self) {
    return common_hal_pwmio_pwmout_deinited(&self->pwmout);
}

void common_hal_pulseio_pulseout_deinit(pulseio_pulseout_obj_t *self) {
    if (common_hal_pulseio_pulseout_deinited(self)) {
        return;
    }
    turn_on(self);
    common_hal_pwmio_pwmout_deinit(&self->pwmout);

    refcount--;
    if (refcount == 0) {
        stm_peripherals_timer_free(tim_handle.Instance);
    }
}

void common_hal_pulseio_pulseout_send(pulseio_pulseout_obj_t *self, uint16_t *pulses, uint16_t length) {
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
    while (pulse_array_index < length) {
        // Do other things while we wait. The interrupts will handle sending the
        // signal.
        RUN_BACKGROUND_TASKS;

        // // Use when debugging, or issues are irrecoverable
        // if ((supervisor_ticks_ms64() - starttime ) > timeout ) {
        //    mp_raise_RuntimeError(MP_ERROR_TEXT("Error: Send Timeout"));
        // }
    }
    // turn off timer counter.
    tim_handle.Instance->CR1 &= ~TIM_CR1_CEN;
}
