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
#include "shared-module/displayio/mipi_constants.h"
#include "shared-bindings/board/__init__.h"


uint8_t display_init_sequence[] = {
    0xFE, 0x00, // Inter Register Enable1 (FEh)
    0xEF, 0x00, // Inter Register Enable2 (EFh)
    0xB6, 0x02, 0x00, 0x00, // Display Function Control (B6h) [S1→S360 source, G1→G32 gate]
    0x36, 0x01, 0x48, // Memory Access Control(36h) [Invert Row order, invert vertical scan order]
    0x3a, 0x01, 0x55, // COLMOD: Pixel Format Set (3Ah) [16 bits / pixel]
    0xC3, 0x01, 0x13, // Power Control 2 (C3h) [VREG1A = 5.06, VREG1B = 0.68]
    0xC4, 0x01, 0x13, // Power Control 3 (C4h) [VREG2A = -3.7, VREG2B = 0.68]
    0xC9, 0x01, 0x22, // Power Control 4 (C9h)
    0xF0, 0x06, 0x45, 0x09, 0x08, 0x08, 0x26, 0x2a, // SET_GAMMA1 (F0h)
    0xF1, 0x06, 0x43, 0x70, 0x72, 0x36, 0x37, 0x6f, // SET_GAMMA2 (F1h)
    0xF2, 0x06, 0x45, 0x09, 0x08, 0x08, 0x26, 0x2a, // SET_GAMMA3 (F2h)
    0xF3, 0x06, 0x43, 0x70, 0x72, 0x36, 0x37, 0x6f, // SET_GAMMA4 (F3h)

    0x66, 0x0a, 0x3c, 0x00, 0xcd, 0x67, 0x45, 0x45, 0x10, 0x00, 0x00, 0x00,
    0x67, 0x0a, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x01, 0x54, 0x10, 0x32, 0x98,
    0x74, 0x07, 0x10, 0x85, 0x80, 0x00, 0x00, 0x4e, 0x00,
    0x98, 0x02, 0x3e, 0x07,

    0x35, 0x00, // Tearing Effect Line ON (35h) [both V-blanking and H-blanking]
    0x21, 0x00, // Display Inversion ON (21h)
    0x11, 0x80, 0x78, // Sleep Out Mode (11h) and delay(120)
    0x29, 0x80, 0x14, // Display ON (29h) and delay(20)
    0x2a, 0x04, 0x00, 0x00, 0x00, 239, // Column Address Set (2Ah) [Start col = 0, end col = 239]
    0x2b, 0x04, 0x00, 0x00, 0x00, 239, // Row Address Set (2Bh) [Start row = 0, end row = 239]
};

void board_init(void) {
    busio_spi_obj_t *spi = common_hal_board_create_spi(0);
    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    bus->base.type = &fourwire_fourwire_type;

    common_hal_fourwire_fourwire_construct(
        bus,
        spi,
        &pin_GPIO4,     // DC
        &pin_GPIO7,     // CS
        &pin_GPIO8,     // RST
        40000000,       // baudrate
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
        0,              // row start
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
        &pin_GPIO9,     // backlight pin
        NO_BRIGHTNESS_COMMAND,
        0.8f,           // brightness
        false,          // single_byte_bounds
        false,          // data_as_commands
        true,           // auto_refresh
        60,             // native_frames_per_second
        true,           // backlight_on_high
        false,          // SH1107_addressing
        50000           // backlight pwm frequency
        );
}

bool espressif_board_reset_pin_number(gpio_num_t pin_number) {
    // Hold pind must be set high to avoid a power off when battery powered
    if (pin_number == 46) {
        // Turn on hold output
        gpio_set_direction(46, GPIO_MODE_DEF_OUTPUT);
        gpio_set_level(46, true);
        return true;
    }
    return false;
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.

// TODO: Should we turn off the display when asleep, in board_deinit() ?
