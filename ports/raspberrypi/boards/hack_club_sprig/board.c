/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 ajs256
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/displayio/FourWire.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"
#include "supervisor/shared/board.h"

displayio_fourwire_obj_t board_display_obj;

// display init sequence from CircuitPython library https://github.com/adafruit/Adafruit_CircuitPython_ST7735R/blob/dfae353330cf051d1f31db9e4b681c8d70900cc5/adafruit_st7735r.py
uint8_t display_init_sequence[] = {
    // sw reset
    0x01, 0x80, 0x96,
    // sleep out and delay
    0x11, 0x80, 0xFF,
    // _FRMCTR1
    0xB1, 0x03, 0x01, 0x2C, 0x2D,
    // _FRMCTR2
    0xB2, 0x03, 0x01, 0x2C, 0x2D,
    // _FRMCTR3
    0xB3, 0x06, 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D,
    // _INVCTR line inversion
    0xB4, 0x01, 0x07,
    // _PWCTR1 GVDD = 4.7V, 1.0uA
    0xC0, 0x03, 0xA2, 0x02, 0x84,
    // _PWCTR2 VGH=14.7V, VGL=-7.35V
    0xC1, 0x01, 0xC5,
    // _PWCTR3 Opamp current small, Boost frequency
    0xC2, 0x02, 0x0A, 0x00,
    0xC3, 0x02, 0x8A, 0x2A,
    0xC4, 0x02, 0x8A, 0xEE,
    // _VMCTR1 VCOMH = 4V, VOML = -1.1V
    0xC5, 0x01, 0x0E,
    // _INVOFF
    0x20, 0x00,
    // _MADCTL bottom to top refresh
    0x36, 0x01, 0x18,
    // COLMOD - 16 bit color
    0x3A, 0x01, 0x05,
    // _GMCTRP1 Gamma
    0xE0, 0x10, 0x02, 0x1C, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2D, 0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10,
    // _GMCTRN1
    0xE1, 0x10, 0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10,
    // _NORON
    0x13, 0x80, 0x0A,
    // _DISPON
    0x29, 0x80, 0x64,
    // _MADCTL Default rotation + BGR encoding
    0x36, 0x01, 0xC0,
};


void board_init(void) {
    displayio_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    busio_spi_obj_t *spi = &bus->inline_bus;
    common_hal_busio_spi_construct(spi, &pin_GPIO18, &pin_GPIO19, &pin_GPIO16, false);
    common_hal_busio_spi_never_reset(spi);

    bus->base.type = &displayio_fourwire_type;
    common_hal_displayio_fourwire_construct(bus,
        spi,
        &pin_GPIO22, // DC
        &pin_GPIO20, // CS
        &pin_GPIO26, // RST
        30000000,
        0,
        0);

    displayio_display_obj_t *display = &allocate_display()->display;
    display->base.type = &displayio_display_type;
    common_hal_displayio_display_construct(display,
        bus,
        160, // Width
        128, // Height
        0, // column start
        0, // row start
        270, // rotation
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
        &pin_GPIO17,  // backlight pin
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

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
