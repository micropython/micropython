// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "supervisor/board.h"
#include "common-hal/microcontroller/Pin.h"
#include "supervisor/shared/board.h"
#include "hal/include/hal_gpio.h"

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
    board_reset_user_neopixels(&pin_PB23, 10);
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
