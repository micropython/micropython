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

#include "eic_handler.h"
#include "samd/external_interrupts.h"
#include "py/runtime.h"
#include "supervisor/shared/translate.h"

void common_hal_rotaryio_incrementalencoder_construct(rotaryio_incrementalencoder_obj_t* self,
    const mcu_pin_obj_t* pin_a, const mcu_pin_obj_t* pin_b) {
    if (!pin_a->has_extint || !pin_b->has_extint) {
        mp_raise_RuntimeError(translate("Both pins must support hardware interrupts"));
    }

    // TODO: The SAMD51 has a peripheral dedicated to quadrature encoder debugging. Use it instead
    // of the external interrupt.

    if (eic_get_enable()) {
        if (!eic_channel_free(pin_a->extint_channel) || !eic_channel_free(pin_b->extint_channel)) {
            mp_raise_RuntimeError(translate("A hardware interrupt channel is already in use"));
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

    set_eic_channel_data(self->eic_channel_a, (void*) self);
    set_eic_channel_data(self->eic_channel_b, (void*) self);

    self->position = 0;
    self->quarter_count = 0;

    // Top two bits of self->last_state don't matter, because they'll be gone as soon as
    // interrupt handler is called.
    self->last_state =
        ((uint8_t) gpio_get_pin_level(self->pin_a) << 1) |
        (uint8_t) gpio_get_pin_level(self->pin_b);

    claim_pin(pin_a);
    claim_pin(pin_b);

    set_eic_handler(self->eic_channel_a, EIC_HANDLER_INCREMENTAL_ENCODER);
    turn_on_eic_channel(self->eic_channel_a, EIC_CONFIG_SENSE0_BOTH_Val);

    set_eic_handler(self->eic_channel_b, EIC_HANDLER_INCREMENTAL_ENCODER);
    turn_on_eic_channel(self->eic_channel_b, EIC_CONFIG_SENSE0_BOTH_Val);
}

bool common_hal_rotaryio_incrementalencoder_deinited(rotaryio_incrementalencoder_obj_t* self) {
    return self->pin_a == NO_PIN;
}

void common_hal_rotaryio_incrementalencoder_deinit(rotaryio_incrementalencoder_obj_t* self) {
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

mp_int_t common_hal_rotaryio_incrementalencoder_get_position(rotaryio_incrementalencoder_obj_t* self) {
    return self->position;
}

void common_hal_rotaryio_incrementalencoder_set_position(rotaryio_incrementalencoder_obj_t* self,
        mp_int_t new_position) {
    self->position = new_position;
}

void incrementalencoder_interrupt_handler(uint8_t channel) {
    rotaryio_incrementalencoder_obj_t* self = get_eic_channel_data(channel);

    // This table also works for detent both at 11 and 00
    // For 11 at detent:
    // Turning cw: 11->01->00->10->11
    // Turning ccw: 11->10->00->01->11
    // For 00 at detent:
    // Turning cw: 00->10->11->10->00
    // Turning ccw: 00->01->11->10->00

    // index table by state <oldA><oldB><newA><newB>
    #define BAD 7
    static const int8_t transitions[16] = {
        0,    // 00 -> 00 no movement
        -1,   // 00 -> 01 3/4 ccw (11 detent) or 1/4 ccw (00 at detent)
        +1,   // 00 -> 10 3/4 cw or 1/4 cw
        BAD,  // 00 -> 11 non-Gray-code transition
        +1,   // 01 -> 00 2/4 or 4/4 cw
        0,    // 01 -> 01 no movement
        BAD,  // 01 -> 10 non-Gray-code transition
        -1,   // 01 -> 11 4/4 or 2/4 ccw
        -1,   // 10 -> 00 2/4 or 4/4 ccw
        BAD,  // 10 -> 01 non-Gray-code transition
        0,    // 10 -> 10 no movement
        +1,   // 10 -> 11 4/4 or 2/4 cw
        BAD,  // 11 -> 00 non-Gray-code transition
        +1,   // 11 -> 01 1/4 or 3/4 cw
        -1,   // 11 -> 10 1/4 or 3/4 ccw
        0,    // 11 -> 11 no movement
    };

    // Shift the old AB bits to the "old" position, and set the new AB bits.
    // TODO(tannewt): If we need more speed then read the pin directly. gpio_get_pin_level has
    // smarts to compensate for pin direction we don't need.
    self->last_state = (self->last_state & 0x3) << 2 |
        ((uint8_t) gpio_get_pin_level(self->pin_a) << 1) |
        (uint8_t) gpio_get_pin_level(self->pin_b);

    int8_t quarter_incr = transitions[self->last_state];
    if (quarter_incr == BAD) {
        // Missed a transition. We don't know which way we're going, so do nothing.
        return;
    }

    self->quarter_count += quarter_incr;
    if (self->quarter_count >= 4) {
        self->position += 1;
        self->quarter_count = 0;
    } else if (self->quarter_count <= -4) {
        self->position -= 1;
        self->quarter_count = 0;
    }
}
