// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Nick Moore for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "common-hal/rotaryio/IncrementalEncoder.h"
#include "shared-module/rotaryio/IncrementalEncoder.h"
#include "shared-bindings/rotaryio/IncrementalEncoder.h"
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

    // Ensure object starts in its deinit state.
    common_hal_rotaryio_incrementalencoder_mark_deinit(self);

    nrfx_gpiote_in_config_t cfg = {
        .sense = NRF_GPIOTE_POLARITY_TOGGLE,
        .pull = NRF_GPIO_PIN_PULLUP,
        .is_watcher = false,
        .hi_accuracy = true,
        .skip_gpio_setup = false
    };
    nrfx_err_t err = nrfx_gpiote_in_init(pin_a->number, &cfg, _intr_handler);
    if (err != NRFX_SUCCESS) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("All channels in use"));
    }
    err = nrfx_gpiote_in_init(pin_b->number, &cfg, _intr_handler);
    if (err != NRFX_SUCCESS) {
        nrfx_gpiote_in_uninit(pin_a->number);
        mp_raise_RuntimeError(MP_ERROR_TEXT("All channels in use"));
    }
    // Set the pins only now, after success to avoid out-of-bounds writes in nrfx_gpiote_in_uninit().
    self->pin_a = pin_a->number;
    self->pin_b = pin_b->number;
    _objs[self->pin_a] = self;
    _objs[self->pin_b] = self;
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

    common_hal_rotaryio_incrementalencoder_mark_deinit(self);
}

void common_hal_rotaryio_incrementalencoder_mark_deinit(rotaryio_incrementalencoder_obj_t *self) {
    self->pin_a = NO_PIN;
}
