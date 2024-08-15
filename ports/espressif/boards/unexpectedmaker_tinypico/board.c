// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "driver/gpio.h"
#include "common-hal/microcontroller/Pin.h"

void board_init(void) {
    reset_board();
}

bool board_requests_safe_mode(void) {
    return false;
}

void reset_board(void) {
    // Turn on APA102 power by default
    gpio_set_direction(13, GPIO_MODE_DEF_OUTPUT);
    gpio_set_level(13, true);
}

void board_deinit(void) {
    // Turn off APA102 power
    gpio_set_direction(13, GPIO_MODE_DEF_OUTPUT);
    gpio_set_level(13, false);
}
