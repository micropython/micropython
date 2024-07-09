// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "supervisor/board.h"
#include "py/mpconfig.h"

#include "common-hal/digitalio/DigitalInOut.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/neopixel_write/__init__.h"

void reset_board(void) {
    uint8_t zeroes[96];
    memset(zeroes, 0, 96);
    digitalio_digitalinout_obj_t neopixel;
    common_hal_digitalio_digitalinout_construct(&neopixel, &pin_PA27);
    common_hal_digitalio_digitalinout_switch_to_output(&neopixel, false, DRIVE_MODE_PUSH_PULL);
    common_hal_neopixel_write(&neopixel, zeroes, 96);
    common_hal_digitalio_digitalinout_deinit(&neopixel);
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
