// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT


#include "common-hal/countio/Counter.h"
#include "shared-bindings/countio/Counter.h"

#include "py/runtime.h"

#include "nrfx_gpiote.h"

// obj array to map pin number -> self since nrfx hide the mapping
static countio_counter_obj_t *_countio_objs[NUMBER_OF_PINS];

static void _intr_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
    countio_counter_obj_t *self = _countio_objs[pin];
    if (!self) {
        return;
    }
    self->count += 1;
}

void common_hal_countio_counter_construct(countio_counter_obj_t *self,
    const mcu_pin_obj_t *pin, countio_edge_t edge, digitalio_pull_t pull) {

    self->pin = pin->number;
    _countio_objs[self->pin] = self;

    self->count = 0;

    nrf_gpiote_polarity_t polarity = NRF_GPIOTE_POLARITY_TOGGLE;
    switch (edge) {
        case EDGE_RISE:
            polarity = NRF_GPIOTE_POLARITY_LOTOHI;
            break;
        case EDGE_FALL:
            polarity = NRF_GPIOTE_POLARITY_HITOLO;
            break;
        case EDGE_RISE_AND_FALL:
        default:
            break;
    }

    nrf_gpio_pin_pull_t hal_pull = NRF_GPIO_PIN_NOPULL;
    switch (pull) {
        case PULL_UP:
            hal_pull = NRF_GPIO_PIN_PULLUP;
            break;
        case PULL_DOWN:
            hal_pull = NRF_GPIO_PIN_PULLDOWN;
            break;
        case PULL_NONE:
        default:
            break;
    }

    nrfx_gpiote_in_config_t cfg = {
        .sense = polarity,
        .pull = hal_pull,
        .is_watcher = false,
        .hi_accuracy = true,
        .skip_gpio_setup = false,
    };

    nrfx_err_t err = nrfx_gpiote_in_init(self->pin, &cfg, _intr_handler);
    if (err != NRFX_SUCCESS) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("All channels in use"));
    }
    nrfx_gpiote_in_event_enable(self->pin, true);

    claim_pin(pin);
}

bool common_hal_countio_counter_deinited(countio_counter_obj_t *self) {
    return self->pin == NO_PIN;
}

void common_hal_countio_counter_deinit(countio_counter_obj_t *self) {
    if (common_hal_countio_counter_deinited(self)) {
        return;
    }
    _countio_objs[self->pin] = NULL;

    nrfx_gpiote_in_event_disable(self->pin);
    nrfx_gpiote_in_uninit(self->pin);
    reset_pin_number(self->pin);
    self->pin = NO_PIN;
}

mp_int_t common_hal_countio_counter_get_count(countio_counter_obj_t *self) {
    return self->count;
}

void common_hal_countio_counter_set_count(countio_counter_obj_t *self,
    mp_int_t new_count) {
    self->count = new_count;
}
