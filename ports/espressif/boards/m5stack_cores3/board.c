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
#include "shared-bindings/busio/I2C.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"
#include "shared-bindings/board/__init__.h"

fourwire_fourwire_obj_t board_display_obj;

#define DELAY 0x80
#define AXP2101_I2C_ADDRESS 0x34
#define AW9523B_I2C_ADDRESS 0x58

uint8_t display_init_sequence[] = {
    0x01, DELAY, 0x80,            // Software reset then delay 0x80 (128ms)
    0xC8, 0x03, 0xFF, 0x93, 0x42,   // Turn on the external command
    0xC0, 0x02, 0x12, 0x12,       // Power Control 1
    0xC1, 0x01, 0x03,             // Power Control 2
    0xC5, 0x01, 0xF2,             // VCOM Control 1
    0xB0, 0x01, 0xE0,             // RGB Interface SYNC Mode
    0xF6, 0x03, 0x01, 0x00, 0x00, // Interface control
    0XE0, 0x0F, 0x00, 0x0C, 0x11, 0x04, 0x11, 0x08, 0x37, 0x89, 0x4C, 0x06, 0x0C, 0x0A, 0x2E, 0x34, 0x0F,   // Positive Gamma Correction
    0xE1, 0x0F, 0x00, 0x0B, 0x11, 0x05, 0x13, 0x09, 0x33, 0x67, 0x48, 0x07, 0x0E, 0x0B, 0x2E, 0x33, 0x0F,   // Negative Gamma Correction
    0xB6, 0x04, 0x08, 0x82, 0x1D, 0x04,  // Display Function Control
    0x3A, 0x01, 0x55,             // COLMOD: Pixel Format Set 16 bit
    0x21, 0x00,                  // Display inversion ON
    0x36, 0x01, 0x08,             // Memory Access Control: RGB order
    0x11, DELAY, 0x78,            // Exit Sleep then delay 0x78 (120ms)
    0x29, DELAY, 0x78,            // Display on then delay 0x78 (120ms)
};

static bool display_init(void) {
    busio_spi_obj_t *spi = common_hal_board_create_spi(0);
    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    bus->base.type = &fourwire_fourwire_type;

    common_hal_fourwire_fourwire_construct(
        bus,
        spi,
        &pin_GPIO35,    // DC
        &pin_GPIO3,     // CS
        NULL,           // RST
        40000000,       // baudrate
        0,              // polarity
        0               // phase
        );
    busdisplay_busdisplay_obj_t *display = &allocate_display()->display;
    display->base.type = &busdisplay_busdisplay_type;

    common_hal_busdisplay_busdisplay_construct(
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
        NULL,           // backlight pin
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

    return true;
}

static bool axp2101_init(busio_i2c_obj_t *i2c) {
    int rc;
    uint8_t write_buf[2];

    // 0x90 = 0b1011_1001  // LDOS ON/OFF control 0
    write_buf[0] = 0x90;
    write_buf[1] = 0b10111001;
    rc = common_hal_busio_i2c_write(i2c, AXP2101_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // 0x92, 0x0D // ALDO1 set to 1.8v for AW88298
    write_buf[0] = 0x92;
    write_buf[1] = 0x0D;
    rc = common_hal_busio_i2c_write(i2c, AXP2101_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // 0x93, 0x1C // ALDO2 set to 3.3v for ES7210
    write_buf[0] = 0x93;
    write_buf[1] = 0x1C;
    rc = common_hal_busio_i2c_write(i2c, AXP2101_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // 0x94, 0x1C // ALDO3 set to 3.3v for camera
    write_buf[0] = 0x94;
    write_buf[1] = 0x1C;
    rc = common_hal_busio_i2c_write(i2c, AXP2101_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // 0x95, 0x1C // ALDO3 set to 3.3v for TF card slot
    write_buf[0] = 0x95;
    write_buf[1] = 0x1C;
    rc = common_hal_busio_i2c_write(i2c, AXP2101_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // 0x99, 0x18 // DLDO1 set to 2.9v for TFT backlight
    write_buf[0] = 0x99;
    write_buf[1] = 0x18;
    rc = common_hal_busio_i2c_write(i2c, AXP2101_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // 0x27, 0x00 // PowerKey Hold=1sec / PowerOff=4sec
    write_buf[0] = 0x27;
    write_buf[1] = 0x00;
    rc = common_hal_busio_i2c_write(i2c, AXP2101_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // 0x69, 0x11 // CHGLED setting
    write_buf[0] = 0x69;
    write_buf[1] = 0x11;
    rc = common_hal_busio_i2c_write(i2c, AXP2101_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // 0x10, 0x30 // PMU common config
    write_buf[0] = 0x10;
    write_buf[1] = 0x30;
    rc = common_hal_busio_i2c_write(i2c, AXP2101_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    return true;
}

static bool aw9523b_init(busio_i2c_obj_t *i2c) {
    int rc;
    uint8_t write_buf[2];

    // 0x02 = 0b0000_0111 // AW_RST, BUD_OUT_EN, TOUCH_RST
    write_buf[0] = 0x02;
    write_buf[1] = 0b00000111;
    rc = common_hal_busio_i2c_write(i2c, AW9523B_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // 0x03 = 0b1000_0011 // BOOST_EN, CAM_RST, LCD_RST
    write_buf[0] = 0x03;
    write_buf[1] = 0b10000011;
    rc = common_hal_busio_i2c_write(i2c, AW9523B_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // 0x04 = 0b0001_1000 // Set TF_SW, ES_INT as input
    write_buf[0] = 0x04;
    write_buf[1] = 0b00011000;
    rc = common_hal_busio_i2c_write(i2c, AW9523B_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // 0x05 = 0b0000_1100 // Set AW_INT, TOUCH_INT as input
    write_buf[0] = 0x05;
    write_buf[1] = 0b00001100;
    rc = common_hal_busio_i2c_write(i2c, AW9523B_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // 0x11 = 0b0001_0000 // Set P0 outputs in push pull mode
    write_buf[0] = 0x11;
    write_buf[1] = 0b00010000;
    rc = common_hal_busio_i2c_write(i2c, AW9523B_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    return true;
}

void board_init(void) {
    busio_i2c_obj_t *internal_i2c = common_hal_board_create_i2c(0);

    if (!axp2101_init(internal_i2c)) {
        mp_printf(&mp_plat_print, "could not initialize AXP2101");
        return;
    }

    if (!aw9523b_init(internal_i2c)) {
        mp_printf(&mp_plat_print, "could not initialize AW9523B");
        return;
    }

    if (!display_init()) {
        mp_printf(&mp_plat_print, "could not initialize the display");
        return;
    }
}
