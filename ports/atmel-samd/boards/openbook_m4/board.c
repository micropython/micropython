// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Joey Castillo
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "hal/include/hal_gpio.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-bindings/time/__init__.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"


#define DELAY 0x80
#define HEIGHT 400
#define WIDTH 300

uint8_t start_sequence[] = {
    0x01, 0x04, 0x03, 0x00, 0x2b, 0x2b, // power setting
    0x06, 0x03, 0x17, 0x17, 0x17, // booster soft start
    0x04, 0x80, 0xc8, // power on and wait 200 ms
    0x00, 0x01, 0x0f, // panel setting
    0x61, 0x04, (HEIGHT >> 8) & 0xFF, HEIGHT & 0xFF, (WIDTH >> 8) & 0xFF, WIDTH & 0xFF // Resolution
};

uint8_t stop_sequence[] = {
    0x50, 0x01, 0xf7, // CDI setting
    0x02, 0x80, 0xf0  // Power off
};

uint8_t refresh_sequence[] = {
    0x12, 0x00
};

void board_init(void) {
    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    busio_spi_obj_t *spi = &bus->inline_bus;
    common_hal_busio_spi_construct(spi, &pin_PB13, &pin_PB15, NULL, false);

    bus->base.type = &fourwire_fourwire_type;
    common_hal_fourwire_fourwire_construct(bus,
        spi,
        MP_OBJ_FROM_PTR(&pin_PB05), // EPD_DC Command or data
        MP_OBJ_FROM_PTR(&pin_PB07), // EPD_CS Chip select
        MP_OBJ_FROM_PTR(&pin_PA00), // EPD_RST Reset
        1000000, // Baudrate
        0, // Polarity
        0); // Phase

    epaperdisplay_epaperdisplay_obj_t *display = &allocate_display()->epaper_display;
    display->base.type = &epaperdisplay_epaperdisplay_type;
    epaperdisplay_construct_args_t args = EPAPERDISPLAY_CONSTRUCT_ARGS_DEFAULTS;
    args.bus = bus;
    args.start_sequence = start_sequence;
    args.start_sequence_len = sizeof(start_sequence);
    args.stop_sequence = stop_sequence;
    args.stop_sequence_len = sizeof(stop_sequence);
    args.width = 300;
    args.height = 400;
    args.ram_width = 300;
    args.ram_height = 400;
    args.rotation = 270;
    args.write_black_ram_command = 0x13;
    args.refresh_sequence = refresh_sequence;
    args.refresh_sequence_len = sizeof(refresh_sequence);
    args.refresh_time = 40.0;
    args.busy_pin = &pin_PA01;
    args.seconds_per_frame = 5.0;
    common_hal_epaperdisplay_epaperdisplay_construct(display, &args);
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
