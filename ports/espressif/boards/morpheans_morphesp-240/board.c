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

// From Arduino-ST7789 library https://github.com/ananevilya/Arduino-ST7789-Library/blob/master/Arduino_ST7789.cpp
#define ST7789_TFTWIDTH         240
#define ST7789_TFTHEIGHT        240

#define ST7789_240x240_XSTART 0
#define ST7789_240x240_YSTART 0

#define ST7789_NOP      0x00
#define ST7789_SWRESET  0x01
#define ST7789_RDDID    0x04
#define ST7789_RDDST    0x09

#define ST7789_SLPIN    0x10
#define ST7789_SLPOUT   0x11
#define ST7789_PTLON    0x12
#define ST7789_NORON    0x13

#define ST7789_INVOFF   0x20
#define ST7789_INVON    0x21
#define ST7789_DISPOFF  0x28
#define ST7789_DISPON   0x29
#define ST7789_CASET    0x2A
#define ST7789_RASET    0x2B
#define ST7789_RAMWR    0x2C
#define ST7789_RAMRD    0x2E

#define ST7789_PTLAR    0x30
#define ST7789_COLMOD   0x3A
#define ST7789_MADCTL   0x36
#define ST7789_VSCSAD   0x37
#define ST7789_PORCTRL  0xB2
#define ST7789_GCTRL    0xB7
#define ST7789_VCOMS    0xBB
#define ST7789_LCMCTRL  0xC0
#define ST7789_IDSET    0xC1
#define ST7789_VDVVRHEN 0xC2
#define ST7789_VRHS     0xC3
#define ST7789_VDVS     0xC4
#define ST7789_VCMOFSET 0xC5
#define ST7789_FRCTRL2  0xC6
#define ST7789_CABCCTRL 0xC7
#define ST7789_REGSEL1  0xC8
#define ST7789_REGSEL2  0xCA
#define ST7789_PWMFRSEL 0xCC
#define ST7789_PWCTRL1  0xD0
#define ST7789_VAPVANEN 0xD2
#define ST7789_PVGAMCTRL 0xE0
#define ST7789_NVGAMCTRL 0xE1

#define ST7789_MADCTL_MY  0x80
#define ST7789_MADCTL_MX  0x40
#define ST7789_MADCTL_MV  0x20
#define ST7789_MADCTL_ML  0x10
#define ST7789_MADCTL_RGB 0x00

#define ST7789_RDID1    0xDA
#define ST7789_RDID2    0xDB
#define ST7789_RDID3    0xDC
#define ST7789_RDID4    0xDD

#define DISPLAY_MADCTL      (ST7789_MADCTL_RGB)
#define DISPLAY_VSCSAD      0

// The init_sequence is bitpacked to minimize the ram impact. Every command begins with a
// command byte followed by a byte to determine the parameter count and delay. When the top bit
// of the second byte is 1 (0x80), a delay will occur after the command parameters are sent.
// The remaining 7 bits are the parameter count excluding any delay byte. The bytes following
// are the parameters. When the delay bit is set, a single byte after the parameters specifies
// the delay duration in milliseconds. The value 0xff will lead to an extra long 500 ms delay
// instead of 255 ms.uint8_t display_init_sequence[] = {
// display init sequence according to LilyGO example app

uint8_t display_init_sequence[] = {
    // From Lilygo example
    // sw reset
    0x01, 0 | DELAY, 150,
    // sleep out
    0x11, 0 | DELAY, 120,
    // normal display mode on
    0x13, 0,
    // display and color format settings
    0x36, 1, DISPLAY_MADCTL,
    0xB6, 2, 0x0A, 0x82,
    0x3A, 1 | DELAY,  0x55, 10,
    // ST7789V frame rate setting
    0xB2, 5, 0x0C, 0x0C, 0x00, 0x33, 0x33,
    // voltages: VGH / VGL
    0xB7, 1, 0x35,
    // ST7789V power setting
    0xBB, 1, 0x28,
    0xC0, 1, 0x0C,
    0xC2, 2, 0x01, 0xFF,
    0xC3, 1, 0x10,
    0xC4, 1, 0x20,
    0xC6, 1, 0x0F,
    0xD0, 2, 0xA4, 0xA1,
    // ST7789V gamma setting
    0xE0, 14, 0xD0, 0x00, 0x02, 0x07, 0x0A, 0x28, 0x32, 0x44, 0x42, 0x06, 0x0E, 0x12, 0x14, 0x17,
    0xE1, 14, 0xD0, 0x00, 0x02, 0x07, 0x0A, 0x28, 0x31, 0x54, 0x47, 0x0E, 0x1C, 0x17, 0x1B, 0x1E,
    0x21, 0,
    // display on
    0x21, 0 | DELAY, 10,        // _INVON
    0x29, 0 | DELAY, 120
};


void board_init(void) {
    // USB
    common_hal_never_reset_pin(&pin_GPIO19);
    common_hal_never_reset_pin(&pin_GPIO20);

    // Debug UART
    #ifdef DEBUG
    common_hal_never_reset_pin(&pin_GPIO6);
    common_hal_never_reset_pin(&pin_GPIO7);
    #endif /* DEBUG */

    // Display

    busio_spi_obj_t *spi = &displays[0].fourwire_bus.inline_bus;

    common_hal_busio_spi_construct(
        spi,
        &pin_GPIO12,    // CLK
        &pin_GPIO11,    // MOSI
        NULL            // MISO not connected
        );

    common_hal_busio_spi_never_reset(spi);

    displayio_fourwire_obj_t *bus = &displays[0].fourwire_bus;
    bus->base.type = &displayio_fourwire_type;

    common_hal_displayio_fourwire_construct(
        bus,
        spi,
        &pin_GPIO14,    // DC
        &pin_GPIO10,    // CS
        &pin_GPIO9,    // RST
        40000000,       // baudrate
        0,              // polarity
        0               // phase
        );

    // workaround as board_init() is called before reset_port() in main.c
    pwmout_reset();

    displayio_display_obj_t *display = &displays[0].display;
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
        NULL,           // There is no backlight pin, defined for now.
        NO_BRIGHTNESS_COMMAND,
        1.0f,           // brightness (ignored)
        true,           // auto_brightness
        false,          // single_byte_bounds
        false,          // data_as_commands
        true,           // auto_refresh
        60,             // native_frames_per_second
        false,          // backlight_on_high
        false           // SH1107_addressing
        );
}

bool board_requests_safe_mode(void) {
    return false;
}

void reset_board(void) {

}

void board_deinit(void) {
    common_hal_displayio_release_displays();
}
