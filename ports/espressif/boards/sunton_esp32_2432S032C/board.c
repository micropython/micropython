// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"
#include "shared-bindings/board/__init__.h"
#include "supervisor/shared/board.h"

#include "shared-module/displayio/mipi_constants.h"
#include "driver/gpio.h"
#include "common-hal/microcontroller/Pin.h"

#define DELAY 0x80

// ST7789
uint8_t display_init_sequence[] = {
    0x01, 0 | DELAY, 0x96,  // _SWRESET and Delay 150ms
    0x11, 0 | DELAY, 0xFF,  // _SLPOUT and Delay 500ms
    0x3A, 0x81, 0x55, 0x0A,  // _COLMOD and Delay 10ms
    0x36, 0x01, 0x08,  // _MADCTL (BGR)
    0x21, 0 | DELAY, 0x0A,  // _INVON Hack and Delay 10ms
    0x13, 0 | DELAY, 0x0A,  // _NORON and Delay 10ms
    0x36, 0x01, 0xA0,  // _MADCTL (Landscape)
    0xE0, 0x0E, 0xD0, 0x00, 0x02, 0x07, 0x0A, 0x28, 0x32, 0x44,
    0x42, 0x06, 0x0E, 0x12, 0x14, 0x17,  // gamma-curve positive
    0xE1, 0x0E, 0xD0, 0x00, 0x02, 0x07, 0x0A, 0x28, 0x31, 0x54,
    0x47, 0x0E, 0x1C, 0x17, 0x1B, 0x1E,  // gamma-curve negative
    0x29, 0 | DELAY, 0xFF,  // _DISPON and Delay 500ms
};

static void display_init(void) {
    busio_spi_obj_t *spi = common_hal_board_create_spi(0);
    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    bus->base.type = &fourwire_fourwire_type;

    common_hal_fourwire_fourwire_construct(
        bus,
        spi,
        &pin_GPIO2,  // TFT_DC
        &pin_GPIO15,  // TFT_CS
        NULL,  // TFT_RST
        26600000, // Baudrate
        0, // Polarity
        0 // Phase
        );

    busdisplay_busdisplay_obj_t *display = &allocate_display()->display;
    display->base.type = &busdisplay_busdisplay_type;

    common_hal_busdisplay_busdisplay_construct(display,
        bus,
        320, // Width
        240, // Height
        0, // column start
        0, // row start
        0, // rotation
        16, // Color depth
        false, // Grayscale
        false, // pixels in a byte share a row. Only valid for depths < 8
        1, // bytes per cell. Only valid for depths < 8
        false, // reverse_pixels_in_byte. Only valid for depths < 8
        true, // reverse_pixels_in_word
        MIPI_COMMAND_SET_COLUMN_ADDRESS, // Set column command
        MIPI_COMMAND_SET_PAGE_ADDRESS, // Set row command
        MIPI_COMMAND_WRITE_MEMORY_START, // Write memory command
        display_init_sequence,
        sizeof(display_init_sequence),
        &pin_GPIO27,  // backlight pin
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
    display_init();
}

bool espressif_board_reset_pin_number(gpio_num_t pin_number) {
    // Pull the speaker pin low to reduce noise on reset
    if (pin_number == 26) {
        // Turn on audio
        gpio_set_direction(pin_number, GPIO_MODE_DEF_OUTPUT);
        gpio_set_level(pin_number, false);
        return true;
    }
    return false;
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
