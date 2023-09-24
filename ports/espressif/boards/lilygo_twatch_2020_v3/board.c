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
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"

#define DELAY 0x80

// display init sequence for ST7789 (works in python)
uint8_t display_init_sequence[] = {
    // sw reset
    0x01, 0 | DELAY, 0x96,
    // sleep out
    0x11, 0 | DELAY, 0xFF,
    0x3A, 1 | DELAY,  0x55, 10,
    0x36, 1, 0x08,
    0x21, 0 | DELAY, 10,
    0x13, 0 | DELAY, 10,
    0x36, 1, 0xC0,
    // display on
    0x29, 0 | DELAY, 255,
};

static void display_init(void) {
    displayio_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    busio_spi_obj_t *spi = &bus->inline_bus;

    common_hal_busio_spi_construct(
        spi,
        &pin_GPIO18,    // CLK
        &pin_GPIO19,    // MOSI
        NULL,           // MISO not connected
        false           // Not half-duplex
        );

    common_hal_busio_spi_never_reset(spi);

    bus->base.type = &displayio_fourwire_type;

    common_hal_displayio_fourwire_construct(
        bus,
        spi,
        &pin_GPIO27,    // DC
        &pin_GPIO5,     // CS
        NULL,           // RST
        24000000,       // baudrate
        0,              // polarity
        0               // phase
        );

    displayio_display_obj_t *display = &allocate_display()->display;
    display->base.type = &displayio_display_type;

    common_hal_displayio_display_construct(
        display,
        bus,
        240,            // width (after rotation)
        240,            // height (after rotation)
        0,              // column start
        80,             // row start
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
        &pin_GPIO15,    // backlight pin
        NO_BRIGHTNESS_COMMAND,
        1.0f,           // brightness
        false,          // single_byte_bounds
        false,          // data_as_commands
        true,           // auto_refresh
        60,             // native_frames_per_second
        true,           // backlight_on_high
        false,          // SH1107_addressing
        5000           // backlight pwm frequency
        );
}

/*
#define APX202_ADDRESS (0x35)
#define AXP202_LDO2 (2)
#define AXP202_ON  (1)
#define AXP202_OFF (0)
#define AXP202_DCDC3 (1)
#define AXP202_IC_TYPE (0x03)
#define AXP202_LDO234_DC23_CTL (0x12)
#define AXP_PASS (1)
#define AXP_FAIL (-1)
#define AXP202_CHIP_ID (0x41)
#define AXP192_CHIP_ID (0x03)
#define FORCED_OPEN_DCDC3(x) (x |= (AXP202_ON << AXP202_DCDC3))

static void backlight_init(void) {
    // Turn LDO2 on for display
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t data = 0;
    uint8_t outputReg = 0;
    uint8_t read = 0;
    // lock
    // _readByte(AXP202_LDO234_DC23_CTL, 1, &outputReg)
    // time.sleep(0.001)
    // _readByte(AXP202_LDO234_DC23_CTL, 1, &data)
    // time.sleep(0.001)
    // data |= 1 << AXP202_LDO2
    // FORCED_OPEN_DCDC3(data)
    // _writeByte(AXP202_LDO234_DC23_CTL, 1, &data)
    // time.sleep(0.001)
    // unlock
}
*/

void board_init(void) {
    // Display
    display_init();
    // backlight_init();
}

bool espressif_board_reset_pin_number(gpio_num_t pin_number) {
    if (pin_number == MOTOR_PIN) {
        // no motor
        gpio_set_direction(pin_number, GPIO_MODE_DEF_OUTPUT);
        gpio_set_level(pin_number, false);
        return true;
    }
    return false;
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
