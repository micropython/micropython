/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_PWMIO_PWMOUT_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_PWMIO_PWMOUT_H

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/pwmio/PWMOut.h"

extern const mp_obj_type_t pwmio_pwmout_type;

typedef enum pwmout_result_t {
    PWMOUT_OK,
    PWMOUT_INVALID_PIN,
    PWMOUT_INVALID_FREQUENCY,
    PWMOUT_INVALID_FREQUENCY_ON_PIN,
    PWMOUT_VARIABLE_FREQUENCY_NOT_AVAILABLE,
    PWMOUT_ALL_TIMERS_ON_PIN_IN_USE,
    PWMOUT_ALL_TIMERS_IN_USE,
    PWMOUT_ALL_CHANNELS_IN_USE,
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

// This is used by the supervisor to claim PWMOut devices indefinitely.
extern void common_hal_pwmio_pwmout_never_reset(pwmio_pwmout_obj_t *self);
extern void common_hal_pwmio_pwmout_reset_ok(pwmio_pwmout_obj_t *self);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_PWMIO_PWMOUT_H
