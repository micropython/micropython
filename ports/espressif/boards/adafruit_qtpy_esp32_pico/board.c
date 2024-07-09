// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "components/driver/gpio/include/driver/gpio.h"
#include "components/hal/include/hal/gpio_hal.h"
#include "common-hal/microcontroller/Pin.h"

void board_init(void) {
    reset_board();
}

void reset_board(void) {
    // Turn on NeoPixel power by default.
    gpio_set_direction(8, GPIO_MODE_DEF_OUTPUT);
    gpio_set_level(8, true);
}

void board_deinit(void) {
    // Turn off NeoPixel
    gpio_set_direction(8, GPIO_MODE_DEF_OUTPUT);
    gpio_set_level(8, false);
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
