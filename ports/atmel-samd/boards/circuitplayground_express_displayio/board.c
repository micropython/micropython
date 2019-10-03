/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include "boards/board.h"
#include "common-hal/microcontroller/Pin.h"
#include "hal/include/hal_gpio.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/neopixel_write/__init__.h"

void board_init(void)
{
}

// Check the status of the two buttons on CircuitPlayground Express. If both are
// pressed, then boot into user safe mode.
bool board_requests_safe_mode(void) {
    gpio_set_pin_function(PIN_PA14, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_direction(PIN_PA14, GPIO_DIRECTION_IN);
    gpio_set_pin_pull_mode(PIN_PA14, GPIO_PULL_DOWN);

    gpio_set_pin_function(PIN_PA28, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_direction(PIN_PA28, GPIO_DIRECTION_IN);
    gpio_set_pin_pull_mode(PIN_PA28, GPIO_PULL_DOWN);
    bool safe_mode = gpio_get_pin_level(PIN_PA14) &&
        gpio_get_pin_level(PIN_PA28);
    reset_pin_number(PIN_PA14);
    reset_pin_number(PIN_PA28);
    return safe_mode;
}

void reset_board(void) {
    uint8_t empty[30];
    memset(empty, 0, 30);
    digitalio_digitalinout_obj_t neopixel_pin;
    common_hal_digitalio_digitalinout_construct(&neopixel_pin, &pin_PB23);
    common_hal_digitalio_digitalinout_switch_to_output(&neopixel_pin, false,
        DRIVE_MODE_PUSH_PULL);
    common_hal_neopixel_write(&neopixel_pin, empty, 30);
    common_hal_digitalio_digitalinout_deinit(&neopixel_pin);
}
