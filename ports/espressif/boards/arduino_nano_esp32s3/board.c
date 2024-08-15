// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "driver/gpio.h"

bool espressif_board_reset_pin_number(gpio_num_t pin_number) {
    if (pin_number == 13) {
        // Set D13 LED to input when not in use
        gpio_set_direction(pin_number, GPIO_MODE_DEF_INPUT);
        gpio_set_pull_mode(pin_number, GPIO_PULLDOWN_ONLY);
        return true;
    }

    return false;
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
