/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Nick Moore for Adafruit Industries
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

#include "common-hal/rotaryio/IncrementalEncoder.h"
#include "shared-module/rotaryio/IncrementalEncoder.h"
#include "nrfx_gpiote.h"

#include "py/runtime.h"

#include <stdio.h>

// obj array to map pin number -> self since nrfx hide the mapping
static rotaryio_incrementalencoder_obj_t *_objs[NUMBER_OF_PINS];

static void _intr_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
    rotaryio_incrementalencoder_obj_t *self = _objs[pin];
    if (!self) {
        return;
    }

    uint8_t new_state =
        ((uint8_t)nrf_gpio_pin_read(self->pin_a) << 1) |
        (uint8_t)nrf_gpio_pin_read(self->pin_b);

    shared_module_softencoder_state_update(self, new_state);
}

void common_hal_rotaryio_incrementalencoder_construct(rotaryio_incrementalencoder_obj_t *self,
    const mcu_pin_obj_t *pin_a, const mcu_pin_obj_t *pin_b) {

    self->pin_a = pin_a->number;
    self->pin_b = pin_b->number;

    _objs[self->pin_a] = self;
    _objs[self->pin_b] = self;

    nrfx_gpiote_in_config_t cfg = {
        .sense = NRF_GPIOTE_POLARITY_TOGGLE,
        .pull = NRF_GPIO_PIN_PULLUP,
        .is_watcher = false,
        .hi_accuracy = true,
        .skip_gpio_setup = false
    };
    nrfx_gpiote_in_init(self->pin_a, &cfg, _intr_handler);
    nrfx_gpiote_in_init(self->pin_b, &cfg, _intr_handler);
    nrfx_gpiote_in_event_enable(self->pin_a, true);
    nrfx_gpiote_in_event_enable(self->pin_b, true);

    claim_pin(pin_a);
    claim_pin(pin_b);
}

bool common_hal_rotaryio_incrementalencoder_deinited(rotaryio_incrementalencoder_obj_t *self) {
    return self->pin_a == NO_PIN;
}

void common_hal_rotaryio_incrementalencoder_deinit(rotaryio_incrementalencoder_obj_t *self) {
    if (common_hal_rotaryio_incrementalencoder_deinited(self)) {
        return;
    }
    _objs[self->pin_a] = NULL;
    _objs[self->pin_b] = NULL;

    nrfx_gpiote_in_event_disable(self->pin_a);
    nrfx_gpiote_in_event_disable(self->pin_b);
    nrfx_gpiote_in_uninit(self->pin_a);
    nrfx_gpiote_in_uninit(self->pin_b);
    reset_pin_number(self->pin_a);
    reset_pin_number(self->pin_b);
    self->pin_a = NO_PIN;
    self->pin_b = NO_PIN;
}
