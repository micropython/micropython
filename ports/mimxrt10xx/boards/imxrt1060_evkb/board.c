/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2019 Artur Pacholec
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

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
    NULL,               // Must end in NULL.
};

void board_init(void) {
    common_hal_usb_host_port_construct(&pin_USB_OTG2_DP, &pin_USB_OTG2_DN);
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
