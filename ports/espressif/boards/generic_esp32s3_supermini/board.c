// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "supervisor/board.h"
#include "supervisor/shared/settings.h"
#include "supervisor/shared/status_leds.h"

void board_init(void) {
    #if CIRCUITPY_SETTINGS_TOML
    // Clones of this board differ in ways the chip can't report.
    // CIRCUITPY_BOARD_VARIANT in settings.toml picks one:
    //   "rgb_led": status NeoPixel takes R,G,B
    char variant[16];
    if (settings_get_str("CIRCUITPY_BOARD_VARIANT", variant, sizeof(variant)) == SETTINGS_OK &&
        strcmp(variant, "rgb_led") == 0) {
        status_led_set_neopixel_rgb_order(true);
    }
    #endif
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
