// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
// SPDX-FileCopyrightText: Copyright (c) 2021 skieast/Bruce Segal
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/microcontroller/Pin.h"
#include "supervisor/board.h"

#include "components/driver/gpio/include/driver/gpio.h"
#include "soc/usb_serial_jtag_struct.h"

void board_init(void) {
    // This board has LEDs connected to the USB pins
    USB_SERIAL_JTAG.conf0.usb_pad_enable = 0;
    USB_SERIAL_JTAG.conf0.dp_pullup = 0;
}

bool espressif_board_reset_pin_number(gpio_num_t pin_number) {
    // Pull LEDs down on reset rather than the default up
    if (pin_number == 3 || pin_number == 4 || pin_number == 5 || pin_number == 18 || pin_number == 19) {
        gpio_config_t cfg = {
            .pin_bit_mask = BIT64(pin_number),
            .mode = GPIO_MODE_DISABLE,
            .pull_up_en = false,
            .pull_down_en = true,
            .intr_type = GPIO_INTR_DISABLE,
        };
        gpio_config(&cfg);
        return true;
    }
    return false;
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
