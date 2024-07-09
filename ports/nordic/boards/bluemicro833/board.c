// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "py/obj.h"
#include "peripherals/nrf/pins.h"
#include "supervisor/shared/board.h"

#include "nrf_gpio.h"

void board_init(void) {
    // "never_reset" the pin here because CircuitPython will try to reset pins after a VM run otherwise.
    never_reset_pin_number(POWER_SWITCH_PIN->number);
    // Turn on power to sensors and neopixels.
    nrf_gpio_cfg(POWER_SWITCH_PIN->number,
        NRF_GPIO_PIN_DIR_OUTPUT,
        NRF_GPIO_PIN_INPUT_DISCONNECT,
        NRF_GPIO_PIN_NOPULL,
        NRF_GPIO_PIN_S0S1,
        NRF_GPIO_PIN_NOSENSE);
    nrf_gpio_pin_write(POWER_SWITCH_PIN->number, true);
}

void board_deinit(void) {
    // Turn off power to sensors and neopixels.
    nrf_gpio_cfg(POWER_SWITCH_PIN->number,
        NRF_GPIO_PIN_DIR_OUTPUT,
        NRF_GPIO_PIN_INPUT_DISCONNECT,
        NRF_GPIO_PIN_NOPULL,
        NRF_GPIO_PIN_S0S1,
        NRF_GPIO_PIN_NOSENSE);
    nrf_gpio_pin_write(POWER_SWITCH_PIN->number, false);
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
