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

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "common-hal/microcontroller/Pin.h"
#include "hal/include/hal_gpio.h"
#include "supervisor/shared/external_flash/external_flash.h"

void board_init(void) {
}

bool board_requests_safe_mode(void) {
    return false;
}

void reset_board(void) {
}

void board_deinit(void) {
}

void external_flash_setup(void) {
    // Do not reset the external flash write-protect and hold pins high
    never_reset_pin_number(PIN_PB22);
    never_reset_pin_number(PIN_PB23);

    // note: using output instead of input+pullups because the pullups are a little weak
    // Set the WP pin high
    gpio_set_pin_function(PIN_PB22, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_direction(PIN_PB22, GPIO_DIRECTION_OUT);
    gpio_set_pin_level(PIN_PB22, true);

    // Set the HOLD pin high
    gpio_set_pin_function(PIN_PB23, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_direction(PIN_PB23, GPIO_DIRECTION_OUT);
    gpio_set_pin_level(PIN_PB23, true);
}
