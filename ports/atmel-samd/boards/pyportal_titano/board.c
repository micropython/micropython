/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
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

#include "boards/board.h"
#include "mpconfigboard.h"
#include "hal/include/hal_gpio.h"

#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/displayio/FourWire.h"

#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"

#include "tick.h"

#define DELAY 0x80

uint8_t display_init_sequence[] = {
    0x01, 2, 0x80, 0x64, // swreset
    0xB9, 5, 0x83, 0xFF, 0x83, 0x57, 0xFF,
    0xB3, 5, 0x04, 0x80, 0x00, 0x06, 0x06,
    0xB6, 2, 0x01, 0x25,
    0xCC, 2, 0x01, 0x05,
    0xB1, 7,
        0x06, 0x00, 0x15, 0x1C, 0x1C, 0x83, 0xAA,
    0xC0, 7,
        0x06, 0x50, 0x50, 0x01, 0x3C, 0x1E, 0x08,
    0xB4, 8,
        0x07, 0x02, 0x40, 0x00, 0x2A, 0x2A, 0x0D, 0x78,
    0xE0, 34,
      0x02, 0x0A, 0x11, 0x1d, 0x23, 0x35, 0x41, 0x4b, 0x4b,
      0x42, 0x3A, 0x27, 0x1B, 0x08, 0x09, 0x03, 0x02, 0x0A,
      0x11, 0x1d, 0x23, 0x35, 0x41, 0x4b, 0x4b, 0x42, 0x3A,
      0x27, 0x1B, 0x08, 0x09, 0x03, 0x00, 0x01,
    0x3a, 1, 0x55,
    0x36, 1, 0xC0,
    0x35, 1, 0x00, 
    0x44, 2, 0x00, 0x02,
    0x11, 0x80 + 150/5, // Exit Sleep, then delay 150 ms
    0x29, 0x80 +  50/5
};

void board_init(void) {
    busio_spi_obj_t* spi = &displays[0].fourwire_bus.inline_bus;
    common_hal_busio_spi_construct(spi, &pin_PA13, &pin_PA12, &pin_PA14);
    common_hal_busio_spi_never_reset(spi);
    
    displayio_fourwire_obj_t* bus = &displays[0].fourwire_bus;
    bus->base.type = &displayio_fourwire_type;
    common_hal_displayio_fourwire_construct(bus,
        spi,
        &pin_PB05, // TFT_DC Command or data
        &pin_PB06, // TFT_CS Chip select
        &pin_PA00, // TFT_RST Reset
        24000000);

    displayio_display_obj_t* display = &displays[0].display;
    display->base.type = &displayio_display_type;
    common_hal_displayio_display_construct(display,
        bus,
        320, // Width
        480, // Height
        0, // column start
        0, // row start
        0, // rotation
        16, // Color depth
        false, // grayscale
        false, // pixels_in_byte_share_row (unused for depths > 8)
        1, // bytes per cell. Only valid for depths < 8
        false, // reverse_pixels_in_byte. Only valid for depths < 8
        MIPI_COMMAND_SET_COLUMN_ADDRESS, // Set column command
        MIPI_COMMAND_SET_PAGE_ADDRESS, // Set row command
        MIPI_COMMAND_WRITE_MEMORY_START, // Write memory command
        0x37, // Set vertical scroll command
        display_init_sequence,
        sizeof(display_init_sequence),
        &pin_PB31, // Backlight pin
        NO_BRIGHTNESS_COMMAND,
        1.0f, // brightness (ignored)
        true, // auto_brightness
        false, // single_byte_bounds
        false); // data_as_commands
}

bool board_requests_safe_mode(void) {
    return false;
}

void reset_board(void) {
}
