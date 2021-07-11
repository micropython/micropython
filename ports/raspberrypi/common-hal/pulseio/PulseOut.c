/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Dave Putz for Adafruit Industries
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
#include "mpconfigport.h"
#include "py/runtime.h"
#include "shared-bindings/pulseio/PulseOut.h"
#include "shared-bindings/pwmio/PWMOut.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "common-hal/pwmio/PWMOut.h"
#include "supervisor/shared/translate.h"
#include "src/rp2_common/hardware_pwm/include/hardware/pwm.h"
#include "src/common/pico_time/include/pico/time.h"

static uint8_t refcount = 0;
static uint16_t *pulse_buffer = NULL;
static volatile uint16_t pulse_index = 0;
static uint16_t pulse_length;
pwmio_pwmout_obj_t *pwmout_obj;
volatile uint16_t current_duty_cycle;
static uint32_t min_pulse = 0;
static alarm_id_t cur_alarm;

void pulse_finish(pwmio_pwmout_obj_t *carrier) {
    pulse_index++;
    // Turn pwm pin off by setting duty cyle to 1.
    common_hal_pwmio_pwmout_set_duty_cycle(carrier,1);
    if (pulse_index >= pulse_length) {
        return;
    }
    if (pulse_index % 2 == 0) {
        common_hal_pwmio_pwmout_set_duty_cycle(carrier,current_duty_cycle);
    }
    uint64_t delay = pulse_buffer[pulse_index];
    if (delay < min_pulse) {
        delay = min_pulse;
    }
    cur_alarm = 0;
    // if the alarm cannot be set, try again with a longer delay
    while (cur_alarm == 0) {
        cur_alarm = add_alarm_in_us(delay, pulseout_interrupt_handler, carrier, false);
        delay = delay + 1;
    }
}

int64_t pulseout_interrupt_handler(alarm_id_t id, void *user_data) {
    pulse_finish(user_data);
    return 0;
}

void pulseout_reset() {
    refcount = 0;
}

void common_hal_pulseio_pulseout_construct(pulseio_pulseout_obj_t *self,
    const pwmio_pwmout_obj_t *carrier,
    const mcu_pin_obj_t *pin,
    uint32_t frequency,
    uint16_t duty_cycle) {

    refcount++;
    pwmout_obj = (pwmio_pwmout_obj_t *)carrier;
    current_duty_cycle = common_hal_pwmio_pwmout_get_duty_cycle(pwmout_obj);
    pwm_set_enabled(carrier->slice,false);
    common_hal_pwmio_pwmout_set_duty_cycle(pwmout_obj,1);
    self->pin = carrier->pin->number;
    self->slice = carrier->slice;
    self->carrier = pwmout_obj;
    min_pulse = (1000000 / carrier->actual_frequency) / 2;
}

bool common_hal_pulseio_pulseout_deinited(pulseio_pulseout_obj_t *self) {
    return self->pin == NO_PIN;
}

void common_hal_pulseio_pulseout_deinit(pulseio_pulseout_obj_t *self) {
    if (common_hal_pulseio_pulseout_deinited(self)) {
        return;
    }
    refcount--;
    self->pin = NO_PIN;
}

void common_hal_pulseio_pulseout_send(pulseio_pulseout_obj_t *self, uint16_t *pulses, uint16_t length) {
    pulse_buffer = pulses;
    pulse_index = 0;
    pulse_length = length;

    common_hal_pwmio_pwmout_set_duty_cycle(self->carrier,current_duty_cycle);
    pwm_set_enabled(self->slice,true);
    uint64_t delay = pulse_buffer[0];
    if (delay < min_pulse) {
        delay = min_pulse;
    }
    alarm_id_t init_alarm = 0;
    // if the alarm cannot be set, try again with a longer delay
    while (init_alarm == 0) {
        init_alarm = add_alarm_in_us(delay, pulseout_interrupt_handler, self->carrier, false);
        delay = delay + 1;
    }
    cur_alarm = init_alarm;

    while (pulse_index < length) {
        // Do other things while we wait. The interrupts will handle sending the
        // signal.
        RUN_BACKGROUND_TASKS;
    }
    // Short delay to give pin time to settle before disabling PWM
    common_hal_mcu_delay_us(min_pulse);
    pwm_set_enabled(self->slice,false);
}
