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
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"
#include "shared-bindings/board/__init__.h"

#include "esp_log.h"
#include "esp_err.h"
#include "driver/i2c.h"

displayio_fourwire_obj_t board_display_obj;

#define DELAY 0x80

uint8_t display_init_sequence[] = {
    0x01, 0 | DELAY, 120, // SWRESET
    0x11, 0 | DELAY, 5, // SLPOUT
    0x36, 1, 0b10100000,  // _MADCTL for rotation 0
    0x3a, 1, 0x55, // COLMOD - 16bit color
    0x21, 0,                 // _INVON
    0x13, 0,                 // _NORON
    0x29, 0 | DELAY, 5,                // _DISPON
};

#define I2C_MASTER_SCL_IO           34
#define I2C_MASTER_SDA_IO           33
#define I2C_MASTER_NUM              0
#define I2C_MASTER_FREQ_HZ          400000
#define I2C_MASTER_TX_BUF_DISABLE   0
#define I2C_MASTER_RX_BUF_DISABLE   0
#define I2C_MASTER_TIMEOUT_MS       1000
#define I2C_WAIT                    40      // Timing (in microseconds) for I2C

#define AW9523_ADDR (0x5B)
#define AW9523_REG_SOFTRESET (0x7f)
#define AW9523_REG_OUTPUT0 (0x02)
#define AW9523_REG_CONFIG0 (0x04)
#define AW9523_DEFAULT_OUTPUT (0)
#define AW9523_DEFAULT_CONFIG (0x2)


static void io_expander_backlight_init(void) {
    int i2c_num = I2C_MASTER_NUM;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    i2c_param_config(i2c_num, &conf);
    i2c_driver_install(i2c_num, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, AW9523_ADDR << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, AW9523_REG_SOFTRESET, true);
    i2c_master_write_byte(cmd, 0, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, AW9523_ADDR << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, AW9523_REG_CONFIG0, true);
    i2c_master_write_byte(cmd, AW9523_DEFAULT_CONFIG >> 8, true);
    i2c_master_write_byte(cmd, AW9523_DEFAULT_CONFIG & 0xff, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, AW9523_ADDR << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, AW9523_REG_OUTPUT0, true);
    i2c_master_write_byte(cmd, AW9523_DEFAULT_OUTPUT >> 8, true);
    i2c_master_write_byte(cmd, AW9523_DEFAULT_OUTPUT & 0xff, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    i2c_driver_delete(i2c_num);
}

void board_init(void) {
    io_expander_backlight_init();
    busio_spi_obj_t *spi = common_hal_board_create_spi(0);
    displayio_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    bus->base.type = &displayio_fourwire_type;
    common_hal_displayio_fourwire_construct(bus,
        spi,
        &pin_GPIO40, // TFT_DC Command or data
        &pin_GPIO39, // TFT_CS Chip select
        &pin_GPIO38, // TFT_RESET Reset
        40000000, // Baudrate
        0, // Polarity
        0); // Phase

    displayio_display_obj_t *display = &allocate_display()->display;
    display->base.type = &displayio_display_type;
    common_hal_displayio_display_construct(
        display,
        bus,
        240, // Width (after rotation)
        240, // Height (after rotation)
        80, // column start
        0, // row start
        0, // rotation
        16, // Color depth
        false, // Grayscale
        false, // Pixels in a byte share a row. Only used for depth < 8
        1, // bytes per cell. Only valid for depths < 8
        false, // reverse_pixels_in_byte. Only valid for depths < 8
        true, // reverse_pixels_in_word
        MIPI_COMMAND_SET_COLUMN_ADDRESS, // Set column command
        MIPI_COMMAND_SET_PAGE_ADDRESS, // Set row command
        MIPI_COMMAND_WRITE_MEMORY_START, // Write memory command
        display_init_sequence,
        sizeof(display_init_sequence),
        NULL,  // backlight pin
        NO_BRIGHTNESS_COMMAND,
        1.0f, // brightness
        false, // single_byte_bounds
        false, // data_as_commands
        true, // auto_refresh
        60, // native_frames_per_second
        true, // backlight_on_high
        false, // not SH1107
        50000); // backlight pwm frequency
}

void board_deinit(void) {
    common_hal_displayio_release_displays();
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
