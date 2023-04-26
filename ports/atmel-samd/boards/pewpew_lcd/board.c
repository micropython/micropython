/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include "shared-bindings/board/__init__.h"
#include "shared-bindings/displayio/FourWire.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"
#include "shared-bindings/busio/SPI.h"

displayio_fourwire_obj_t board_display_obj;

#define DELAY 0x80

uint8_t display_init_sequence[] = {
    0xe2, 0,  // reset
    0x2f, 0, // power on
    0x80, 0,  // contrast 0
    0xa4, 0,  // display normal
    0xaf, 0,  // display on
    0x40, 0,  // start line 0
};

void board_init(void) {
    displayio_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    busio_spi_obj_t *spi = &bus->inline_bus;
    common_hal_busio_spi_construct(spi, &pin_PA23, &pin_PA22, NULL, false);
    common_hal_busio_spi_never_reset(spi);

    bus->base.type = &displayio_fourwire_type;
    common_hal_displayio_fourwire_construct(bus,
        spi,
        NULL, // Command or data
        &pin_PA19, // Chip select
        &pin_PA18, // Reset
        40000000LL, // Baudrate
        0, // Polarity
        0); // Phase

    displayio_display_obj_t *display = &allocate_display()->display;
    display->base.type = &displayio_display_type;
    common_hal_displayio_display_construct(display,
        bus,
        96, // Width
        68, // Height
        0, // column start
        0, // row start
        180, // rotation
        1, // Color depth
        true, // grayscale
        false, // pixels in byte share row. Only used with depth < 8
        1, // bytes per cell. Only valid for depths < 8
        false, // reverse_pixels_in_byte. Only valid for depths < 8
        false, // reverse_pixels_in_word
        0, // Set column command
        0, // Set row command
        0, // Write memory command
        display_init_sequence,
        sizeof(display_init_sequence),
        NULL, // &pin_PA17, // brightness pin
        NO_BRIGHTNESS_COMMAND,
        0.0f, // brightness
        false, // single_byte_bounds
        true, // data as commands
        true, // auto_refresh
        2, // native_frames_per_second
        true, // backlight_on_high
        true, // SH1107_addressing
        50000); // backlight pwm frequency
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
