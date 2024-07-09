// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "common-hal/microcontroller/Pin.h"
#include "hal/include/hal_gpio.h"
#include "supervisor/shared/external_flash/external_flash.h"

void external_flash_setup(void) {
    // Do not reset the external flash write-protect and hold pins high
    never_reset_pin_number(PIN_PB22);
    never_reset_pin_number(PIN_PB23);

    // note: using output instead of input+pullups because the pullups are a little weak
    // Set the WP pin high
    gpio_set_pin_function(PIN_PB22, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_direction(PIN_PB22, GPIO_DIRECTION_OUT);
    gpio_set_pin_level(PIN_PB22, true);

    // Set the HOLD pin high
    gpio_set_pin_function(PIN_PB23, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_direction(PIN_PB23, GPIO_DIRECTION_OUT);
    gpio_set_pin_level(PIN_PB23, true);
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
