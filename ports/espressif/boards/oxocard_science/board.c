// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"

#define DELAY 0x80

// display init sequence
uint8_t display_init_sequence[] = {
    0x01, DELAY, 0x96,  // _SWRESET and Delay 150ms
    0x11, DELAY, 0xFF,  // _SLPOUT and Delay 500ms
    0x3A, DELAY | 1, 0x55, 0x0A,  // _COLMOD (65K of RGB, 16 bits/pixel) and Delay 10ms
    0x36, 0x01, 0x00,  // _MADCTL
    0x2A, 0x04, 0x00, 0x00, 0x00, 0xF0,  // _CASET (0..240)
    0x2B, 0x04, 0x00, 0x00, 0x00, 0xF0,  // _RASET (0..240)
    0x21, DELAY, 0x0A,  // _INVON Hack and Delay 10ms
    0x13, DELAY, 0x0A,  // _NORON and Delay 10ms
    0x29, DELAY, 0xFF,  // _DISPON and Delay 500ms
};

static void display_init(void) {
    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    busio_spi_obj_t *spi = &bus->inline_bus;

    common_hal_busio_spi_construct(
        spi,
        &pin_GPIO14,    // CLK
        &pin_GPIO13,    // MOSI
        &pin_GPIO12,    // MISO
        false);         // Not half-duplex

    common_hal_busio_spi_never_reset(spi);

    bus->base.type = &fourwire_fourwire_type;

    common_hal_fourwire_fourwire_construct(
        bus,
        spi,
        &pin_GPIO27,    // DC
        &pin_GPIO15,    // CS
        &pin_GPIO4,     // RST
        24000000,       // baudrate
        0,              // polarity
        0               // phase
        );

    busdisplay_busdisplay_obj_t *display = &allocate_display()->display;
    display->base.type = &busdisplay_busdisplay_type;

    common_hal_busdisplay_busdisplay_construct(
        display,
        bus,
        240,            // width (after rotation)
        240,            // height (after rotation)
        0,              // column start
        80,             // row start
        0,              // rotation
        16,             // color depth
        false,          // grayscale
        false,          // pixels in a byte share a row. Only valid for depths < 8
        1,              // bytes per cell. Only valid for depths < 8
        false,          // reverse_pixels_in_byte. Only valid for depths < 8
        true,           // reverse_pixels_in_word
        MIPI_COMMAND_SET_COLUMN_ADDRESS, // set column command
        MIPI_COMMAND_SET_PAGE_ADDRESS,   // set row command
        MIPI_COMMAND_WRITE_MEMORY_START, // write memory command
        display_init_sequence,
        sizeof(display_init_sequence),
        &pin_GPIO19,    // backlight pin
        NO_BRIGHTNESS_COMMAND,
        1.0f,           // brightness
        false,          // single_byte_bounds
        false,          // data_as_commands
        true,           // auto_refresh
        60,             // native_frames_per_second
        true,           // backlight_on_high
        false,          // SH1107_addressing
        50000           // backlight pwm frequency
        );
}

void board_init(void) {
    // Display
    display_init();
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
