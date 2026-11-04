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
        gpio_config_t cfg = {
            .pin_bit_mask = BIT64(pin_number),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = false,
            .pull_down_en = true,
            .intr_type = GPIO_INTR_DISABLE,
        };
        gpio_config(&cfg);
        return true;
    }

    return false;
}

void board_init(void) {
    // Set this pin to its default state at boot. reset_pin_number() restores
    // that state when an object using the pin is deinitialized.
    //
    // GPIO13 (A6) defaults to an input with a pull-down.
    espressif_board_reset_pin_number(13);
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
