// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "common-hal/microcontroller/Pin.h"
#include "hardware/gpio.h"
#include "shared-bindings/usb_host/Port.h"
#include "supervisor/board.h"

#include "common-hal/picodvi/__init__.h"

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.


#if defined(DEFAULT_USB_HOST_5V_POWER)
bool board_reset_pin_number(uint8_t pin_number) {
    if (pin_number == DEFAULT_USB_HOST_5V_POWER->number) {
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
#endif

void board_init(void) {
    #if defined(DEFAULT_USB_HOST_5V_POWER)
    // GPIO29 (USB_HOST_5V_POWER) powers the USB host port.
    // Default to high (on).
    board_reset_pin_number(DEFAULT_USB_HOST_5V_POWER->number);
    #endif

    #if defined(DEFAULT_USB_HOST_DATA_PLUS)
    common_hal_usb_host_port_construct(DEFAULT_USB_HOST_DATA_PLUS, DEFAULT_USB_HOST_DATA_MINUS);
    #endif
    picodvi_autoconstruct();
}
