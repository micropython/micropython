/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Dan Halbert for Adafruit Industries
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

#include "supervisor/shared/board.h"

#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/neopixel_write/__init__.h"

#ifdef USER_NEOPIXELS_PIN

// The maximum number of user neopixels right now is 10, on Circuit Playgrounds.
// PyBadge and PyGamer have max 5
#define USER_NEOPIXELS_MAX_COUNT 10

void board_reset_user_neopixels(void) {
    // Turn off on-board NeoPixel string
    uint8_t empty[USER_NEOPIXELS_MAX_COUNT * 3] = { 0 };
    digitalio_digitalinout_obj_t neopixel_pin;
    common_hal_digitalio_digitalinout_construct(&neopixel_pin, USER_NEOPIXELS_PIN);
    common_hal_digitalio_digitalinout_switch_to_output(&neopixel_pin, false,
        DRIVE_MODE_PUSH_PULL);
    common_hal_neopixel_write(&neopixel_pin, empty, USER_NEOPIXELS_MAX_COUNT * 3);
    common_hal_digitalio_digitalinout_deinit(&neopixel_pin);
}

#endif
