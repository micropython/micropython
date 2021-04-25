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
#include "mpconfigboard.h"

#include "shared-bindings/audiopwmio/PWMAudioOut.h"
#include "shared-bindings/board/__init__.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"
#include "shared-bindings/busio/SPI.h"

#include "supervisor/spi_flash_api.h"

audiopwmio_pwmaudioout_obj_t board_buzz_obj;

#define DELAY 0x80

uint8_t display_init_sequence[] = {
    0x01, 0 | DELAY, 150, // SWRESET
    0x11, 0 | DELAY, 255,     // SLPOUT
    0xb1, 3, 0x01, 0x2C, 0x2D,     // _FRMCTR1
    0xb2, 3, 0x01, 0x2C, 0x2D,     //
    0xb3, 6, 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D,
    0xb4, 1, 0x07,     // _INVCTR line inversion
    0xc0, 3, 0xa2, 0x02, 0x84,     // _PWCTR1 GVDD = 4.7V, 1.0uA
    0xc1, 1, 0xc5,     // _PWCTR2 VGH=14.7V, VGL=-7.35V
    0xc2, 2, 0x0a, 0x00,     // _PWCTR3 Opamp current small, Boost frequency
    0xc3, 2, 0x8a, 0x2a,
    0xc4, 2, 0x8a, 0xee,
    0xc5, 1, 0x0e,     // _VMCTR1 VCOMH = 4V, VOML = -1.1V
    0x20, 0,     // _INVOFF //MISMATCh 0x2a vs 0x20
    0x36, 1, 0x18,     // _MADCTL bottom to top refresh
    // 1 clk cycle nonoverlap, 2 cycle gate rise, 3 sycle osc equalie,
    // fix on VTL
    0x3a, 1, 0x05,     // COLMOD - 16bit color
    0xe0, 0x10, 0x02, 0x1c, 0x07, 0x12,
    0x37, 0x32, 0x29, 0x2d,
    0x29, 0x25, 0x2B, 0x39,
    0x00, 0x01, 0x03, 0x10,                             // _GMCTRP1 Gamma
    0xe1, 0x10, 0x03, 0x1d, 0x07, 0x06,
    0x2E, 0x2C, 0x29, 0x2D,
    0x2E, 0x2E, 0x37, 0x3F,
    0x00, 0x00, 0x02, 0x10,                             // _GMCTRN1
    0x13, 0 | DELAY, 10,     // _NORON
    0x29, 0 | DELAY, 100,     // _DISPON
};

void board_init(void) {
    displayio_fourwire_obj_t *bus = &displays[0].fourwire_bus;
    bus->base.type = &displayio_fourwire_type;
    busio_spi_obj_t *internal_spi = &supervisor_flash_spi_bus;
    common_hal_displayio_fourwire_construct(bus,
        internal_spi,
        &pin_PA08, // Command or data
        &pin_PB12, // Chip select
        &pin_PB10, // Reset
        24000000, // Baudrate
        0, // Polarity
        0); // Phase

    displayio_display_obj_t *display = &displays[0].display;
    display->base.type = &displayio_display_type;
    common_hal_displayio_display_construct(display,
        bus,
        160, // Width
        128, // Height
        0, // column start
        0, // row start
        90, // rotation
        16, // Color depth
        false, // Grayscale
        false, // Pixels in a byte share a row. Only used for depth < 8
        1, // bytes per cell. Only valid for depths < 8
        false, // reverse_pixels_in_byte. Only valid for depths < 8
        true, // reverse_pixels_in_word
        MIPI_COMMAND_SET_COLUMN_ADDRESS, // Set column command
        MIPI_COMMAND_SET_PAGE_ADDRESS, // Set row command
        MIPI_COMMAND_WRITE_MEMORY_START, // Write memory command
        0x37, // set vertical scroll command
        display_init_sequence,
        sizeof(display_init_sequence),
        &pin_PB03,
        NO_BRIGHTNESS_COMMAND,
        1.0f, // brightness (ignored)
        false, // auto_brightness
        false, // single_byte_bounds
        false, // data_as_commands
        true, // auto_refresh
        60, // native_frames_per_second
        true, // backlight_on_high
        false); // SH1107_addressing

    board_buzz_obj.base.type = &audiopwmio_pwmaudioout_type;
    common_hal_audiopwmio_pwmaudioout_construct(&board_buzz_obj,
        &pin_PB08, NULL, 0x8000);
    never_reset_pin_number(pin_PB08.port, pin_PB08.number);
}

bool board_requests_safe_mode(void) {
    return false;
}

void reset_board(void) {

}
