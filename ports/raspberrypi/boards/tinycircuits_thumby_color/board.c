// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Cooper Dalrymple
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


#define DELAY 0x80

// display init sequence according to TinyCircuits-Tiny-Game-Engine
uint8_t display_init_sequence[] = {
    0xFE, 0, // inter register enable 1
    0xEF, 0, // inter register enable 2
    0xB0, 1, 0xC0,
    0xB1, 1, 0x80,
    0xB2, 1, 0x2F,
    0xB3, 1, 0x03,
    0xB7, 1, 0x01,
    0xB6, 1, 0x19,
    0xAC, 1, 0xC8, // Complement Principle of RGB 5, 6, 5
    0xAB, 1, 0x0f, // ?
    0x3A, 1, 0x05, // COLMOD: Pixel Format Set
    0xB4, 1, 0x04, // ?
    0xA8, 1, 0x07, // Frame Rate Set
    0xB8, 1, 0x08, // ?
    0xE7, 1, 0x5A, // VREG_CTL
    0xE8, 1, 0x23, // VGH_SET
    0xE9, 1, 0x47, // VGL_SET
    0xEA, 1, 0x99, // VGH_VGL_CLK
    0xC6, 1, 0x30, // ?
    0xC7, 1, 0x1F, // ?
    0xF0, 14, 0x05, 0x1D, 0x51, 0x2F, 0x85, 0x2A, 0x11, 0x62, 0x00, 0x07, 0x07, 0x0F, 0x08, 0x1F, // SET_GAMMA1
    0xF1, 14, 0x2E, 0x41, 0x62, 0x56, 0xA5, 0x3A, 0x3f, 0x60, 0x0F, 0x07, 0x0A, 0x18, 0x18, 0x1D, // SET_GAMMA2
    0x11, 0 | DELAY, 120,
    0x29, 0 | DELAY, 10, // display on
};

void board_init(void) {
    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    busio_spi_obj_t *spi = &bus->inline_bus;
    common_hal_busio_spi_construct(
        spi,
        DEFAULT_SPI_BUS_SCK,    // CLK
        DEFAULT_SPI_BUS_MOSI,   // MOSI
        NULL,                   // MISO not connected
        false                   // Not half-duplex
        );

    bus->base.type = &fourwire_fourwire_type;

    common_hal_fourwire_fourwire_construct(
        bus,
        spi,
        MP_OBJ_FROM_PTR(CIRCUITPY_BOARD_LCD_DC),     // DC
        MP_OBJ_FROM_PTR(CIRCUITPY_BOARD_LCD_CS),     // CS
        MP_OBJ_FROM_PTR(CIRCUITPY_BOARD_LCD_RESET),  // RST
        80000000,                                    // baudrate
        0,                                           // polarity
        0                                            // phase
        );

    busdisplay_busdisplay_obj_t *display = &allocate_display()->display;
    display->base.type = &busdisplay_busdisplay_type;
    common_hal_busdisplay_busdisplay_construct(
        display,
        bus,
        128,                                // width (after rotation)
        128,                                // height (after rotation)
        0,                                  // column start
        0,                                  // row start
        0,                                  // rotation
        16,                                 // color depth
        false,                              // grayscale
        false,                              // pixels in a byte share a row. Only valid for depths < 8
        1,                                  // bytes per cell. Only valid for depths < 8
        false,                              // reverse_pixels_in_byte. Only valid for depths < 8
        true,                               // reverse_pixels_in_word
        MIPI_COMMAND_SET_COLUMN_ADDRESS,    // set column command
        MIPI_COMMAND_SET_PAGE_ADDRESS,      // set row command
        MIPI_COMMAND_WRITE_MEMORY_START,    // write memory command
        display_init_sequence,
        sizeof(display_init_sequence),
        CIRCUITPY_BOARD_LCD_BACKLIGHT,      // backlight pin
        NO_BRIGHTNESS_COMMAND,
        1.0f,                               // brightness
        false,                              // single_byte_bounds
        false,                              // data_as_commands
        true,                               // auto_refresh
        60,                                 // native_frames_per_second
        true,                               // backlight_on_high
        false,                              // SH1107_addressing
        50000                               // backlight pwm frequency
        );
}

void reset_board(void) {
}
