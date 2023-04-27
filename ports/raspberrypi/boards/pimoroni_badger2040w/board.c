/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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
#include "shared-bindings/board/__init__.h"
#include "supervisor/shared/board.h"
#include "badger-shared.h"

digitalio_digitalinout_obj_t enable_pin_obj;

#define DELAY 0x80

enum reg {
    PSR      = 0x00,
    PWR      = 0x01,
    POF      = 0x02,
    PFS      = 0x03,
    PON      = 0x04,
    PMES     = 0x05,
    BTST     = 0x06,
    DSLP     = 0x07,
    DTM1     = 0x10,
    DSP      = 0x11,
    DRF      = 0x12,
    DTM2     = 0x13,
    LUT_VCOM = 0x20,
    LUT_WW   = 0x21,
    LUT_BW   = 0x22,
    LUT_WB   = 0x23,
    LUT_BB   = 0x24,
    PLL      = 0x30,
    TSC      = 0x40,
    TSE      = 0x41,
    TSR      = 0x43,
    TSW      = 0x42,
    CDI      = 0x50,
    LPD      = 0x51,
    TCON     = 0x60,
    TRES     = 0x61,
    REV      = 0x70,
    FLG      = 0x71,
    AMV      = 0x80,
    VV       = 0x81,
    VDCS     = 0x82,
    PTL      = 0x90,
    PTIN     = 0x91,
    PTOU     = 0x92,
    PGM      = 0xa0,
    APG      = 0xa1,
    ROTP     = 0xa2,
    CCSET    = 0xe0,
    PWS      = 0xe3,
    TSSET    = 0xe5
};

enum PSR_FLAGS {
    RES_96x230   = 0b00000000,
    RES_96x252   = 0b01000000,
    RES_128x296  = 0b10000000,
    RES_160x296  = 0b11000000,

    LUT_OTP      = 0b00000000,
    LUT_REG      = 0b00100000,

    FORMAT_BWR   = 0b00000000,
    FORMAT_BW    = 0b00010000,

    SCAN_DOWN    = 0b00000000,
    SCAN_UP      = 0b00001000,

    SHIFT_LEFT   = 0b00000000,
    SHIFT_RIGHT  = 0b00000100,

    BOOSTER_OFF  = 0b00000000,
    BOOSTER_ON   = 0b00000010,

    RESET_SOFT   = 0b00000000,
    RESET_NONE   = 0b00000001
};

enum PWR_FLAGS_1 {
    VDS_EXTERNAL = 0b00000000,
    VDS_INTERNAL = 0b00000010,

    VDG_EXTERNAL = 0b00000000,
    VDG_INTERNAL = 0b00000001
};

enum PWR_FLAGS_2 {
    VCOM_VD      = 0b00000000,
    VCOM_VG      = 0b00000100,

    VGHL_16V     = 0b00000000,
    VGHL_15V     = 0b00000001,
    VGHL_14V     = 0b00000010,
    VGHL_13V     = 0b00000011
};

enum BOOSTER_FLAGS {
    START_10MS = 0b00000000,
    START_20MS = 0b01000000,
    START_30MS = 0b10000000,
    START_40MS = 0b11000000,

    STRENGTH_1 = 0b00000000,
    STRENGTH_2 = 0b00001000,
    STRENGTH_3 = 0b00010000,
    STRENGTH_4 = 0b00011000,
    STRENGTH_5 = 0b00100000,
    STRENGTH_6 = 0b00101000,
    STRENGTH_7 = 0b00110000,
    STRENGTH_8 = 0b00111000,

    OFF_0_27US = 0b00000000,
    OFF_0_34US = 0b00000001,
    OFF_0_40US = 0b00000010,
    OFF_0_54US = 0b00000011,
    OFF_0_80US = 0b00000100,
    OFF_1_54US = 0b00000101,
    OFF_3_34US = 0b00000110,
    OFF_6_58US = 0b00000111
};

enum PFS_FLAGS {
    FRAMES_1   = 0b00000000,
    FRAMES_2   = 0b00010000,
    FRAMES_3   = 0b00100000,
    FRAMES_4   = 0b00110000
};

enum TSE_FLAGS {
    TEMP_INTERNAL = 0b00000000,
    TEMP_EXTERNAL = 0b10000000,

    OFFSET_0      = 0b00000000,
    OFFSET_1      = 0b00000001,
    OFFSET_2      = 0b00000010,
    OFFSET_3      = 0b00000011,
    OFFSET_4      = 0b00000100,
    OFFSET_5      = 0b00000101,
    OFFSET_6      = 0b00000110,
    OFFSET_7      = 0b00000111,

    OFFSET_MIN_8  = 0b00001000,
    OFFSET_MIN_7  = 0b00001001,
    OFFSET_MIN_6  = 0b00001010,
    OFFSET_MIN_5  = 0b00001011,
    OFFSET_MIN_4  = 0b00001100,
    OFFSET_MIN_3  = 0b00001101,
    OFFSET_MIN_2  = 0b00001110,
    OFFSET_MIN_1  = 0b00001111
};

enum PLL_FLAGS {
    // other frequency options exist but there doesn't seem to be much
    // point in including them - this is a fair range of options...
    HZ_29      = 0b00111111,
    HZ_33      = 0b00111110,
    HZ_40      = 0b00111101,
    HZ_50      = 0b00111100,
    HZ_67      = 0b00111011,
    HZ_100     = 0b00111010,
    HZ_200     = 0b00111001
};

// This is an UC8151 control chip. The display is a 2.9" grayscale EInk.
const uint8_t display_start_sequence[] = {
    PWR, 5, VDS_INTERNAL | VDG_INTERNAL, VCOM_VD | VGHL_16V, 0b101011, 0b101011, 0b101011, // power setting
    PON, DELAY, 200, // power on and wait 200 ms
    BTST, 3, (START_10MS | STRENGTH_3 | OFF_6_58US), (START_10MS | STRENGTH_3 | OFF_6_58US), (START_10MS | STRENGTH_3 | OFF_6_58US),
    PSR, 1, (RES_128x296 | LUT_REG | FORMAT_BW | SCAN_UP | SHIFT_RIGHT | BOOSTER_ON | RESET_NONE),
    PFS, 1, FRAMES_1,
    TSE, 1, TEMP_INTERNAL | OFFSET_0,
    TCON, 1, 0x22, // tcon setting
    CDI, 1, 0b01001100, // vcom and data interval
    PLL, 1, HZ_100, // PLL set to 100 Hz

    // Look up tables for voltage sequence for pixel transition
    // Common voltage
    LUT_VCOM, 44,
    0x00, 0x16, 0x16, 0x0d, 0x00, 0x01,
    0x00, 0x23, 0x23, 0x00, 0x00, 0x02,
    0x00, 0x16, 0x16, 0x0d, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00,

    // White to white
    LUT_WW, 42,
    0x54, 0x16, 0x16, 0x0d, 0x00, 0x01,
    0x60, 0x23, 0x23, 0x00, 0x00, 0x02,
    0xa8, 0x16, 0x16, 0x0d, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    // Black to white
    LUT_BW, 42,
    0x54, 0x16, 0x16, 0x0d, 0x00, 0x01,
    0x60, 0x23, 0x23, 0x00, 0x00, 0x02,
    0xa8, 0x16, 0x16, 0x0d, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    // White to black
    LUT_WB, 42,
    0xa8, 0x16, 0x16, 0x0d, 0x00, 0x01,
    0x60, 0x23, 0x23, 0x00, 0x00, 0x02,
    0x54, 0x16, 0x16, 0x0d, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    // Black to black
    LUT_BB, 42,
    0xa8, 0x16, 0x16, 0x0d, 0x00, 0x01,
    0x60, 0x23, 0x23, 0x00, 0x00, 0x02,
    0x54, 0x16, 0x16, 0x0d, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const uint8_t display_stop_sequence[] = {
    POF, 0x00  // Power off
};

const uint8_t refresh_sequence[] = {
    DRF, 0x00
};

void board_init(void) {
    // Drive the EN_3V3 pin high so the board stays awake on battery power
    enable_pin_obj.base.type = &digitalio_digitalinout_type;
    common_hal_digitalio_digitalinout_construct(&enable_pin_obj, &pin_GPIO10);
    common_hal_digitalio_digitalinout_switch_to_output(&enable_pin_obj, true, DRIVE_MODE_PUSH_PULL);

    // Never reset
    common_hal_digitalio_digitalinout_never_reset(&enable_pin_obj);

    // Set up the SPI object used to control the display
    busio_spi_obj_t *spi = common_hal_board_create_spi(0);
    common_hal_busio_spi_never_reset(spi);

    // Set up the DisplayIO pin object
    displayio_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    bus->base.type = &displayio_fourwire_type;
    common_hal_displayio_fourwire_construct(bus,
        spi,
        &pin_GPIO20, // EPD_DC Command or data
        &pin_GPIO17, // EPD_CS Chip select
        &pin_GPIO21, // EPD_RST Reset
        1200000, // Baudrate
        0, // Polarity
        0); // Phase

    // Set up the DisplayIO epaper object
    displayio_epaperdisplay_obj_t *display = &allocate_display()->epaper_display;
    display->base.type = &displayio_epaperdisplay_type;
    common_hal_displayio_epaperdisplay_construct(
        display,
        bus,
        display_start_sequence, sizeof(display_start_sequence),
        0, // start up time
        display_stop_sequence, sizeof(display_stop_sequence),
        296,  // width
        128,  // height
        160,  // ram_width
        296,  // ram_height
        0,  // colstart
        0,  // rowstart
        270,  // rotation
        NO_COMMAND,  // set_column_window_command
        NO_COMMAND,  // set_row_window_command
        NO_COMMAND,  // set_current_column_command
        NO_COMMAND,  // set_current_row_command
        DTM2,  // write_black_ram_command
        true,  // black_bits_inverted
        DTM1,  // write_color_ram_command
        false,  // color_bits_inverted
        0x000000,  // highlight_color
        refresh_sequence, sizeof(refresh_sequence),  // refresh_display_command
        1.0,  // refresh_time
        &pin_GPIO26,  // busy_pin
        false,  // busy_state
        2.0, // seconds_per_frame
        false,  // always_toggle_chip_select
        false, // grayscale
        false, // acep
        false,  // two_byte_sequence_length
        false); // address_little_endian
}

void board_deinit(void) {
    displayio_epaperdisplay_obj_t *display = &displays[0].epaper_display;
    if (display->base.type == &displayio_epaperdisplay_type) {
        while (common_hal_displayio_epaperdisplay_get_busy(display)) {
            RUN_BACKGROUND_TASKS;
        }
    }
    common_hal_displayio_release_displays();
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
