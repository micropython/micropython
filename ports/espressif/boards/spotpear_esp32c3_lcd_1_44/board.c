// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 CDarius
//
// SPDX-License-Identifier: MIT
#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"

// display init sequence according to adafruit_st7735r.py library
uint8_t display_init_sequence[] = {
    0x01, 0x80, 0x96,             // SWRESET and Delay 150ms
    0x11, 0x80, 0xff,             // SLPOUT and Delay
    0xb1, 0x03, 0x01, 0x2C, 0x2D,   // _FRMCTR1
    0xb2, 0x03, 0x01, 0x2C, 0x2D,   // _FRMCTR2
    0xb3, 0x06, 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D, // _FRMCTR3
    0xb4, 0x01, 0x07,             // _INVCTR line inversion
    0xc0, 0x03, 0xa2, 0x02, 0x84,   // _PWCTR1 GVDD = 4.7V, 1.0uA
    0xc1, 0x01, 0xc5,             // _PWCTR2 VGH=14.7V, VGL=-7.35V
    0xc2, 0x02, 0x0a, 0x00,        // _PWCTR3 Opamp current small, Boost frequency
    0xc3, 0x02, 0x8a, 0x2a,
    0xc4, 0x02, 0x8a, 0xee,
    0xc5, 0x01, 0x0e,             // _VMCTR1 VCOMH = 4V, VOML = -1.1V
    0x36, 0x01, 0xc8,             // MADCTL Rotate display
    0x3a, 0x01, 0x05,             // COLMOD - 16bit color
    0xe0, 0x10, 0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d, 0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10, // _GMCTRP1 Gamma
    0xe1, 0x10, 0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10, // _GMCTRN1
    0x13, 0x80, 0x0a,             // _NORON
    0x29, 0x80, 0x64              // _DISPON
};

static bool display_init(void) {
    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    busio_spi_obj_t *spi = &bus->inline_bus;
    common_hal_busio_spi_construct(spi, &pin_GPIO3, &pin_GPIO4, NULL, false);
    common_hal_busio_spi_never_reset(spi);

    bus->base.type = &fourwire_fourwire_type;

    common_hal_fourwire_fourwire_construct(
        bus,
        spi,
        &pin_GPIO0,    // DC
        &pin_GPIO2,     // CS
        &pin_GPIO5,    // RST
        10000000,       // baudrate
        0,              // polarity
        0               // phase
        );

    busdisplay_busdisplay_obj_t *display = &allocate_display()->display;
    display->base.type = &busdisplay_busdisplay_type;

    common_hal_busdisplay_busdisplay_construct(
        display,
        bus,
        128,             // width (after rotation)
        128,            // height (after rotation)
        2,             // column start
        1,              // row start
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
        NULL,    // backlight pin
        NO_BRIGHTNESS_COMMAND,
        1.0f,           // brightness
        false,          // single_byte_bounds
        false,          // data_as_commands
        true,           // auto_refresh
        80,             // native_frames_per_second
        false,          // backlight_on_high
        false,          // SH1107_addressing
        50000           // backlight pwm frequency
        );

    return true;
}

void board_init(void) {
    display_init();
}
