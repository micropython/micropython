// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "supervisor/shared/board.h"

#if CIRCUITPY_DIGITALIO && CIRCUITPY_NEOPIXEL_WRITE

#include <string.h>

#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/neopixel_write/__init__.h"

void board_reset_user_neopixels(const mcu_pin_obj_t *pin, size_t count) {
    // Turn off on-board NeoPixel string
    uint8_t empty[count * 3];
    memset(empty, 0, count * 3);
    digitalio_digitalinout_obj_t neopixel_pin;
    common_hal_digitalio_digitalinout_construct(&neopixel_pin, pin);
    common_hal_digitalio_digitalinout_switch_to_output(&neopixel_pin, false,
        DRIVE_MODE_PUSH_PULL);
    common_hal_neopixel_write(&neopixel_pin, empty, count * 3);
    common_hal_digitalio_digitalinout_deinit(&neopixel_pin);
}

#endif

// Do-nothing so not all boards need to provide this function.
MP_WEAK bool board_requests_safe_mode(void) {
    return false;
}

// Do-nothing so not all boards need to provide this function.
MP_WEAK void board_init(void) {
}

// Do-nothing so not all boards need to provide this function.
MP_WEAK void board_deinit(void) {
}

// Do-nothing so not all boards need to provide this function.
MP_WEAK void reset_board(void) {
}
