// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"

#include "mpconfigboard.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"
#include "shared-bindings/board/__init__.h"
#include "supervisor/shared/board.h"
#include "inky-shared.h"

#define DELAY 0x80

digitalio_digitalinout_obj_t enable_pin_obj;

// This is an SPD1656 control chip. The display is a 5.7" ACeP EInk.

const uint8_t display_start_sequence[] = {
    0x01, 4, 0x37, 0x00, 0x23, 0x23, // power setting
    0x00, 2, 0xe3, 0x08, // panel setting (PSR, 0xe3: no rotation)
    0x03, 1, 0x00, // PFS
    0x06, 3, 0xc7, 0xc7, 0x1d, // booster
    0x30, 1, 0x3c, // PLL setting
    0x41, 1, 0x00, // TSE
    0x50, 1, 0x37, // vcom and data interval setting
    0x60, 1, 0x22, // tcon setting
    0x61, 4, 0x02, 0x58, 0x01, 0xc0, // tres
    0xe3, 1, 0xaa, // PWS
    0x04, DELAY | 0, 0xc8, // VCM DC and delay 200ms
};

const uint8_t display_stop_sequence[] = {
    0x02, 1, 0x00, // power off
    0x07, 1, 0xa5 // deep sleep
};

const uint8_t refresh_sequence[] = {
    0x12, 0x00
};

void board_init(void) {
    // Drive the EN_3V3 pin high so the board stays awake on battery power
    enable_pin_obj.base.type = &digitalio_digitalinout_type;
    common_hal_digitalio_digitalinout_construct(&enable_pin_obj, &pin_GPIO2);
    common_hal_digitalio_digitalinout_switch_to_output(&enable_pin_obj, true, DRIVE_MODE_PUSH_PULL);

    // Never reset
    common_hal_digitalio_digitalinout_never_reset(&enable_pin_obj);
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
        600,  // width
        448,  // height
        640,  // ram_width
        480,  // ram_height
        0,  // colstart
        0,  // rowstart
        0,  // rotation
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
        40.0, // seconds_per_frame
        false,  // always_toggle_chip_select
        false, // grayscale
        true, // acep
        false,  // two_byte_sequence_length
        false); // address_little_endian
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
