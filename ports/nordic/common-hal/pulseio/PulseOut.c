// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "common-hal/pulseio/PulseOut.h"

#include <stdint.h>

#include "py/mpconfig.h"
#include "nrf/pins.h"
#include "nrf/timers.h"
#include "py/gc.h"
#include "py/runtime.h"
#include "shared-bindings/pulseio/PulseOut.h"
#include "shared-bindings/pwmio/PWMOut.h"

// A single timer is shared amongst all PulseOut objects under the assumption that
// the code is single threaded.
static uint8_t refcount = 0;

static nrfx_timer_t *timer = NULL;

static uint16_t *pulse_array = NULL;
static volatile uint16_t pulse_array_index = 0;
static uint16_t pulse_array_length;

static void turn_on(pulseio_pulseout_obj_t *pulseout) {
    pulseout->pwmout.pwm->PSEL.OUT[0] = pulseout->pwmout.pin->number;
}

static void turn_off(pulseio_pulseout_obj_t *pulseout) {
    // Disconnect pin from PWM.
    pulseout->pwmout.pwm->PSEL.OUT[0] = 0xffffffff;
    // Make sure pin is low.
    nrf_gpio_pin_clear(pulseout->pwmout.pin->number);
}

static void start_timer(void) {
    nrfx_timer_clear(timer);
    // true enables interrupt.
    nrfx_timer_compare(timer, NRF_TIMER_CC_CHANNEL0, pulse_array[pulse_array_index], true);
    nrfx_timer_resume(timer);
}

static void pulseout_event_handler(nrf_timer_event_t event_type, void *p_context) {
    pulseio_pulseout_obj_t *pulseout = (pulseio_pulseout_obj_t *)p_context;
    if (event_type != NRF_TIMER_EVENT_COMPARE0) {
        // Spurious event.
        return;
    }
    nrfx_timer_pause(timer);

    pulse_array_index++;
    // Ignore a zero-length pulse
    while (pulse_array_index < pulse_array_length &&
           pulse_array[pulse_array_index] == 0) {
        pulse_array_index++;
    }

    // No more pulses. Turn off output and don't restart.
    if (pulse_array_index >= pulse_array_length) {
        turn_off(pulseout);
        return;
    }

    // Alternate on and off, starting with on.
    if (pulse_array_index % 2 == 0) {
        turn_on(pulseout);
    } else {
        turn_off(pulseout);
    }

    // Count up to the next given value.
    start_timer();
}

void common_hal_pulseio_pulseout_construct(pulseio_pulseout_obj_t *self,
    const mcu_pin_obj_t *pin,
    uint32_t frequency,
    uint16_t duty_cycle) {

    pwmout_result_t result = common_hal_pwmio_pwmout_construct(
        &self->pwmout, pin, duty_cycle, frequency, false);

    // This will raise an exception and not return if needed.
    common_hal_pwmio_pwmout_raise_error(result);

    if (refcount == 0) {
        timer = nrf_peripherals_allocate_timer_or_throw();
    }
    refcount++;

    nrfx_timer_config_t timer_config = {
        // PulseOut durations are in microseconds, so this is convenient.
        .frequency = NRF_TIMER_FREQ_1MHz,
        .mode = NRF_TIMER_MODE_TIMER,
        .bit_width = NRF_TIMER_BIT_WIDTH_32,
        .interrupt_priority = NRFX_TIMER_DEFAULT_CONFIG_IRQ_PRIORITY,
        .p_context = self,
    };

    nrfx_timer_init(timer, &timer_config, &pulseout_event_handler);
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
        nrf_peripherals_free_timer(timer);
    }
}

void common_hal_pulseio_pulseout_send(pulseio_pulseout_obj_t *self, uint16_t *pulses, uint16_t length) {
    pulse_array = pulses;
    pulse_array_index = 0;
    pulse_array_length = length;

    nrfx_timer_enable(timer);

    turn_on(self);
    // Count up to the next given value.
    start_timer();

    while (pulse_array_index < length) {
        // Do other things while we wait. The interrupts will handle sending the
        // signal.
        RUN_BACKGROUND_TASKS;
    }

    nrfx_timer_disable(timer);
}
