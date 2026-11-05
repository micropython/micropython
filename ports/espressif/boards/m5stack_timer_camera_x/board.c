// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Bill Sideris, independently providing these changes.
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "driver/gpio.h"
#include "common-hal/microcontroller/Pin.h"

bool espressif_board_reset_pin_number(gpio_num_t pin_number) {
    if (pin_number == 33) {
        /*
         * Turn on BAT_HOLD by default,
         * so that the board does not power off
         * when usb is disconnected or
         * the power button is released.
         */
        config_pin_as_output_with_level(pin_number, true);
        return true;
    }
    return false;
}

void board_init(void) {
    // GPIO33 (BAT_HOLD) keeps the board on when USB is disconnected or the
    // power button is released. Default to high (on).
    espressif_board_reset_pin_number(33);
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
