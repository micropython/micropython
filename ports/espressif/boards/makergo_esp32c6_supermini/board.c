// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 bill88t
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "driver/gpio.h"
#include "common-hal/microcontroller/Pin.h"

bool espressif_board_reset_pin_number(gpio_num_t pin_number) {
    if (pin_number == 15) {
        // Turn on the spare led on boot as a power indicator.
        config_pin_as_output_with_level(pin_number, true);
        return true;
    }
    return false;
}

void board_init(void) {
    // GPIO15 (LED) drives an LED used as a power indicator. Default to high
    // (on).
    espressif_board_reset_pin_number(15);
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
