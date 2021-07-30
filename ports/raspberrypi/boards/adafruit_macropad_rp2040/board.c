/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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

#include "shared-bindings/board/__init__.h"
#include "shared-bindings/displayio/FourWire.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "src/rp2_common/hardware_gpio/include/hardware/gpio.h"
#include "supervisor/shared/board.h"

displayio_fourwire_obj_t board_display_obj;

#define DELAY 0x80

uint8_t display_init_sequence[] = {
    0xae, 0, // sleep
    0xd5, 1, 0x80, // fOsc divide by 2
    0xa8, 1, 0x3f, // multiplex 64
    0xd3, 1, 0x00, // offset 0
    0x40, 1, 0x00, // start line 0
    0xad, 1, 0x8b, // dc/dc on
    0xa1, 0, // segment remap = 0
    0xc8, 0, // scan incr
    0xda, 1, 0x12, // com pins
    0x81, 1, 0xff, // contrast 255
    0xd9, 1, 0x1f, // pre/dis-charge 2DCLKs/2CLKs
    0xdb, 1, 0x20, // VCOM deslect 0.770
    0x20, 1, 0x20,
    0x33, 0, // VPP 9V
    0xa6, 0, // not inverted
    0xa4, 0, // normal
    0xaf, 0, // on
};

void board_init(void) {
    busio_spi_obj_t *spi = &displays[0].fourwire_bus.inline_bus;
    common_hal_busio_spi_construct(spi, &pin_GPIO26, &pin_GPIO27, NULL);
    common_hal_busio_spi_never_reset(spi);

    displayio_fourwire_obj_t *bus = &displays[0].fourwire_bus;
    bus->base.type = &displayio_fourwire_type;
    common_hal_displayio_fourwire_construct(bus,
        spi,
        &pin_GPIO24, // Command or data
        &pin_GPIO22, // Chip select
        &pin_GPIO23, // Reset
        1000000, // Baudrate
        0, // Polarity
        0); // Phase

    displayio_display_obj_t *display = &displays[0].display;
    display->base.type = &displayio_display_type;
    common_hal_displayio_display_construct(display,
        bus,
        128, // Width
        64, // Height
        2, // column start
        0, // row start
        0, // rotation
        1, // Color depth
        true, // grayscale
        false, // pixels in byte share row. Only used with depth < 8
        1, // bytes per cell. Only valid for depths < 8
        false, // reverse_pixels_in_byte. Only valid for depths < 8
        true, // reverse_pixels_in_word
        0, // Set column command
        0, // Set row command
        0, // Write memory command
        0xd3, // set vertical scroll command
        display_init_sequence,
        sizeof(display_init_sequence),
        NULL,
        0x81,
        1.0f, // brightness
        false, // auto_brightness
        true, // single_byte_bounds
        true, // data as commands
        true, // auto_refresh
        60, // native_frames_per_second
        true, // backlight_on_high
        true); // SH1107_addressing
}

bool board_requests_safe_mode(void) {
    return false;
}

void reset_board(void) {
    // turn off any left over LED
    board_reset_user_neopixels(&pin_GPIO19, 12);
}

void board_deinit(void) {
}
