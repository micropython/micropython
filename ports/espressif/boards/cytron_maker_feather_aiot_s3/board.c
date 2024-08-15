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
    gpio_set_direction(11, GPIO_MODE_DEF_OUTPUT);
    gpio_set_level(11, true);
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
