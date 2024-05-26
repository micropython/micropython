// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "shared-bindings/usb_host/Port.h"

// These pins should never ever be reset; doing so could interfere with basic operation.
// Used in common-hal/microcontroller/Pin.c
const mcu_pin_obj_t *mimxrt10xx_reset_forbidden_pins[] = {
    // SWD Pins
    &pin_GPIO_AD_B0_06, // SWDIO
    &pin_GPIO_AD_B0_07, // SWCLK

    // FLEX flash
    &pin_GPIO_SD_B1_00,
    &pin_GPIO_SD_B1_01,
    &pin_GPIO_SD_B1_02,
    &pin_GPIO_SD_B1_03,
    &pin_GPIO_SD_B1_04,
    &pin_GPIO_SD_B1_05,
    &pin_GPIO_SD_B1_06,
    &pin_GPIO_SD_B1_07,
    &pin_GPIO_SD_B1_08,
    &pin_GPIO_SD_B1_09,
    &pin_GPIO_SD_B1_10,
    &pin_GPIO_SD_B1_11,

    // USB Pins
    &pin_GPIO_AD_B0_01, // ID Pin
    &pin_GPIO_AD_B0_03, // OC/Fault Pin
    NULL,                       // Must end in NULL.
};

void board_init(void) {
    common_hal_usb_host_port_construct(&pin_USB_OTG2_DP, &pin_USB_OTG2_DN);
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
