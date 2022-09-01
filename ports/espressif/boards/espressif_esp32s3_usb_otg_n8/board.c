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
#include "shared-bindings/displayio/FourWire.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"

#define DELAY 0x80

// Init sequence from:
// https://github.com/espressif/esp-dev-kits/blob/26ad8c9070b717da9fa06ce480099b7826761c2a/esp32-s3-usb-otg/components/display_screen/controller_driver/st7789/st7789.c#L75

// display init sequence according to LilyGO example app
uint8_t display_init_sequence[] = {
    // sw reset
    0x01, 0 | DELAY, 150,
    // normal display mode on
    0x13, 0,
    // display and color format settings
    // 0x36, 1, 0x68,
    // 0xB6, 2, 0x0A, 0x82,
    0x3A, 1 | DELAY,  0x05, 10,
    // ST7789V frame rate setting
    0xB2, 5, 0x0C, 0x0C, 0x00, 0x33, 0x33,
    // voltages: VGH / VGL
    0xB7, 1, 0x35,
    // ST7789V power setting
    0xBB, 1, 0x19,
    0xC0, 1, 0x2C,
    0xC2, 1, 0x01,
    0xC3, 1, 0x12,
    0xC4, 1, 0x20,
    0xC6, 1, 0x0F,
    0xD0, 2, 0xA4, 0xA1,
    // ST7789V gamma setting
    0xE0, 14, 0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F, 0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x1F, 0x23,
    0xE1, 14, 0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F, 0x44, 0x51, 0x2F, 0x1F, 0x1F, 0x20, 0x23,
    0x21, 0,
    // sleep out
    0x11, 0 | DELAY, 255,
    // display on
    0x29, 0 | DELAY, 255,
};

void board_init(void) {
    busio_spi_obj_t *spi = &displays[0].fourwire_bus.inline_bus;
    common_hal_busio_spi_construct(spi, &pin_GPIO6, &pin_GPIO7, NULL, false);
    common_hal_busio_spi_never_reset(spi);

    displayio_fourwire_obj_t *bus = &displays[0].fourwire_bus;
    bus->base.type = &displayio_fourwire_type;
    common_hal_displayio_fourwire_construct(bus,
        spi,
        &pin_GPIO4, // TFT_DC Command or data
        &pin_GPIO5, // TFT_CS Chip select
        &pin_GPIO8, // TFT_RST Reset
        60000000, // Baudrate
        0, // Polarity
        0); // Phase

    displayio_display_obj_t *display = &displays[0].display;
    display->base.type = &displayio_display_type;
    common_hal_displayio_display_construct(display,
        bus,
        240, // Width
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
        &pin_GPIO9,  // backlight pin
        NO_BRIGHTNESS_COMMAND,
        1.0f, // brightness
        false, // single_byte_bounds
        false, // data_as_commands
        true, // auto_refresh
        60, // native_frames_per_second
        true, // backlight_on_high
        false, // SH1107_addressing
        50000); // backlight pwm frequency

    #if CIRCUITPY_DEBUG
    common_hal_never_reset_pin(DEFAULT_UART_BUS_TX);
    #endif
}

bool espressif_board_reset_pin_number(gpio_num_t pin_number) {
    // Override the USB_SEL, LED_GREEN, LED_YELLOW and BOOST_EN pins.
    if (pin_number == 18 || pin_number == 15 || pin_number == 16 || pin_number == 13) {
        gpio_reset_pin(pin_number);
        gpio_pullup_dis(pin_number);
        gpio_pulldown_en(pin_number);
        return true;
    }
    return false;
}

bool board_requests_safe_mode(void) {
    return false;
}

void reset_board(void) {

}

void board_deinit(void) {
}
