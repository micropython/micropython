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
#include "shared-bindings/board/__init__.h"
#include "shared-bindings/displayio/I2CDisplay.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"
#include "shared-bindings/busio/I2C.h"
#include "supervisor/shared/board.h"
#include "shared-bindings/board/__init__.h"

uint8_t display_init_sequence[] = { // SSD1306
    0xAE, 0, // DISPLAY_OFF
    0x20, 1, 0x00, // Set memory addressing to horizontal mode.
    0x81, 1, 0xcf, // set contrast control
    0xA1, 0, // Column 127 is segment 0
    0xA6, 0, // Normal display
    0xc8, 0, // Normal display
    0xA8, 1, 0x1f, // Mux ratio is height-1
    0xd5, 1, 0x80, // Set divide ratio
    0xd9, 1, 0xf1, // Set pre-charge period
    0xda, 1, 0x02, // Set com configuration to 2 if height is 32 and width not 64
    0xdb, 1, 0x40, // Set vcom configuration
    0x8d, 1, 0x14, // Enable charge pump
    0xAF, 0, // DISPLAY_ON
};

static void display_init(void) {
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    displayio_i2cdisplay_obj_t *bus = &displays[0].i2cdisplay_bus;
    bus->base.type = &displayio_i2cdisplay_type;
    common_hal_displayio_i2cdisplay_construct(bus,
        i2c,
        0x3c,
        &pin_GPIO18 // reset
        );

    displayio_display_obj_t *display = &displays[0].display;
    display->base.type = &displayio_display_type;
    common_hal_displayio_display_construct(display,
        bus,
        128, // Width
        32, // Height
        0, // column start
        0, // row start
        0, // rotation
        1, // Color depth
        true, // grayscale
        false, // pixels in byte share row. Only used with depth < 8
        1, // bytes per cell. Only valid for depths < 8
        false, // reverse_pixels_in_byte. Only valid for depths < 8
        true, // reverse_pixels_in_word
        0x21, // Set column command
        0x22, // Set row command
        44, // Write ram command
        display_init_sequence,
        sizeof(display_init_sequence),
        NULL, // no backlight pin
        0x81, // brightness command
        1.0f, // brightness
        true, // single_byte_bounds
        true, // data as commands
        true, // auto_refresh
        60, // native_frames_per_second
        true, // backlight_on_high
        false, // SH1107_addressing
        0); // backlight pwm frequency
}

void board_init(void) {
    // init display
    display_init();
    // Debug UART
    #ifdef DEBUG
    common_hal_never_reset_pin(&pin_GPIO43);
    common_hal_never_reset_pin(&pin_GPIO44);
    #endif /* DEBUG */
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
