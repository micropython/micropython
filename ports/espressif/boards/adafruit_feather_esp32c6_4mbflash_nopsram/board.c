// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "driver/gpio.h"

bool espressif_board_reset_pin_number(gpio_num_t pin_number) {
    if (pin_number == 20) {
        // Turn on I2C power by default.
        config_pin_as_output_with_level(pin_number, true);
        return true;
    }

    return false;
}

void board_init(void) {
    // GPIO20 (NEOPIXEL_I2C_POWER) powers the NeoPixel and the STEMMA QT
    // connector. Default to high (on). After a chip reset the pin is an input
    // with no pull-up, which leaves them unpowered.
    espressif_board_reset_pin_number(20);
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
