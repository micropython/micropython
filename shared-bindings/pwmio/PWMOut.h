// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/pwmio/PWMOut.h"

extern const mp_obj_type_t pwmio_pwmout_type;

typedef enum pwmout_result_t {
    PWMOUT_OK,
    PWMOUT_INVALID_PIN,
    PWMOUT_INVALID_FREQUENCY,
    PWMOUT_INVALID_FREQUENCY_ON_PIN,
    PWMOUT_VARIABLE_FREQUENCY_NOT_AVAILABLE,
    PWMOUT_INTERNAL_RESOURCES_IN_USE,
    PWMOUT_INITIALIZATION_ERROR,
} pwmout_result_t;

extern pwmout_result_t common_hal_pwmio_pwmout_construct(pwmio_pwmout_obj_t *self,
    const mcu_pin_obj_t *pin, uint16_t duty, uint32_t frequency,
    bool variable_frequency);
extern void common_hal_pwmio_pwmout_deinit(pwmio_pwmout_obj_t *self);
extern bool common_hal_pwmio_pwmout_deinited(pwmio_pwmout_obj_t *self);
extern void common_hal_pwmio_pwmout_set_duty_cycle(pwmio_pwmout_obj_t *self, uint16_t duty);
extern uint16_t common_hal_pwmio_pwmout_get_duty_cycle(pwmio_pwmout_obj_t *self);
extern void common_hal_pwmio_pwmout_set_frequency(pwmio_pwmout_obj_t *self, uint32_t frequency);
extern uint32_t common_hal_pwmio_pwmout_get_frequency(pwmio_pwmout_obj_t *self);
extern bool common_hal_pwmio_pwmout_get_variable_frequency(pwmio_pwmout_obj_t *self);

// Don't use this! It is only used internally for backwards compatibility.
extern const mcu_pin_obj_t *common_hal_pwmio_pwmout_get_pin(pwmio_pwmout_obj_t *self);

// This is used by the supervisor to claim PWMOut devices indefinitely.
extern void common_hal_pwmio_pwmout_never_reset(pwmio_pwmout_obj_t *self);
extern void common_hal_pwmio_pwmout_reset_ok(pwmio_pwmout_obj_t *self);

extern void common_hal_pwmio_pwmout_raise_error(pwmout_result_t result);
