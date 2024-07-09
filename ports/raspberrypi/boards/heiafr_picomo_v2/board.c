// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"
#include "supervisor/shared/board.h"


#define DELAY 0x80

uint8_t display_init_sequence[] = {
    0x01, 0 | DELAY, 150, // SWRESET

    0x36, 1, 0x04, // MADCTL
    0x35, 1, 0x00, // TEON
    0xB2, 5, 0x0c, 0x0c, 0x00, 0x33, 0x33, // FRMCTR2
    0x3A, 1, 0x05, // COLMOD
    0xB7, 1, 0x14, // GCTRL
    0xBB, 1, 0x37, // VCOMS
    0xC0, 1, 0x2c, // LCMCTRL
    0xC2, 1, 0x01, // VDVVRHEN
    0xC3, 1, 0x12, // VRHS
    0xC4, 1, 0x20, // VDVS
    0xD0, 2, 0xa4, 0xa1, // PWRCTRL1
    0xC6, 1, 0x0f, // FRCTRL2
    0xE0, 14, 0xd0, 0x04, 0x0d, 0x11, 0x13, 0x2b, 0x3f, 0x54, 0x4c, 0x18, 0x0d, 0x0b, 0x1f, 0x23, // GMCTRP1
    0xE1, 14, 0xd0, 0x04, 0x0c, 0x11, 0x13, 0x2c, 0x3f, 0x44, 0x51, 0x2f, 0x1f, 0x1f, 0x20, 0x23, // GMCTRN1
    0x21, 0, // INVON

    0x11, 0 | DELAY, 255, // SLPOUT
    0x29, 0 | DELAY, 100, // DISPON

    0x2a, 4, 0x00, 0, 0x00, 0xfe, // CASET
    0x2b, 4, 0x00, 0, 0x00, 0xfe, // RASET
    0x2c, 0, // RAMWR
};

void board_init(void) {
    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    busio_spi_obj_t *spi = &bus->inline_bus;
    common_hal_busio_spi_construct(spi, &pin_GPIO18, &pin_GPIO19, NULL, false);
    common_hal_busio_spi_never_reset(spi);

    bus->base.type = &fourwire_fourwire_type;
    common_hal_fourwire_fourwire_construct(bus,
        spi,
        &pin_GPIO16, // TFT_DC Command or data
        &pin_GPIO17, // TFT_CS Chip select
        NULL, // TFT_RST Reset
        62500000, // Baudrate
        0, // Polarity
        0); // Phase

    busdisplay_busdisplay_obj_t *display = &allocate_display()->display;
    display->base.type = &busdisplay_busdisplay_type;
    common_hal_busdisplay_busdisplay_construct(display,
        bus,
        240, // Width
        280, // Height
        0, // column start
        20, // row start
        0, // rotation
        16, // Color depth
        false, // Grayscale
        false, // pixels in a byte share a row. Only valid for depths < 8
        1, // bytes per cell. Only valid for depths < 8
        false, // reverse_pixels_in_byte. Only valid for depths < 8
        true, // reverse_bytes_in_word
        MIPI_COMMAND_SET_COLUMN_ADDRESS, // Set column command
        MIPI_COMMAND_SET_PAGE_ADDRESS, // Set row command
        MIPI_COMMAND_WRITE_MEMORY_START, // Write memory command
        display_init_sequence,
        sizeof(display_init_sequence),
        NULL, // no backlight pin
        NO_BRIGHTNESS_COMMAND,
        1.0f, // brightness
        false, // single_byte_bounds
        false, // data_as_commands
        true, // auto_refresh
        60, // native_frames_per_second
        true, // backlight_on_high
        false, // SH1107_addressing
        0); // backlight pwm frequency
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
