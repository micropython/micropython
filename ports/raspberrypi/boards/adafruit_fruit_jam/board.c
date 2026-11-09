// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "common-hal/microcontroller/Pin.h"
#include "hardware/gpio.h"
#include "py/mphal.h"
#include "shared-bindings/usb_host/Port.h"
#include "supervisor/board.h"

#include "common-hal/picodvi/__init__.h"

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.

#define I2S_RESET_PIN_NUMBER 22

bool board_reset_pin_number(uint8_t pin_number) {
    #if defined(DEFAULT_USB_HOST_5V_POWER)
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
    #endif
    // Set I2S out of reset.
    if (pin_number == I2S_RESET_PIN_NUMBER) {
        gpio_put(pin_number, 1);
        gpio_set_dir(pin_number, GPIO_OUT);
        gpio_set_function(pin_number, GPIO_FUNC_SIO);

        return true;
    }
    return false;
}

void board_init(void) {
    // Reset the DAC to put it in a known state.
    gpio_put(I2S_RESET_PIN_NUMBER, 0);
    gpio_set_dir(I2S_RESET_PIN_NUMBER, GPIO_OUT);
    gpio_set_function(I2S_RESET_PIN_NUMBER, GPIO_FUNC_SIO);
    mp_hal_delay_us(1);
    board_reset_pin_number(I2S_RESET_PIN_NUMBER);

    #if defined(DEFAULT_USB_HOST_5V_POWER)
    // GPIO11 (USB_HOST_5V_POWER) powers the USB host port.
    // Default to high (on).
    board_reset_pin_number(DEFAULT_USB_HOST_5V_POWER->number);
    #endif

    #if defined(DEFAULT_USB_HOST_DATA_PLUS)
    common_hal_usb_host_port_construct(DEFAULT_USB_HOST_DATA_PLUS, DEFAULT_USB_HOST_DATA_MINUS);
    #endif

    picodvi_autoconstruct();
}
