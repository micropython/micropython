/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
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
#include "shared-bindings/microcontroller/Pin.h"

#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"

uint8_t display_init_sequence[] = {
    0x01, 0x80, 0x96, //  _SWRESET and Delay 150ms
    0x11, 0x80, 0xFF, //  _SLPOUT and Delay 500ms
    0x3A, 0x81, 0x55, 0x0A, //  _COLMOD and Delay 10ms
    0x21, 0x80, 0x0A, //  _INVON
    0x13, 0x80, 0x0A, //  _NORON and Delay 10ms
    0x36, 0x01, 0xA0, //  _MADCTL
    0x29, 0x80, 0xFF, //  _DISPON and Delay 500ms
};

void board_init(void) {
    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    busio_spi_obj_t *spi = &bus->inline_bus;
    common_hal_busio_spi_construct(spi, &pin_GPIO7, &pin_GPIO6, NULL, false);
    common_hal_busio_spi_never_reset(spi);

    bus->base.type = &fourwire_fourwire_type;
    common_hal_fourwire_fourwire_construct(bus,
        spi,
        &pin_GPIO4, // TFT_DC Command or data
        &pin_GPIO5, // TFT_CS Chip select
        &pin_GPIO48, // TFT_RST Reset
        60000000, // Baudrate
        0, // Polarity
        0); // Phase

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
        &pin_GPIO45,  // backlight pin
        NO_BRIGHTNESS_COMMAND,
        1.0f, // brightness
        false, // single_byte_bounds
        false, // data_as_commands
        true, // auto_refresh
        60, // native_frames_per_second
        false, // backlight_on_high
        false, // SH1107_addressing
        50000); // backlight pwm frequency
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
