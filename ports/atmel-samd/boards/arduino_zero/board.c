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

#include "boards/board.h"
#include "mpconfigboard.h"
#include "hal/include/hal_gpio.h"

void board_init(void)
{
    gpio_set_pin_function(MICROPY_HW_LED_TX, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_direction(MICROPY_HW_LED_TX, GPIO_DIRECTION_OUT);
    gpio_set_pin_level(MICROPY_HW_LED_TX, true);

    gpio_set_pin_function(MICROPY_HW_LED_RX, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_direction(MICROPY_HW_LED_RX, GPIO_DIRECTION_OUT);
    gpio_set_pin_level(MICROPY_HW_LED_RX, true);
}

bool board_requests_safe_mode(void) {
    return false;
}

void reset_board(void) {
}
