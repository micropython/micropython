// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 ThePiHut
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "driver/gpio.h"

void board_init(void) {
    reset_board();
}

void reset_board(void) {
    // Board-specific initialization
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
