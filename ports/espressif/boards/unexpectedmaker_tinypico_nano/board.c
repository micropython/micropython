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

bool board_requests_safe_mode(void) {
    return false;
}

void reset_board(void) {
}

void board_deinit(void) {
}
