// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "driver/gpio.h"

void board_init(void) {
    reset_board();
}

void reset_board(void) {
    // Turn on I2C power by default.

    gpio_set_direction(7, GPIO_MODE_DEF_OUTPUT);
    gpio_set_level(7, true);
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
