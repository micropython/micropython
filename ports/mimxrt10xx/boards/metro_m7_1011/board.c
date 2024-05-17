// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "shared-bindings/microcontroller/Pin.h"

// These pins should never ever be reset; doing so could interfere with basic operation.
// Used in common-hal/microcontroller/Pin.c
const mcu_pin_obj_t *mimxrt10xx_reset_forbidden_pins[] = {
    // SWD Pins
    &pin_GPIO_AD_13,// SWDIO
    &pin_GPIO_AD_12,// SWCLK

    // FLEX flash
    &pin_GPIO_SD_12,
    &pin_GPIO_SD_11,
    &pin_GPIO_SD_10,
    &pin_GPIO_SD_09,
    &pin_GPIO_SD_08,
    &pin_GPIO_SD_07,
    &pin_GPIO_SD_06,
    NULL,                       // Must end in NULL.
};

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.

bool mimxrt10xx_board_reset_pin_number(const mcu_pin_obj_t *pin) {
    #if CIRCUITPY_SWO_TRACE
    if (pin == &pin_GPIO_AD_09) {
        IOMUXC_SetPinMux( /* Add these lines*/
            IOMUXC_GPIO_AD_09_ARM_TRACE_SWO,
            0U);
        IOMUXC_SetPinConfig( /* Add these lines*/
            IOMUXC_GPIO_AD_09_ARM_TRACE_SWO,
            0x00F9U);
        return true;
    }
    #endif
    return false;
}
