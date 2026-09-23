// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"
#include "shared-bindings/board/__init__.h"
#include "shared-bindings/digitalio/DigitalInOut.h"

// SSD1683 EPD Driver chip
const uint8_t display_start_sequence[] = {
    // Init
    0x12, 0x80, 0x0a,                   // Soft reset
    0x01, 0x03, 0x2b, 0x01, 0x00,       // Set MUX as 300
    0x21, 0x02, 0x40, 0x00,             // Display update control
    0x3c, 0x01, 0x01,                   // Border waveform
    0x11, 0x01, 0x03,                   // X- mode
    0x44, 0x02, 0x00, 0x31,             // Set RAM X Address Start/End Pos
    0x45, 0x04, 0x00, 0x00, 0x2b, 0x01, // Set RAM Y Address Start/End Pos
    0x4e, 0x01, 0x00,                   // Set RAM X Address counter
    0x4f, 0x02, 0x00, 0x00,             // Set RAM Y Address counter
};

const uint8_t display_stop_sequence[] = {
    0x10, 0x01, 0x32,
};

const uint8_t refresh_sequence[] = {
    0x22, 0x01, 0xf7,                   // Display update sequence option
    0x20, 0x80, 0x0a,                   // Master activation
};

void board_init(void) {

    // Enable EPD with driver pin
    digitalio_digitalinout_obj_t epd_enable_pin_obj;
    epd_enable_pin_obj.base.type = &digitalio_digitalinout_type;
    common_hal_digitalio_digitalinout_construct(&epd_enable_pin_obj, &pin_GPIO7);
    common_hal_digitalio_digitalinout_switch_to_output(&epd_enable_pin_obj, true, DRIVE_MODE_PUSH_PULL);

    // Set up the SPI object used to control the display
    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    busio_spi_obj_t *spi = &bus->inline_bus;
    common_hal_busio_spi_construct(spi, &pin_GPIO12, &pin_GPIO11, NULL, false);

    // Set up the DisplayIO pin object
    bus->base.type = &fourwire_fourwire_type;
    common_hal_fourwire_fourwire_construct(bus,
        spi,
        MP_OBJ_FROM_PTR(&pin_GPIO46), // EPD_DC Command or data
        MP_OBJ_FROM_PTR(&pin_GPIO45), // EPD_CS Chip select
        MP_OBJ_FROM_PTR(&pin_GPIO47), // EPD_RST Reset
        1000000, // Baudrate
        0, // Polarity
        0); // Phase

    // Set up the DisplayIO epaper object
    epaperdisplay_epaperdisplay_obj_t *display = &allocate_display()->epaper_display;
    display->base.type = &epaperdisplay_epaperdisplay_type;

    epaperdisplay_construct_args_t args = EPAPERDISPLAY_CONSTRUCT_ARGS_DEFAULTS;
    args.bus = bus;
    args.start_sequence = display_start_sequence;
    args.start_sequence_len = sizeof(display_start_sequence);
    args.start_up_time = 1.0;
    args.stop_sequence = display_stop_sequence;
    args.stop_sequence_len = sizeof(display_stop_sequence);
    args.width = 400;
    args.height = 300;
    args.ram_width = 400;
    args.ram_height = 300;
    args.write_black_ram_command = 0x24;
    args.write_color_ram_command = 0x26;
    args.refresh_sequence = refresh_sequence;
    args.refresh_sequence_len = sizeof(refresh_sequence);
    args.refresh_time = 1.0;
    args.busy_pin = &pin_GPIO48;
    args.busy_state = true;
    args.seconds_per_frame = 2.0;
    common_hal_epaperdisplay_epaperdisplay_construct(display, &args);
}

void board_deinit(void) {
    epaperdisplay_epaperdisplay_obj_t *display = &displays[0].epaper_display;
    if (display->base.type == &epaperdisplay_epaperdisplay_type) {
        while (common_hal_epaperdisplay_epaperdisplay_get_busy(display)) {
            RUN_BACKGROUND_TASKS;
        }
    }
    common_hal_displayio_release_displays();
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
