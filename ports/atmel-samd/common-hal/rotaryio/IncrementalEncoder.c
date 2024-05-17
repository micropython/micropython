// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "common-hal/rotaryio/IncrementalEncoder.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/rotaryio/IncrementalEncoder.h"
#include "shared-module/rotaryio/IncrementalEncoder.h"

#include "atmel_start_pins.h"

#include "eic_handler.h"
#include "samd/external_interrupts.h"
#include "py/runtime.h"

void common_hal_rotaryio_incrementalencoder_construct(rotaryio_incrementalencoder_obj_t *self,
    const mcu_pin_obj_t *pin_a, const mcu_pin_obj_t *pin_b) {
    if (!pin_a->has_extint) {
        raise_ValueError_invalid_pin_name(MP_QSTR_pin_a);
    }
    if (!pin_b->has_extint) {
        raise_ValueError_invalid_pin_name(MP_QSTR_pin_b);
    }

    // TODO: The SAMD51 has a peripheral dedicated to quadrature encoder debugging. Use it instead
    // of the external interrupt.

    if (eic_get_enable()) {
        if (!eic_channel_free(pin_a->extint_channel) || !eic_channel_free(pin_b->extint_channel)) {
            mp_raise_RuntimeError(MP_ERROR_TEXT("Internal resource(s) in use"));
        }
    } else {
        turn_on_external_interrupt_controller();
    }

    // These default settings apply when the EIC isn't yet enabled.
    self->eic_channel_a = pin_a->extint_channel;
    self->eic_channel_b = pin_b->extint_channel;
    self->pin_a = pin_a->number;
    self->pin_b = pin_b->number;

    gpio_set_pin_function(self->pin_a, GPIO_PIN_FUNCTION_A);
    gpio_set_pin_pull_mode(self->pin_a, GPIO_PULL_UP);

    gpio_set_pin_function(self->pin_b, GPIO_PIN_FUNCTION_A);
    gpio_set_pin_pull_mode(self->pin_b, GPIO_PULL_UP);

    set_eic_channel_data(self->eic_channel_a, (void *)self);
    set_eic_channel_data(self->eic_channel_b, (void *)self);

    self->position = 0;
    self->sub_count = 0;

    shared_module_softencoder_state_init(self,
        ((uint8_t)gpio_get_pin_level(self->pin_a) << 1) |
        (uint8_t)gpio_get_pin_level(self->pin_b));

    claim_pin(pin_a);
    claim_pin(pin_b);

    set_eic_handler(self->eic_channel_a, EIC_HANDLER_INCREMENTAL_ENCODER);
    turn_on_eic_channel(self->eic_channel_a, EIC_CONFIG_SENSE0_BOTH_Val);

    set_eic_handler(self->eic_channel_b, EIC_HANDLER_INCREMENTAL_ENCODER);
    turn_on_eic_channel(self->eic_channel_b, EIC_CONFIG_SENSE0_BOTH_Val);
}

bool common_hal_rotaryio_incrementalencoder_deinited(rotaryio_incrementalencoder_obj_t *self) {
    return self->pin_a == NO_PIN;
}

void common_hal_rotaryio_incrementalencoder_deinit(rotaryio_incrementalencoder_obj_t *self) {
    if (common_hal_rotaryio_incrementalencoder_deinited(self)) {
        return;
    }

    set_eic_handler(self->eic_channel_a, EIC_HANDLER_NO_INTERRUPT);
    turn_off_eic_channel(self->eic_channel_a);

    set_eic_handler(self->eic_channel_b, EIC_HANDLER_NO_INTERRUPT);
    turn_off_eic_channel(self->eic_channel_b);

    reset_pin_number(self->pin_a);
    self->pin_a = NO_PIN;

    reset_pin_number(self->pin_b);
    self->pin_b = NO_PIN;
}

void incrementalencoder_interrupt_handler(uint8_t channel) {
    rotaryio_incrementalencoder_obj_t *self = get_eic_channel_data(channel);

    uint8_t new_state =
        ((uint8_t)gpio_get_pin_level(self->pin_a) << 1) |
        (uint8_t)gpio_get_pin_level(self->pin_b);

    shared_module_softencoder_state_update(self, new_state);
}
