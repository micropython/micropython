/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2019 Lucian Copeland for Adafruit Industries
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

#include "shared-bindings/microcontroller/Pin.h"

#include "py/mphal.h"

STATIC uint8_t claimed_pins[1];

// Mark pin as free and return it to a quiescent state.
void reset_pin_number(uint8_t pin_port, uint8_t pin_number) {
    if (pin_port == 0x0F) {
        return;
    }

    // Clear claimed bit.
    claimed_pins[pin_port] &= ~(1 << pin_number);
}

void common_hal_reset_pin(const mcu_pin_obj_t *pin) {
    if (pin == NULL) {
        return;
    }
    reset_pin_number(0, pin->number);
}

void claim_pin(const mcu_pin_obj_t *pin) {
    // Set bit in claimed_pins bitmask.
    claimed_pins[0] |= 1 << pin->number;
}

void common_hal_mcu_pin_claim(const mcu_pin_obj_t *pin) {
    claim_pin(pin);
}

bool pin_number_is_free(uint8_t pin_port, uint8_t pin_number) {
    return !(claimed_pins[pin_port] & 1 << pin_number);
}

bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t *pin) {
    return pin_number_is_free(0, pin->number);
}
