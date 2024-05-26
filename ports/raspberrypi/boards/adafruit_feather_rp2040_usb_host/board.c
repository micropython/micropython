// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"

#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/usb_host/Port.h"
#include "hardware/gpio.h"

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.

digitalio_digitalinout_obj_t _host_power;

bool board_reset_pin_number(uint8_t pin_number) {
    if (pin_number == 18) {
        // doing this (rather than gpio_init) in this specific order ensures no
        // glitch if pin was already configured as a high output. gpio_init() temporarily
        // configures the pin as an input, so the power enable value would potentially
        // glitch.
        gpio_put(pin_number, 1);
        gpio_set_dir(pin_number, GPIO_OUT);
        gpio_set_function(pin_number, GPIO_FUNC_SIO);

        return true;
    }
    return false;
}
void board_init(void) {
    common_hal_usb_host_port_construct(&pin_GPIO16, &pin_GPIO17);
}
