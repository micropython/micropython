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
#include "hal/include/hal_gpio.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/displayio/FourWire.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"
#include "shared-bindings/digitalio/DigitalInOut.h"

displayio_fourwire_obj_t board_display_obj;
digitalio_digitalinout_obj_t CTR_5V;
digitalio_digitalinout_obj_t CTR_3V3;
digitalio_digitalinout_obj_t USB_HOST_ENABLE;

uint8_t display_init_sequence[] = {
    0x01, 0x80, 0x80, // Software reset then delay 0x80 (128ms)
    0xEF, 0x03, 0x03, 0x80, 0x02,
    0xCF, 0x03, 0x00, 0xC1, 0x30,
    0xED, 0x04, 0x64, 0x03, 0x12, 0x81,
    0xE8, 0x03, 0x85, 0x00, 0x78,
    0xCB, 0x05, 0x39, 0x2C, 0x00, 0x34, 0x02,
    0xF7, 0x01, 0x20,
    0xEA, 0x02, 0x00, 0x00,
    0xc0, 0x01, 0x23, // Power control VRH[5:0]
    0xc1, 0x01, 0x10, // Power control SAP[2:0];BT[3:0]
    0xc5, 0x02, 0x3e, 0x28, // VCM control
    0xc7, 0x01, 0x86, // VCM control2
    0x36, 0x01, 0x38, // Memory Access Control
    0x37, 0x01, 0x00, // Vertical scroll zero
    0x3a, 0x01, 0x55, // COLMOD: Pixel Format Set
    0xb1, 0x02, 0x00, 0x18, // Frame Rate Control (In Normal Mode/Full Colors)
    0xb6, 0x03, 0x08, 0x82, 0x27, // Display Function Control
    0xF2, 0x01, 0x00, // 3Gamma Function Disable
    0x26, 0x01, 0x01, // Gamma curve selected
    0xe0, 0x0f, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00, // Set Gamma
    0xe1, 0x0f, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F, // Set Gamma
    0x11, 0x80, 0x78, // Exit Sleep then delay 0x78 (120ms)
    0x29, 0x80, 0x78, // Display on then delay 0x78 (120ms)
};

void board_init(void) {
    busio_spi_obj_t *spi = &displays[0].fourwire_bus.inline_bus;
    common_hal_busio_spi_construct(spi, &pin_PB20, &pin_PB19, NULL);
    common_hal_busio_spi_never_reset(spi);

    displayio_fourwire_obj_t *bus = &displays[0].fourwire_bus;
    bus->base.type = &displayio_fourwire_type;
    common_hal_displayio_fourwire_construct(bus,
        spi,
        &pin_PC06, // TFT_DC Command or data
        &pin_PB21, // TFT_CS Chip select
        &pin_PC07, // TFT_RST Reset
        60000000, // Baudrate
        0, // Polarity
        0); // Phase

    displayio_display_obj_t *display = &displays[0].display;
    display->base.type = &displayio_display_type;
    common_hal_displayio_display_construct(display,
        bus,
        320, // Width
        240, // Height
        0, // column start
        0, // row start
        180, // rotation
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
        &pin_PC05,  // backlight pin
        NO_BRIGHTNESS_COMMAND,
        1.0f, // brightness (ignored)
        true, // auto_brightness
        false, // single_byte_bounds
        false, // data_as_commands
        true, // auto_refresh
        60, // native_frames_per_second
        true, // backlight_on_high
        false); // SH1107_addressing

    // Enabling the Power of the 40-pin at the back
    CTR_5V.base.type = &digitalio_digitalinout_type;
    CTR_3V3.base.type = &digitalio_digitalinout_type;
    USB_HOST_ENABLE.base.type = &digitalio_digitalinout_type;

    common_hal_digitalio_digitalinout_construct(&CTR_5V, PIN_CTR_5V);
    common_hal_digitalio_digitalinout_construct(&CTR_3V3, PIN_CTR_3V3);
    common_hal_digitalio_digitalinout_construct(&USB_HOST_ENABLE, PIN_USB_HOST_ENABLE);

    common_hal_digitalio_digitalinout_set_value(&CTR_5V, true);
    common_hal_digitalio_digitalinout_set_value(&CTR_3V3, false);
    common_hal_digitalio_digitalinout_set_value(&USB_HOST_ENABLE, false);

    // Never reset
    common_hal_digitalio_digitalinout_never_reset(&CTR_5V);
    common_hal_digitalio_digitalinout_never_reset(&CTR_3V3);
    common_hal_digitalio_digitalinout_never_reset(&USB_HOST_ENABLE);

}

bool board_requests_safe_mode(void) {
    return false;
}

void reset_board(void) {
}

void board_deinit(void) {
}
