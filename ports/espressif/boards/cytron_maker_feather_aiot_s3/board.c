// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Wai Weng for Cytron Technologies
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "driver/gpio.h"
#include "common-hal/microcontroller/Pin.h"

void board_init(void) {
    // Most of these pins drive active-high LEDs. Default to pulled down (off).
    //
    // GPIO13 (VOLTAGE_MONITOR) measures the battery voltage. Default to no
    // pull.
    static const gpio_num_t pins_to_reset[] = {
        4, 5, 6, 7, 8, 9, 10, 12, 13, 14, 15, 16, 17, 18, 21, 38, 39, 40, 41, 42, 47, 48,
    };
    for (size_t i = 0; i < MP_ARRAY_SIZE(pins_to_reset); i++) {
        espressif_board_reset_pin_number(pins_to_reset[i]);
    }

    reset_board();
}

bool espressif_board_reset_pin_number(gpio_num_t pin_number) {
    // For GPIOs used in Maker Feather AIoT S3, set the default state to pull down
    // as most of them are connected to active high LED.
    switch ((int8_t)pin_number) {
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 12:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 21:
        case 38:
        case 39:
        case 40:
        case 41:
        case 42:
        case 47:
        case 48:
            gpio_reset_pin(pin_number);
            gpio_pullup_dis(pin_number);
            gpio_pulldown_en(pin_number);
            return true;

        // Do not pull up/down as this is the battery voltage monitoring pin.
        case 13:
            gpio_reset_pin(pin_number);
            gpio_pullup_dis(pin_number);
            gpio_pulldown_dis(pin_number);
            return true;
    }

    return false;
}

void reset_board(void) {
    // Turn on VP by default.
    config_pin_as_output_with_level(11, true);
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
