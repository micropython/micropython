// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "common-hal/microcontroller/Pin.h"
#include "supervisor/shared/board.h"
#include "hal/include/hal_gpio.h"

void reset_board(void) {
    board_reset_user_neopixels(&pin_PA05, 4);
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
