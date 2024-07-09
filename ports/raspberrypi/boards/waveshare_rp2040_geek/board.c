// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"


#include "mpconfigboard.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"
#include "shared-bindings/board/__init__.h"

#define DELAY 0x80


// display init sequence according to https://github.com/adafruit/Adafruit_CircuitPython_ST7789
uint8_t display_init_sequence[] = {
    0x01, 0 | DELAY, 0x96,  // _SWRESET and Delay 150ms
    0x11, 0 | DELAY, 0xFF,  // _SLPOUT and Delay 500ms
    0x3A, 0x81, 0x55, 0x0A,  // _COLMOD and Delay 10ms
    0x36, 0x01, 0x08,  // _MADCTL
    0x21, 0 | DELAY, 0x0A,  // _INVON Hack and Delay 10ms
    0x13, 0 | DELAY, 0x0A,  // _NORON and Delay 10ms
    0x36, 0x01, 0xC0,  // _MADCTL
    0x29, 0 | DELAY, 0xFF,  // _DISPON and Delay 500ms
};

static void display_init(void) {

    busio_spi_obj_t *spi = common_hal_board_create_spi(0);
    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;

    bus->base.type = &fourwire_fourwire_type;

    common_hal_fourwire_fourwire_construct(
        bus,
        spi,
        &pin_GPIO8,  // TFT_DC
        &pin_GPIO9,  // TFT_CS
        &pin_GPIO12,  // TFT_RST
        50000000, // Baudrate
        0, // Polarity
        0 // Phase

        );

    busdisplay_busdisplay_obj_t *display = &allocate_display()->display;
    display->base.type = &busdisplay_busdisplay_type;

    common_hal_busdisplay_busdisplay_construct(
        display,
        bus,
        240, // Width
        135, // Height
        53, // column start
        40, // row start
        270, // rotation
        16, // Color depth
        false, // Grayscale
        false, // Pixels in a byte share a row
        1, // bytes per cell
        false, // reverse_pixels_in_byte
        true, // reverse_pixels_in_word
        MIPI_COMMAND_SET_COLUMN_ADDRESS, // set column command
        MIPI_COMMAND_SET_PAGE_ADDRESS, // set row command
        MIPI_COMMAND_WRITE_MEMORY_START, // write memory command
        display_init_sequence,
        sizeof(display_init_sequence),
        &pin_GPIO25, // backlight pin
        NO_BRIGHTNESS_COMMAND,
        1.0f, // brightness
        false, // single_byte_bounds
        false, // data_as_commands
        true, // auto_refresh
        60, // native_frames_per_second
        true, // backlight_on_high
        false, // SH1107_addressing
        1000 // backlight pwm frequency
        );
}

void board_init(void) {
    display_init();
}
