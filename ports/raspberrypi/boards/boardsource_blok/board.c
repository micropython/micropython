// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "src/rp2_common/hardware_gpio/include/hardware/gpio.h"
#include "supervisor/shared/board.h"

void board_init(void) {
}

bool board_requests_safe_mode(void) {
    return false;
}

void reset_board(void) {
    // turn off any left over LED
    // board_reset_user_neopixels(&pin_GPIO29, 62);
}

void board_deinit(void) {
}
