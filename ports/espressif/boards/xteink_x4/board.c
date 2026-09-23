// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
// SPDX-FileCopyrightText: Copyright (c) 2021 skieast/Bruce Segal
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"

#include "mpconfigboard.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"
#include "supervisor/shared/board.h"

#define DELAY 0x80

// SSD1677 controller driving a GDEQ0426T82 4.26" 800x480 E-Ink display.

const uint8_t ssd1677_display_start_sequence[] = {
    // Software Reset
    0x12, DELAY, 0x00, 0x14,

    // Temperature Sensor Control (use internal sensor)
    0x18, 0x00, 0x01, 0x80,

    // Booster Soft Start
    0x0C, 0x00, 0x05, 0xAE, 0xC7, 0xC3, 0xC0, 0x40,

    // Driver Output Control: 480 gates, GD=0, SM=1, TB=0 = 0x02
    0x01, 0x00, 0x03, 0xDF, 0x01, 0x02,

    // Data Entry Mode: X increment, Y decrement = 0x01
    0x11, 0x00, 0x01, 0x01,

    // Border Waveform Control
    0x3C, 0x00, 0x01, 0x01,

    // Set RAM X Address Start/End: 0 to 799
    0x44, 0x00, 0x04, 0x00, 0x00, 0x1F, 0x03,

    // Set RAM Y Address Start/End: 479 down to 0
    0x45, 0x00, 0x04, 0xDF, 0x01, 0x00, 0x00,

    // Set RAM X Counter to 0
    0x4E, 0x00, 0x02, 0x00, 0x00,

    // Set RAM Y Counter to 479
    0x4F, 0x00, 0x02, 0xDF, 0x01,

    // Auto Write BW RAM (clear to white)
    0x46, DELAY, 0x01, 0xF7, 0xFF,

    // Display Update Control 1: bypass RED
    0x21, 0x00, 0x02, 0x40, 0x00,

    // Display Update Control 2: full refresh with OTP LUT
    0x22, 0x00, 0x01, 0xF7,
};

const uint8_t ssd1677_display_stop_sequence[] = {
    0x22, 0x00, 0x01, 0x83,
    0x20, 0x00, 0x00,
    0x10, 0x00, 0x01, 0x01,
};

const uint8_t ssd1677_display_refresh_sequence[] = {
    0x20, 0x00, 0x00,
};

void board_init(void) {
    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    busio_spi_obj_t *spi = &bus->inline_bus;
    common_hal_busio_spi_construct(spi, &pin_GPIO8, &pin_GPIO10, NULL, false);

    bus->base.type = &fourwire_fourwire_type;
    common_hal_fourwire_fourwire_construct(bus,
        spi,
        MP_OBJ_FROM_PTR(&pin_GPIO4),
        MP_OBJ_FROM_PTR(&pin_GPIO21),
        MP_OBJ_FROM_PTR(&pin_GPIO5),
        40000000,
        0,
        0);

    epaperdisplay_epaperdisplay_obj_t *display = &allocate_display()->epaper_display;
    display->base.type = &epaperdisplay_epaperdisplay_type;

    epaperdisplay_construct_args_t args = EPAPERDISPLAY_CONSTRUCT_ARGS_DEFAULTS;
    args.bus = bus;
    args.start_sequence = ssd1677_display_start_sequence;
    args.start_sequence_len = sizeof(ssd1677_display_start_sequence);
    args.stop_sequence = ssd1677_display_stop_sequence;
    args.stop_sequence_len = sizeof(ssd1677_display_stop_sequence);
    args.width = 800;
    args.height = 480;
    args.ram_width = 800;
    args.ram_height = 480;
    args.rotation = 0;
    args.write_black_ram_command = 0x24;
    args.black_bits_inverted = false;
    args.refresh_sequence = ssd1677_display_refresh_sequence;
    args.refresh_sequence_len = sizeof(ssd1677_display_refresh_sequence);
    args.refresh_time = 1.6;
    args.busy_pin = &pin_GPIO6;
    args.busy_state = true;
    args.seconds_per_frame = 5.0;
    args.grayscale = false;
    args.two_byte_sequence_length = true;
    args.address_little_endian = true;
    common_hal_epaperdisplay_epaperdisplay_construct(display, &args);
}

bool espressif_board_reset_pin_number(gpio_num_t pin_number) {
    return false;
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
