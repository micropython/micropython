// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "src/rp2_common/hardware_gpio/include/hardware/gpio.h"
#include "supervisor/board.h"
#include "supervisor/shared/board.h"


#define DELAY 0x80

uint8_t display_init_sequence[] = {
    0xae, 0, // sleep
    0xd5, 1, 0x80, // fOsc divide by 2
    0xa8, 1, 0x3f, // multiplex 64
    0xd3, 1, 0x00, // offset 0
    0x40, 1, 0x00, // start line 0
    0xad, 1, 0x8b, // dc/dc on
    0xa1, 0, // segment remap = 0
    0xc8, 0, // scan incr
    0xda, 1, 0x12, // com pins
    0x81, 1, 0xff, // contrast 255
    0xd9, 1, 0x1f, // pre/dis-charge 2DCLKs/2CLKs
    0xdb, 1, 0x20, // VCOM deslect 0.770
    0x20, 1, 0x20,
    0x33, 0, // VPP 9V
    0xa6, 0, // not inverted
    0xa4, 0, // normal
    0xaf, 0, // on
};

void board_init(void) {
    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    busio_spi_obj_t *spi = &bus->inline_bus;
    common_hal_busio_spi_construct(spi, &pin_GPIO26, &pin_GPIO27, NULL, false);
    common_hal_busio_spi_never_reset(spi);

    bus->base.type = &fourwire_fourwire_type;
    common_hal_fourwire_fourwire_construct(bus,
        spi,
        &pin_GPIO24, // Command or data
        &pin_GPIO22, // Chip select
        &pin_GPIO23, // Reset
        10000000, // Baudrate
        0, // Polarity
        0); // Phase

    busdisplay_busdisplay_obj_t *display = &allocate_display()->display;
    display->base.type = &busdisplay_busdisplay_type;
    common_hal_busdisplay_busdisplay_construct(display,
        bus,
        128, // Width
        64, // Height
        2, // column start
        0, // row start
        0, // rotation
        1, // Color depth
        true, // grayscale
        false, // pixels in byte share row. Only used with depth < 8
        1, // bytes per cell. Only valid for depths < 8
        false, // reverse_pixels_in_byte. Only valid for depths < 8
        true, // reverse_pixels_in_word
        0, // Set column command
        0, // Set row command
        0, // Write memory command
        display_init_sequence,
        sizeof(display_init_sequence),
        NULL,
        0x81,
        1.0f, // brightness
        true, // single_byte_bounds
        true, // data as commands
        true, // auto_refresh
        60, // native_frames_per_second
        true, // backlight_on_high
        true, // SH1107_addressing
        50000); // backlight pwm frequency
}

void reset_board(void) {
    // turn off any left over LED
    board_reset_user_neopixels(&pin_GPIO19, 12);
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
