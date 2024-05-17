// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/microcontroller/Pin.h"

void board_init(void) {
    // Crystal
    common_hal_never_reset_pin(&pin_GPIO15);
    common_hal_never_reset_pin(&pin_GPIO16);
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
