/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 CDarius
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
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"
#include "shared-bindings/board/__init__.h"

displayio_fourwire_obj_t board_display_obj;


// display init sequence according to M5Gfx
uint8_t display_init_sequence[] = {
    0x01,0x80,0x80,             // Software reset then delay 0x80 (128ms)
    0xC8,0x03,0xFF,0x93,0x42,   // Turn on the external command
    0xC0,0x02,0x12, 0x12,       // Power Control 1
    0xC1,0x01,0x03,             // Power Control 2
    0xC5,0x01,0xF2,             // VCOM Control 1
    0xB0,0x01,0xE0,             // RGB Interface SYNC Mode
    0xF6,0x03,0x01, 0x00, 0x00, // Interface control
    0XE0,0x0F,0x00,0x0C,0x11,0x04,0x11,0x08,0x37,0x89,0x4C,0x06,0x0C,0x0A,0x2E,0x34,0x0F,   // Positive Gamma Correction
    0xE1,0x0F,0x00,0x0B,0x11,0x05,0x13,0x09,0x33,0x67,0x48,0x07,0x0E,0x0B,0x2E,0x33,0x0F,   // Negative Gamma Correction
    0xB6,0x04,0x08,0x82,0x1D,0x04,  // Display Function Control
    0x3A,0x01,0x55,             // COLMOD: Pixel Format Set 16 bit
    0x21,0x00,                  // Display inversion ON
    0x36,0x01,0x08,             // Memory Access Control: RGB order
    0x11,0x80,0x78,             // Exit Sleep then delay 0x78 (120ms)
    0x29,0x80,0x78,             // Display on then delay 0x78 (120ms)
};

void board_init(void) {
    busio_spi_obj_t *spi = common_hal_board_create_spi(0);
    displayio_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    bus->base.type = &displayio_fourwire_type;

    common_hal_displayio_fourwire_construct(
        bus,
        spi,
        &pin_GPIO27,    // DC
        &pin_GPIO14,    // CS
        &pin_GPIO33,    // RST
        40000000,       // baudrate
        0,              // polarity
        0               // phase
        );

    displayio_display_obj_t *display = &allocate_display()->display;
    display->base.type = &displayio_display_type;

    common_hal_displayio_display_construct(
        display,
        bus,
        320,            // width (after rotation)
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
        &pin_GPIO32,    // backlight pin
        NO_BRIGHTNESS_COMMAND,
        1.0f,           // brightness
        false,          // single_byte_bounds
        false,          // data_as_commands
        true,           // auto_refresh
        61,             // native_frames_per_second
        true,           // backlight_on_high
        false,          // SH1107_addressing
        50000           // backlight pwm frequency
        );

}

bool espressif_board_reset_pin_number(gpio_num_t pin_number) {
    // Set speaker gpio to ground to prevent noise from the speaker
    if (pin_number == 25) {
        gpio_set_direction(pin_number, GPIO_MODE_DEF_OUTPUT);
        gpio_set_level(pin_number, false);
        return true;
    }
    return false;
}
