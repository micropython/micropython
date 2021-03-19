/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Joey Castillo
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
#include "hal/include/hal_gpio.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/displayio/FourWire.h"
#include "shared-bindings/time/__init__.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"

displayio_fourwire_obj_t board_display_obj;

#define DELAY 0x80
#define HEIGHT 400
#define WIDTH 300

uint8_t start_sequence[] = {
    0x01, 0x04, 0x03, 0x00, 0x2b, 0x2b, // power setting
    0x06, 0x03, 0x17, 0x17, 0x17, // booster soft start
    0x04, 0x80, 0xc8, // power on and wait 200 ms
    0x00, 0x01, 0x0f, // panel setting
    0x61, 0x04, (HEIGHT >> 8) & 0xFF, HEIGHT & 0xFF, (WIDTH >> 8) & 0xFF, WIDTH & 0xFF // Resolution
};

uint8_t stop_sequence[] = {
    0x50, 0x01, 0xf7, // CDI setting
    0x02, 0x80, 0xf0  // Power off
};

void board_init(void) {
    busio_spi_obj_t *spi = &displays[0].fourwire_bus.inline_bus;
    common_hal_busio_spi_construct(spi, &pin_PB13, &pin_PB15, NULL);
    common_hal_busio_spi_never_reset(spi);

    displayio_fourwire_obj_t *bus = &displays[0].fourwire_bus;
    bus->base.type = &displayio_fourwire_type;
    common_hal_displayio_fourwire_construct(bus,
        spi,
        &pin_PB05, // EPD_DC Command or data
        &pin_PB07, // EPD_CS Chip select
        &pin_PA00, // EPD_RST Reset
        1000000, // Baudrate
        0, // Polarity
        0); // Phase

    displayio_epaperdisplay_obj_t *display = &displays[0].epaper_display;
    display->base.type = &displayio_epaperdisplay_type;
    common_hal_displayio_epaperdisplay_construct(display,
        bus,
        start_sequence,
        sizeof(start_sequence),
        stop_sequence,
        sizeof(stop_sequence),
        300, // width
        400, // height
        300, // RAM width
        400, // RAM height
        0, // colstart
        0, // rowstart
        270, // rotation
        NO_COMMAND, // set_column_window_command
        NO_COMMAND, // set_row_window_command
        NO_COMMAND, // set_current_column_command
        NO_COMMAND, // set_current_row_command
        0x13, // write_black_ram_command
        false, // black_bits_inverted
        NO_COMMAND, // write_color_ram_command (can add this for grayscale eventually)
        false, // color_bits_inverted
        0x000000, // highlight_color
        0x12, // refresh_display_command
        40, // refresh_time
        &pin_PA01, // busy_pin
        false, // busy_state
        5, // seconds_per_frame
        false, // chip_select (don't always toggle chip select)
        false); // grayscale
}

bool board_requests_safe_mode(void) {
    return false;
}

void reset_board(void) {
}
