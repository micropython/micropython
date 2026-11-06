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

const uint8_t display_start_sequence[] = {
    0x12, 0x80, 0x14,           // Soft reset and wait 20ms
    0x11, 0x01, 0x03,           // Ram data entry mode
    0x3c, 0x01, 0x05,           // border color
    0x2c, 0x01, 0x36,           // set vcom voltage
    0x03, 0x01, 0x17,           // Set gate voltage
    0x04, 0x03, 0x41, 0x00, 0x32, // Set source voltage
    0x4e, 0x01, 0x01,           // ram x count
    0x4f, 0x02, 0x00, 0x00,     // ram y count
    0x01, 0x03, 0x27, 0x01, 0x00, // set display size
    0x22, 0x01, 0xf4,           // display update mode
};

const uint8_t display_stop_sequence[] = {
    0x10, 0x81, 0x01, 0x64,     // Deep sleep
};

const uint8_t refresh_sequence[] = {
    0x20,
};

void board_init(void) {

    // Set vext high to enable EPD
    digitalio_digitalinout_obj_t vext_pin_obj;
    vext_pin_obj.base.type = &digitalio_digitalinout_type;
    common_hal_digitalio_digitalinout_construct(&vext_pin_obj, &pin_GPIO18);
    common_hal_digitalio_digitalinout_switch_to_output(&vext_pin_obj, true, DRIVE_MODE_PUSH_PULL);

    // Set up the SPI object used to control the display
    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    busio_spi_obj_t *spi = &bus->inline_bus;
    common_hal_busio_spi_construct(spi, &pin_GPIO2, &pin_GPIO1, NULL, false);

    // Set up the DisplayIO pin object
    bus->base.type = &fourwire_fourwire_type;
    common_hal_fourwire_fourwire_construct(bus,
        spi,
        MP_OBJ_FROM_PTR(&pin_GPIO4), // EPD_DC Command or data
        MP_OBJ_FROM_PTR(&pin_GPIO3), // EPD_CS Chip select
        MP_OBJ_FROM_PTR(&pin_GPIO5), // EPD_RST Reset
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
    args.stop_sequence = display_stop_sequence;
    args.stop_sequence_len = sizeof(display_stop_sequence);
    args.width = 296;
    args.height = 128;
    args.ram_width = 250;
    args.ram_height = 296;
    args.colstart = 8;
    args.rotation = 90;
    args.set_column_window_command = 0x44;
    args.set_row_window_command = 0x45;
    args.set_current_column_command = 0x4E;
    args.set_current_row_command = 0x4F;
    args.write_black_ram_command = 0x24;
    args.write_color_ram_command = 0x26;
    args.highlight_color = 0xFF0000;
    args.refresh_sequence = refresh_sequence;
    args.refresh_sequence_len = sizeof(refresh_sequence);
    args.refresh_time = 1.0;
    args.busy_pin = &pin_GPIO6;
    args.busy_state = true;
    args.seconds_per_frame = 2.0;
    args.address_little_endian = true;
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
