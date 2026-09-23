// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Brian Nelson
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"

#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/busdisplay/BusDisplay.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"

#define DELAY 0x80

// Power hold: the function key powers the board up; SYS_EN must stay high
// to keep it running from the battery once the key is released.
#define SYS_EN_PIN (GPIO_NUM_41)

// ST7789V2, 240x280 glass on a 240x320 controller (20 row offset)
static uint8_t display_init_sequence[] = {
    0x01, 0 | DELAY, 150, // SWRESET
    0x11, 0 | DELAY, 255, // SLPOUT
    0x3A, 1 | DELAY, 0x55, 10, // COLMOD: 16 bits per pixel
    0x36, 1, 0x00, // MADCTL: portrait
    0x21, 0 | DELAY, 10, // INVON: this panel is inverted
    0x13, 0 | DELAY, 10, // NORON
    0x29, 0 | DELAY, 255, // DISPON
};

static void display_init(void) {
    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    busio_spi_obj_t *spi = &bus->inline_bus;
    common_hal_busio_spi_construct(spi, &pin_GPIO6, &pin_GPIO7, NULL, false);

    bus->base.type = &fourwire_fourwire_type;
    common_hal_fourwire_fourwire_construct(
        bus,
        spi,
        MP_OBJ_FROM_PTR(&pin_GPIO4),    // DC
        MP_OBJ_FROM_PTR(&pin_GPIO5),    // CS
        MP_OBJ_FROM_PTR(&pin_GPIO8),    // RST
        80000000,       // baudrate
        0,              // polarity
        0               // phase
        );

    busdisplay_busdisplay_obj_t *display = &allocate_display()->display;
    display->base.type = &busdisplay_busdisplay_type;
    common_hal_busdisplay_busdisplay_construct(
        display,
        bus,
        240,            // width (after rotation)
        280,            // height (after rotation)
        0,              // column start
        20,             // row start
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
        50000           // backlight pwm frequency
        );
}

void board_init(void) {
    display_init();
}

bool espressif_board_reset_pin_number(gpio_num_t pin_number) {
    // Keep the power latch asserted across resets so the board stays on from the battery
    if (pin_number == SYS_EN_PIN) {
        config_pin_as_output_with_level(pin_number, true);
        return true;
    }
    return false;
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
