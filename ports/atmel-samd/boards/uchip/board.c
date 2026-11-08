// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"

#include "common-hal/microcontroller/Pin.h"
#include "mpconfigboard.h"

#include "hal/include/hal_gpio.h"

void board_init(void) {
    // BOOST_ENABLE
    // VEXT_SELECT
    // USB_DETECT
    // USB_HOST_EN
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
