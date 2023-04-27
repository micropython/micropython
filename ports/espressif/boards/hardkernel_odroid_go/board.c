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
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/displayio/FourWire.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"

#include "common-hal/microcontroller/Pin.h"

#define DELAY 0x80

// ILI9341 init sequence from:
// https://github.com/hardkernel/ODROID-GO-MicroPython/blob/loboris/odroid_go/utils/lcd/lcd.py#L55
uint8_t display_init_sequence[] = {
    0x0f, 3, 0x03, 0x80, 0x02, // RDDSDR
    0xcf, 3, 0x00, 0xcf, 0x30, // PWCRTLB
    0xed, 4, 0x64, 0x03, 0x12, 0x81, // PWRONCTRL
    0xe8, 3, 0x85, 0x00, 0x78, // DTCTRLA
    0xcb, 5, 0x39, 0x2c, 0x00, 0x34, 0x02, // PWCTRLA
    0xf7, 1, 0x20, // PRCTRL
    0xea, 2, 0x00, 0x00, // DTCTRLB
    0xc0, 1, 0x1b, // PWCTRL1
    0xc1, 1, 0x12, // PWCTRL2
    0xc5, 2, 0x3e, 0x3c, // VMCTRL1
    0xc7, 1, 0x91, // VMCTRL2
    0x36, 1, 0xa8, // MADCTL
    0x3a, 1, 0x55, // PIXSET
    0xb1, 2, 0x00, 0x1b, // FRMCTR1
    0xb6, 3, 0x0a, 0xa2, 0x27, // DISCTRL
    0xf6, 2, 0x01, 0x30, // INTFACE
    0xf2, 1, 0x00, // ENA3G
    0x26, 1, 0x01, // GAMSET
    0xe0, 15, 0x0f, 0x31, 0x2b, 0x0c, 0x0e, 0x08, 0x4e, 0xf1, 0x37, 0x07, 0x10, 0x03, 0x0e, 0x09, 0x00, // PGAMCTRL
    0xe1, 15, 0x00, 0x0e, 0x14, 0x03, 0x11, 0x07, 0x31, 0xc1, 0x48, 0x08, 0x0f, 0x0c, 0x31, 0x36, 0x0f, // NGAMCTRL
    0x11, 0 | DELAY, 10, // SLPOUT
    0x29, 0 | DELAY, 100, // DISPON
};

void board_init(void) {
    displayio_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    busio_spi_obj_t *spi = &bus->inline_bus;
    common_hal_busio_spi_construct(spi, &pin_GPIO18, &pin_GPIO23, NULL, false);
    common_hal_busio_spi_never_reset(spi);

    bus->base.type = &displayio_fourwire_type;
    common_hal_displayio_fourwire_construct(bus,
        spi,
        &pin_GPIO21, // TFT_DC Command or data
        &pin_GPIO5, // TFT_CS Chip select
        NULL, // TFT_RST Reset
        40000000, // Baudrate
        0, // Polarity
        0); // Phase

    displayio_display_obj_t *display = &allocate_display()->display;
    display->base.type = &displayio_display_type;
    common_hal_displayio_display_construct(display,
        bus,
        320, // Width (after rotation)
        240, // Height (after rotation)
        0, // column start
        0, // row start
        0, // rotation
        16, // Color depth
        false, // grayscale
        false, // pixels in byte share row. only used for depth < 8
        1, // bytes per cell. Only valid for depths < 8
        false, // reverse_pixels_in_byte. Only valid for depths < 8
        true, // reverse_pixels_in_word
        MIPI_COMMAND_SET_COLUMN_ADDRESS, // Set column command
        MIPI_COMMAND_SET_PAGE_ADDRESS, // Set row command
        MIPI_COMMAND_WRITE_MEMORY_START, // Write memory command
        display_init_sequence,
        sizeof(display_init_sequence),
        &pin_GPIO14,  // backlight pin
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

bool espressif_board_reset_pin_number(gpio_num_t pin_number) {
    // Pull LED down on reset rather than the default up
    if (pin_number == 2) {
        gpio_config_t cfg = {
            .pin_bit_mask = BIT64(pin_number),
            .mode = GPIO_MODE_DISABLE,
            .pull_up_en = false,
            .pull_down_en = true,
            .intr_type = GPIO_INTR_DISABLE,
        };
        gpio_config(&cfg);
        return true;
    }
    return false;
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
