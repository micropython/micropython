// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Dave Putz for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "common-hal/pulseio/PulseOut.h"

#include <stdint.h>
#include "mpconfigport.h"
#include "py/runtime.h"
#include "shared-bindings/pulseio/PulseOut.h"
#include "shared-bindings/pwmio/PWMOut.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "common-hal/pwmio/PWMOut.h"
#include "hardware/structs/pwm.h"
#include "src/rp2_common/hardware_gpio/include/hardware/gpio.h"
#include "src/rp2_common/hardware_pwm/include/hardware/pwm.h"
#include "src/common/pico_time/include/pico/time.h"

volatile alarm_id_t cur_alarm = 0;

static void pulse_finish(pulseio_pulseout_obj_t *self) {
    self->pulse_index++;
    // Turn pwm pin off by switching the GPIO mux to SIO (the cpu manual
    // control).
    if (self->pulse_index >= self->pulse_length) {
        gpio_set_function(self->pin, GPIO_FUNC_SIO);
        return;
    }
    if (self->pulse_index % 2 == 0) {
        gpio_set_function(self->pin, GPIO_FUNC_PWM);
    } else {
        gpio_set_function(self->pin, GPIO_FUNC_SIO);
    }
    uint64_t delay = self->pulse_buffer[self->pulse_index];
    if (delay < self->min_pulse) {
        delay = self->min_pulse;
    }
    cur_alarm = 0;
    // if the alarm cannot be set, try again with a longer delay
    while (cur_alarm == 0) {
        cur_alarm = add_alarm_in_us(delay, pulseout_interrupt_handler, self, false);
        delay = delay + 1;
    }
}

int64_t pulseout_interrupt_handler(alarm_id_t id, void *user_data) {
    pulse_finish(user_data);
    return 0;
}

void common_hal_pulseio_pulseout_construct(pulseio_pulseout_obj_t *self,
    const mcu_pin_obj_t *pin,
    uint32_t frequency,
    uint16_t duty_cycle) {

    pwmout_result_t result = common_hal_pwmio_pwmout_construct(
        &self->carrier, pin, 0, frequency, false);
    // This will raise an exception and not return if needed.
    common_hal_pwmio_pwmout_raise_error(result);

    // Disable gpio output before we set the duty cycle.
    gpio_put(pin->number, false);
    gpio_set_dir(pin->number, GPIO_OUT);
    gpio_set_function(pin->number, GPIO_FUNC_SIO);
    common_hal_pwmio_pwmout_set_duty_cycle(&self->carrier, duty_cycle);

    self->pin = pin->number;
    self->slice = self->carrier.slice;
    self->min_pulse = (1000000 / self->carrier.actual_frequency);
}

bool common_hal_pulseio_pulseout_deinited(pulseio_pulseout_obj_t *self) {
    return self->pin == NO_PIN;
}

void common_hal_pulseio_pulseout_deinit(pulseio_pulseout_obj_t *self) {
    if (common_hal_pulseio_pulseout_deinited(self)) {
        return;
    }
    gpio_set_dir(self->pin, GPIO_IN);
    common_hal_pwmio_pwmout_deinit(&self->carrier);
    self->pin = NO_PIN;
}

void common_hal_pulseio_pulseout_send(pulseio_pulseout_obj_t *self, uint16_t *pulses, uint16_t length) {
    self->pulse_buffer = pulses;
    self->pulse_index = 0;
    self->pulse_length = length;

    // Turn on the signal by connecting the PWM to the outside pin.
    gpio_set_function(self->pin, GPIO_FUNC_PWM);
    uint64_t delay = self->pulse_buffer[0];
    if (delay < self->min_pulse) {
        delay = self->min_pulse;
    }
    cur_alarm = 0;
    // if the alarm cannot be set, try again with a longer delay
    while (cur_alarm == 0) {
        cur_alarm = add_alarm_in_us(delay, pulseout_interrupt_handler, self, false);
        delay = delay + 1;
    }

    while (self->pulse_index < length) {
        // Do other things while we wait. The interrupts will handle sending the
        // signal.
        RUN_BACKGROUND_TASKS;
    }
}
