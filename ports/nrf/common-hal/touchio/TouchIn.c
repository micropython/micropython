/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2018 Nick Moore for Adafruit Industries
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

#include <string.h>
#include <stdio.h>

#include "py/nlr.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "py/binary.h"
#include "py/mphal.h"
#include "shared-bindings/touchio/TouchIn.h"
#include "supervisor/shared/translate.h"

#include "nrf.h"
#include "nrfx/hal/nrf_saadc.h"

#include "tick.h"

bool touch_enabled = false;

#define CHANNEL_NO 0
#define N_SAMPLES 10

static uint16_t get_raw_reading(touchio_touchin_obj_t *self) {

    nrf_saadc_value_t samples[N_SAMPLES];

    // Configure analog input.
    // XXX cloned from AnalogIn and probably overkill

    const nrf_saadc_channel_config_t config = {
        .resistor_p = NRF_SAADC_RESISTOR_DISABLED,
        .resistor_n = NRF_SAADC_RESISTOR_DISABLED,
        .gain = NRF_SAADC_GAIN1_6,
        .reference = NRF_SAADC_REFERENCE_INTERNAL,
        .acq_time = NRF_SAADC_ACQTIME_3US,
        .mode = NRF_SAADC_MODE_SINGLE_ENDED,
        .burst = NRF_SAADC_BURST_DISABLED,
        .pin_p = NRF_SAADC_INPUT_VDD,
        .pin_n = NRF_SAADC_INPUT_VDD,
    };

    nrf_saadc_resolution_set(NRF_SAADC_RESOLUTION_14BIT);
    nrf_saadc_oversample_set(NRF_SAADC_OVERSAMPLE_DISABLED);
    nrf_saadc_enable();

    for (uint32_t i = 0; i < NRF_SAADC_CHANNEL_COUNT; i++)
        nrf_saadc_channel_input_set(i, NRF_SAADC_INPUT_DISABLED, NRF_SAADC_INPUT_DISABLED);

    nrf_saadc_channel_init(CHANNEL_NO, &config);
    nrf_saadc_buffer_init(samples, N_SAMPLES);

    // set pad to digital output high for 20us to charge it

    nrf_gpio_cfg_output(self->pin->number);
    nrf_gpio_pin_set(self->pin->number);

    mp_hal_delay_us(20);

    // set pad back to an input and take some samples

     __disable_irq();

    nrf_saadc_channel_input_set(CHANNEL_NO, self->pin->adc_channel, self->pin->adc_channel);
 
    nrf_saadc_task_trigger(NRF_SAADC_TASK_START);
    while (nrf_saadc_event_check(NRF_SAADC_EVENT_STARTED) == 0);
    nrf_saadc_event_clear(NRF_SAADC_EVENT_STARTED);

    // XXX surely there's a better way
    for (uint32_t i = 0; i < N_SAMPLES; i++) {
        nrf_saadc_task_trigger(NRF_SAADC_TASK_SAMPLE);
        while (nrf_saadc_event_check(NRF_SAADC_EVENT_DONE) == 0);
        nrf_saadc_event_clear(NRF_SAADC_EVENT_DONE);
    }

    __enable_irq();

    nrf_saadc_task_trigger(NRF_SAADC_TASK_STOP);
    while (nrf_saadc_event_check(NRF_SAADC_EVENT_STOPPED) == 0);
    nrf_saadc_event_clear(NRF_SAADC_EVENT_STOPPED);

    // turn off SAADC & set output pin low

    nrf_gpio_pin_clear(self->pin->number);

    nrf_saadc_channel_input_set(CHANNEL_NO, NRF_SAADC_INPUT_DISABLED, NRF_SAADC_INPUT_DISABLED);
    nrf_saadc_disable();

    nrf_gpio_cfg_output(self->pin->number);

    int32_t sumy = 0;
    int32_t sumxy = 0;

    //mp_printf(MP_PYTHON_PRINTER, "touch");

    // XXX sort of like a least squares fit here

    for (int i = 0; i < N_SAMPLES; i++) {
        //mp_printf(MP_PYTHON_PRINTER, " %d", samples[i]);
        sumy += samples[i];
        sumxy += i*samples[i];
    }
    int32_t r = (N_SAMPLES - 1) * sumy / 2 - sumxy;

    //mp_printf(MP_PYTHON_PRINTER, " -> %d %d -> %d\n", sumy, sumxy, r);

    return (uint16_t)r;
}

void common_hal_touchio_touchin_construct(touchio_touchin_obj_t* self,
        const mcu_pin_obj_t *pin) {
    if (!pin->adc_channel) {
        mp_raise_ValueError(translate("Invalid pin"));
    }
    self->pin = pin;
    claim_pin(pin);
}

bool common_hal_touchio_touchin_deinited(touchio_touchin_obj_t* self) {
    return self->pin == NULL;
}

void common_hal_touchio_touchin_deinit(touchio_touchin_obj_t* self) {
    if (common_hal_touchio_touchin_deinited(self)) {
        return;
    }

    reset_pin_number(self->pin->number);
    self->pin = NULL;
}

void touchin_reset() {
}

bool common_hal_touchio_touchin_get_value(touchio_touchin_obj_t *self) {
    uint16_t reading = get_raw_reading(self);
    return reading > self->threshold;
}

uint16_t common_hal_touchio_touchin_get_raw_value(touchio_touchin_obj_t *self) {
    return get_raw_reading(self);
}

uint16_t common_hal_touchio_touchin_get_threshold(touchio_touchin_obj_t *self) {
    return self->threshold;
}

void common_hal_touchio_touchin_set_threshold(touchio_touchin_obj_t *self, uint16_t new_threshold) {
    self->threshold = new_threshold;
}
