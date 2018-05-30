/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#include "atmel_start_pins.h"

#include "peripherals/external_interrupts.h"
#include "py/runtime.h"

void common_hal_rotaryio_incrementalencoder_construct(rotaryio_incrementalencoder_obj_t* self,
    const mcu_pin_obj_t* pin_a, const mcu_pin_obj_t* pin_b) {
    if (!pin_a->has_extint || !pin_a->has_extint) {
        mp_raise_RuntimeError("Both pins must support hardware interrupts");
    }

    // TODO: The SAMD51 has a peripheral dedicated to quadrature encoder debugging. Use it instead
    // of the external interrupt.

    if (eic_get_enable()) {
        if (!eic_channel_free(pin_a->extint_channel) || !eic_channel_free(pin_b->extint_channel)) {
            mp_raise_RuntimeError("A hardware interrupt channel is already in use");
        }
    } else {
        turn_on_external_interrupt_controller();
    }

    // These default settings apply when the EIC isn't yet enabled.
    self->eic_channel_a = pin_a->extint_channel;
    self->eic_channel_b = pin_b->extint_channel;
    self->pin_a = pin_a->pin;
    self->pin_b = pin_b->pin;

    gpio_set_pin_function(self->pin_a, GPIO_PIN_FUNCTION_A);
    gpio_set_pin_pull_mode(self->pin_a, GPIO_PULL_UP);

    gpio_set_pin_function(self->pin_b, GPIO_PIN_FUNCTION_A);
    gpio_set_pin_pull_mode(self->pin_b, GPIO_PULL_UP);

    set_eic_channel_data(self->eic_channel_a, (void*) self);
    set_eic_channel_data(self->eic_channel_b, (void*) self);

    bool pin_a_level = gpio_get_pin_level(self->pin_a);
    bool pin_b_level = gpio_get_pin_level(self->pin_b);
    if (!pin_a_level && !pin_b_level) {
        self->last_state = 1;
    } else if (!pin_a_level && pin_b_level) {
        self->last_state = 2;
    } else if (pin_a_level && pin_b_level) {
        self->last_state = 3;
    } else {
        self->last_state = 4;
    }

    turn_on_eic_channel(self->eic_channel_a, EIC_CONFIG_SENSE0_BOTH_Val, EIC_HANDLER_INCREMENTAL_ENCODER);
    turn_on_eic_channel(self->eic_channel_b, EIC_CONFIG_SENSE0_BOTH_Val, EIC_HANDLER_INCREMENTAL_ENCODER);
}

bool common_hal_rotaryio_incrementalencoder_deinited(rotaryio_incrementalencoder_obj_t* self) {
    return self->pin_a == NO_PIN;
}

void common_hal_rotaryio_incrementalencoder_deinit(rotaryio_incrementalencoder_obj_t* self) {
    if (common_hal_rotaryio_incrementalencoder_deinited(self)) {
        return;
    }
    turn_off_eic_channel(self->eic_channel_a);
    turn_off_eic_channel(self->eic_channel_b);
    reset_pin(self->pin_a);
    self->pin_a = NO_PIN;
    reset_pin(self->pin_b);
    self->pin_b = NO_PIN;
}

mp_int_t common_hal_rotaryio_incrementalencoder_get_position(rotaryio_incrementalencoder_obj_t* self) {
    return self->position;
}

void incrementalencoder_interrupt_handler(uint8_t channel) {
    rotaryio_incrementalencoder_obj_t* self = get_eic_channel_data(channel);
    // TODO(tannewt): If we need more speed then read the pin directly. gpio_get_pin_level has
    // smarts to compensate for pin direction we don't need.
    bool pin_a = gpio_get_pin_level(self->pin_a);
    bool pin_b = gpio_get_pin_level(self->pin_b);

    uint8_t this_state;
    if (!pin_a && !pin_b) {
        this_state = 1;
    } else if (!pin_a && pin_b) {
        this_state = 2;
    } else if (pin_a && pin_b) {
        this_state = 3;
    } else {
        this_state = 4;
    }

    // Handle wrap around explicitly.
    if (this_state == 4 && self->last_state == 1) {
        self->position -= 1;
    } else if (this_state == 1 && self->last_state == 4) {
        self->position += 1;
    } else {
        self->position += (this_state - self->last_state);
    }
    self->last_state = this_state;
}
