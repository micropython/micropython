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

// display init sequence according to LilyGO example app
uint8_t display_init_sequence[] = {
    // sw reset
    0x01, 0 | DELAY, 150,
    // sleep out
    0x11, 0 | DELAY, 255,
    // colmod and delay 10ms
    0x3A, 1 | DELAY,  0x55, 10,
    0x21, 0 | DELAY, 10,
    // normal display mode on
    0x13, 0 | DELAY, 10,
    // madctl display and color format settings
    0x36, 1, 0x60,
    // ST7789V gamma setting
    // 0xE0, 14, 0xD0, 0x00, 0x02, 0x07, 0x0A, 0x28, 0x32, 0x44, 0x42, 0x06, 0x0E, 0x12, 0x14, 0x17,
    // 0xE1, 14, 0xD0, 0x00, 0x02, 0x07, 0x0A, 0x28, 0x31, 0x54, 0x47, 0x0E, 0x1C, 0x17, 0x1B, 0x1E,

    // display on
    0x29, 0 | DELAY, 255,
};

static void display_init(void) {

    gpio_set_direction(15, GPIO_MODE_DEF_OUTPUT);
    gpio_set_level(15, true);

    paralleldisplaybus_parallelbus_obj_t *bus = &allocate_display_bus()->parallel_bus;
    bus->base.type = &paralleldisplaybus_parallelbus_type;
    const mcu_pin_obj_t *data_pins[] = {
        &pin_GPIO39,
        &pin_GPIO40,
        &pin_GPIO41,
        &pin_GPIO42,
        &pin_GPIO45,
        &pin_GPIO46,
        &pin_GPIO47,
        &pin_GPIO48
    };
    common_hal_paralleldisplaybus_parallelbus_construct_nonsequential(bus,
        8, // num pins
        data_pins, // Data pins
        &pin_GPIO7, // Command or data
        &pin_GPIO6, // Chip select
        &pin_GPIO8, // Write
        &pin_GPIO9, // Read
        &pin_GPIO5, // Reset
        15000000); // Frequency. ST7789 datasheet says min clock cycle is 66ns which is ~15 mhz.

    busdisplay_busdisplay_obj_t *display = &allocate_display()->display;
    display->base.type = &busdisplay_busdisplay_type;
    common_hal_busdisplay_busdisplay_construct(display,
        bus,
        320, // Width
        170, // Height
        0, // column start
        35, // row start
        0, // rotation
        16, // Color depth
        false, // grayscale
        false, // pixels_in_byte_share_row (unused for depths > 8)
        1, // bytes per cell. Only valid for depths < 8
        false, // reverse_pixels_in_byte. Only valid for depths < 8
        true, // reverse_pixels_in_word
        MIPI_COMMAND_SET_COLUMN_ADDRESS, // Set column command
        MIPI_COMMAND_SET_PAGE_ADDRESS, // Set row command
        MIPI_COMMAND_WRITE_MEMORY_START, // Write memory command
        display_init_sequence,
        sizeof(display_init_sequence),
        &pin_GPIO38, // Backlight pin
        NO_BRIGHTNESS_COMMAND,
        1.0f, // brightness
        false, // single_byte_bounds
        false, // data_as_commands
        true, // auto_refresh
        60, // native_frames_per_second
        true, // backlight_on_high
        false, // SH1107_addressing
        50000); // backlight pwm frequency
}

void board_init(void) {
    // Display
    display_init();
}

bool espressif_board_reset_pin_number(gpio_num_t pin_number) {
    // Override the I2C/TFT power pin reset to prevent resetting the display.
    if (pin_number == 15) {
        // Turn on TFT
        gpio_set_direction(pin_number, GPIO_MODE_DEF_OUTPUT);
        gpio_set_level(pin_number, true);
        return true;
    }
    return false;
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
