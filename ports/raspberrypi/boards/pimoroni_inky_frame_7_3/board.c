// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"

#include "mpconfigboard.h"
#include "shared-bindings/board/__init__.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/time/__init__.h"
#include "shared-module/displayio/__init__.h"
#include "supervisor/shared/board.h"

#define DELAY 0x80

// This is a 7.3" ACeP EInk.

digitalio_digitalinout_obj_t enable_pin_obj;

digitalio_digitalinout_obj_t sr_clock;
digitalio_digitalinout_obj_t sr_data;
digitalio_digitalinout_obj_t sr_latch;

enum reg {
    PSR = 0x00,
    PWR = 0x01,
    POF = 0x02,
    PFS = 0x03,
    PON = 0x04,
    BTST1 = 0x05,
    BTST2 = 0x06,
    DSLP = 0x07,
    BTST3 = 0x08,
    DTM1 = 0x10,
    DSP = 0x11,
    DRF = 0x12,
    IPC = 0x13,
    PLL = 0x30,
    TSC = 0x40,
    TSE = 0x41,
    TSW = 0x42,
    TSR = 0x43,
    CDI = 0x50,
    LPD = 0x51,
    TCON = 0x60,
    TRES = 0x61,
    DAM = 0x65,
    REV = 0x70,
    FLG = 0x71,
    AMV = 0x80,
    VV = 0x81,
    VDCS = 0x82,
    T_VDCS = 0x84,
    AGID = 0x86,
    CMDH = 0xAA,
    CCSET =0xE0,
    PWS = 0xE3,
    TSSET = 0xE6 // E5 or E6
};

const uint8_t display_start_sequence[] = {
    CMDH, 6, 0x49, 0x55, 0x20, 0x08, 0x09, 0x18,
    PWR, 6, 0x3F, 0x00, 0x32, 0x2A, 0x0E, 0x2A,
    PSR, 2, 0x5F, 0x69,
    PFS, 4, 0x00, 0x54, 0x00, 0x44,
    BTST1, 4, 0x40, 0x1F, 0x1F, 0x2C,
    BTST2, 4, 0x6F, 0x1F, 0x16, 0x25,
    BTST3, 4, 0x6F, 0x1F, 0x1F, 0x22,
    IPC, 2, 0x00, 0x04,
    PLL, 1, 0x02,
    TSE, 1, 0x00,
    CDI, 1, 0x3F,
    TCON, 2, 0x02, 0x00,
    TRES, 4, 0x03, 0x20, 0x01, 0xE0,
    VDCS, 1, 0x1E,
    T_VDCS, 1, 0x00,
    AGID, 1, 0x00,
    PWS, 1, 0x2F,
    CCSET, 1, 0x00,
    TSSET, 1, 0x00,
    PON, DELAY, 0xc8,
};

const uint8_t display_stop_sequence[] = {
    POF, 0,
};

const uint8_t refresh_sequence[] = {
    DRF, 1, 0x00,
};

void board_init(void) {
    // Drive the EN_3V3 pin high so the board stays awake on battery power
    enable_pin_obj.base.type = &digitalio_digitalinout_type;
    common_hal_digitalio_digitalinout_construct(&enable_pin_obj, &pin_GPIO2);
    common_hal_digitalio_digitalinout_switch_to_output(&enable_pin_obj, true, DRIVE_MODE_PUSH_PULL);

    // Never reset
    common_hal_digitalio_digitalinout_never_reset(&enable_pin_obj);

    common_hal_digitalio_digitalinout_construct(&sr_clock, &pin_GPIO8);
    common_hal_digitalio_digitalinout_switch_to_output(&sr_clock, false, DRIVE_MODE_PUSH_PULL);

    common_hal_digitalio_digitalinout_construct(&sr_data, &pin_GPIO10);
    common_hal_digitalio_digitalinout_switch_to_input(&sr_data, PULL_NONE);

    common_hal_digitalio_digitalinout_construct(&sr_latch, &pin_GPIO9);
    common_hal_digitalio_digitalinout_switch_to_output(&sr_latch, true, DRIVE_MODE_PUSH_PULL);

    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    busio_spi_obj_t *spi = common_hal_board_create_spi(0);

    bus->base.type = &fourwire_fourwire_type;
    common_hal_fourwire_fourwire_construct(bus,
        spi,
        &pin_GPIO28, // EPD_DC Command or data
        &pin_GPIO17, // EPD_CS Chip select
        &pin_GPIO27, // EPD_RST Reset
        1000000, // Baudrate
        0, // Polarity
        0); // Phase

    epaperdisplay_epaperdisplay_obj_t *display = &allocate_display()->epaper_display;
    display->base.type = &epaperdisplay_epaperdisplay_type;
    common_hal_epaperdisplay_epaperdisplay_construct(
        display,
        bus,
        display_start_sequence, sizeof(display_start_sequence),
        1.0, // start up time
        display_stop_sequence, sizeof(display_stop_sequence),
        800,  // width
        480,  // height
        800,  // ram_width
        480,  // ram_height
        0,  // colstart
        0,  // rowstart
        180,  // rotation
        NO_COMMAND,  // set_column_window_command
        NO_COMMAND,  // set_row_window_command
        NO_COMMAND,  // set_current_column_command
        NO_COMMAND,  // set_current_row_command
        0x10,  // write_black_ram_command
        false,  // black_bits_inverted
        NO_COMMAND,  // write_color_ram_command
        false,  // color_bits_inverted
        0x000000,  // highlight_color
        refresh_sequence, sizeof(refresh_sequence),
        28.0,  // refresh_time
        NULL,  // busy_pin
        false,  // busy_state
        30.0, // seconds_per_frame
        false,  // always_toggle_chip_select
        false, // grayscale
        true, // acep
        false,  // two_byte_sequence_length
        false); // address_little_endian
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
