// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Brian Nelson
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"

#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"
#include "supervisor/shared/settings.h"

// ST7796 init sequence from Freenove's TFT_eSPI setup, landscape (MADCTL MV | BGR)
static uint8_t display_init_sequence[] = {
    0x01, 0x80, 0x78, // Software reset, delay 120 ms
    0x11, 0x80, 0x78, // Sleep out, delay 120 ms
    0xF0, 0x01, 0xC3, // Enable extension command 2 part I
    0xF0, 0x01, 0x96, // Enable extension command 2 part II
    0x36, 0x01, 0x28, // Memory data access control: row/column exchange, BGR
    0x3A, 0x01, 0x55, // 16 bits per pixel
    0xB4, 0x01, 0x01, // 1-dot inversion
    0xB6, 0x03, 0x80, 0x02, 0x3B, // Display function control
    0xE8, 0x08, 0x40, 0x8A, 0x00, 0x00, 0x29, 0x19, 0xA5, 0x33, // Display output control adjust
    0xC1, 0x01, 0x06, // Power control 2
    0xC2, 0x01, 0xA7, // Power control 3
    0xC5, 0x81, 0x18, 0x78, // VCOM control, delay 120 ms
    0xE0, 0x0E, 0xF0, 0x09, 0x0B, 0x06, 0x04, 0x15, 0x2F, 0x54, 0x42, 0x3C, 0x17, 0x14, 0x18, 0x1B, // Positive gamma
    0xE1, 0x8E, 0xE0, 0x09, 0x0B, 0x06, 0x04, 0x03, 0x2B, 0x43, 0x42, 0x3B, 0x16, 0x14, 0x17, 0x1B, 0x78, // Negative gamma, delay 120 ms
    0xF0, 0x01, 0x3C, // Disable extension command 2 part I
    0xF0, 0x81, 0x69, 0x78, // Disable extension command 2 part II, delay 120 ms
    0x29, 0x00, // Display on
};

static void display_init(void) {
    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    busio_spi_obj_t *spi = &bus->inline_bus;
    mp_int_t rotation;

    common_hal_busio_spi_construct(spi, &pin_GPIO14, &pin_GPIO13, &pin_GPIO12, false);

    bus->base.type = &fourwire_fourwire_type;
    common_hal_fourwire_fourwire_construct(bus,
        spi,
        MP_OBJ_FROM_PTR(&pin_GPIO2), // LCD_DC
        MP_OBJ_FROM_PTR(&pin_GPIO15), // LCD_CS
        mp_const_none, // Reset is tied to the ESP32 reset
        26666666, // Baudrate: ESP32 SPI through the GPIO matrix tops out at 26.67 MHz
        0, // Polarity
        0); // Phase

    busdisplay_busdisplay_obj_t *display = &allocate_display()->display;
    display->base.type = &busdisplay_busdisplay_type;
    if (settings_get_int("CIRCUITPY_DISPLAY_ROTATION", &rotation) != SETTINGS_OK) {
        rotation = 0;
    }

    common_hal_busdisplay_busdisplay_construct(display,
        bus,
        480, // Width
        320, // Height
        0, // column start
        0, // row start
        rotation, // rotation
        16, // Color depth
        false, // Grayscale
        false, // pixels in a byte share a row. Only valid for depths < 8
        1, // bytes per cell. Only valid for depths < 8
        false, // reverse_pixels_in_byte. Only valid for depths < 8
        true, // reverse_pixels_in_word
        MIPI_COMMAND_SET_COLUMN_ADDRESS, // Set column command
        MIPI_COMMAND_SET_PAGE_ADDRESS, // Set row command
        MIPI_COMMAND_WRITE_MEMORY_START, // Write memory command
        display_init_sequence,
        sizeof(display_init_sequence),
        &pin_GPIO27, // backlight pin
        NO_BRIGHTNESS_COMMAND,
        1.0f, // brightness
        false, // single_byte_bounds
        false, // data_as_commands
        true, // auto_refresh
        60, // native_frames_per_second
        true, // backlight_on_high
        false, // SH1107_addressing
        50000); // backlight pwm frequency
}

void board_init(void) {
    display_init();
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
