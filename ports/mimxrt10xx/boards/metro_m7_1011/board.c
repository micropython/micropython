/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2019 Artur Pacholec
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

#include "supervisor/board.h"
#include "shared-bindings/microcontroller/Pin.h"

// These pins should never ever be reset; doing so could interfere with basic operation.
STATIC const mcu_pin_obj_t *_reset_forbidden_pins[] = {
    // SWD Pins
    &pin_GPIO_AD_13,// SWDIO
    &pin_GPIO_AD_12,// SWCLK

    // FLEX flash
    &pin_GPIO_SD_12,
    &pin_GPIO_SD_11,
    &pin_GPIO_SD_10,
    &pin_GPIO_SD_09,
    &pin_GPIO_SD_08,
    &pin_GPIO_SD_07,
    &pin_GPIO_SD_06,
};

STATIC bool _reset_forbidden(const mcu_pin_obj_t *pin) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(_reset_forbidden_pins); i++) {
        if (pin == _reset_forbidden_pins[i]) {
            return true;
        }
    }
    return false;
}

bool mimxrt10xx_board_reset_pin_number(const mcu_pin_obj_t *pin) {
    if (_reset_forbidden(pin)) {
        return true;
    }

    // Other reset variations would go here.

    return false;
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
