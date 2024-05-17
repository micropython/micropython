// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"

#define DELAY 0x80

/* All init code scraped from Adafruit_GC9A01A driver */
#define GC9A01A_SWRESET 0x01   ///< Software Reset (maybe, not documented)
#define GC9A01A_INREGEN1 0xFE  ///< Inter register enable 1
#define GC9A01A_INREGEN2 0xEF  ///< Inter register enable 2
#define GC9A01A_MADCTL 0x36    ///< Memory Access Control
#define GC9A01A_COLMOD 0x3A    ///< Pixel Format Set
#define GC9A01A1_POWER2 0xC3   ///< Power Control 2
#define GC9A01A1_POWER3 0xC4   ///< Power Control 3
#define GC9A01A1_POWER4 0xC9   ///< Power Control 4
#define GC9A01A_GAMMA1 0xF0    ///< Set gamma 1
#define GC9A01A_GAMMA2 0xF1    ///< Set gamma 2
#define GC9A01A_GAMMA3 0xF2    ///< Set gamma 3
#define GC9A01A_GAMMA4 0xF3    ///< Set gamma 4
#define GC9A01A_TEON 0x35      ///< Tearing Effect Line ON
#define GC9A01A_INVON 0x21     ///< Display Inversion ON
#define GC9A01A_SLPOUT 0x11    ///< Sleep Out
#define GC9A01A_DISPON 0x29    ///< Display ON
#define GC9A01A_FRAMERATE 0xE8 ///< Frame rate control

#define MADCTL_MX 0x40  ///< Right to left
#define MADCTL_BGR 0x08 ///< Blue-Green-Red pixel order

uint8_t display_init_sequence[] = {
    GC9A01A_SWRESET, DELAY, 150,
    GC9A01A_INREGEN2, 0,
    0xEB, 1, 0x14,
    GC9A01A_INREGEN1, 0,
    GC9A01A_INREGEN2, 0,
    0xEB, 1, 0x14,
    0x84, 1, 0x40,
    0x85, 1, 0xFF,
    0x86, 1, 0xFF,
    0x87, 1, 0xFF,
    0x88, 1, 0x0A,
    0x89, 1, 0x21,
    0x8A, 1, 0x00,
    0x8B, 1, 0x80,
    0x8C, 1, 0x01,
    0x8D, 1, 0x01,
    0x8E, 1, 0xFF,
    0x8F, 1, 0xFF,
    0xB6, 2, 0x00, 0x00,
    GC9A01A_MADCTL, 1, MADCTL_MX | MADCTL_BGR,
    GC9A01A_COLMOD, 1, 0x05,
    0x90, 4, 0x08, 0x08, 0x08, 0x08,
    0xBD, 1, 0x06,
    0xBC, 1, 0x00,
    0xFF, 3, 0x60, 0x01, 0x04,
    GC9A01A1_POWER2, 1, 0x13,
    GC9A01A1_POWER3, 1, 0x13,
    GC9A01A1_POWER4, 1, 0x22,
    0xBE, 1, 0x11,
    0xE1, 2, 0x10, 0x0E,
    0xDF, 3, 0x21, 0x0c, 0x02,
    GC9A01A_GAMMA1, 6, 0x45, 0x09, 0x08, 0x08, 0x26, 0x2A,
    GC9A01A_GAMMA2, 6, 0x43, 0x70, 0x72, 0x36, 0x37, 0x6F,
    GC9A01A_GAMMA3, 6, 0x45, 0x09, 0x08, 0x08, 0x26, 0x2A,
    GC9A01A_GAMMA4, 6, 0x43, 0x70, 0x72, 0x36, 0x37, 0x6F,
    0xED, 2, 0x1B, 0x0B,
    0xAE, 1, 0x77,
    0xCD, 1, 0x63,
    // Unsure what this line (from manufacturer's boilerplate code) is
    // meant to do, but users reported issues, seems to work OK without:
    // 0x70, 9, 0x07, 0x07, 0x04, 0x0E, 0x0F, 0x09, 0x07, 0x08, 0x03, // ?
    GC9A01A_FRAMERATE, 1, 0x34,
    0x62, 12, 0x18, 0x0D, 0x71, 0xED, 0x70, 0x70, 0x18, 0x0F, 0x71, 0xEF, 0x70, 0x70,
    0x63, 12, 0x18, 0x11, 0x71, 0xF1, 0x70, 0x70, 0x18, 0x13, 0x71, 0xF3, 0x70, 0x70,
    0x64, 7, 0x28, 0x29, 0xF1, 0x01, 0xF1, 0x00, 0x07,
    0x66, 10, 0x3C, 0x00, 0xCD, 0x67, 0x45, 0x45, 0x10, 0x00, 0x00, 0x00,
    0x67, 10, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x01, 0x54, 0x10, 0x32, 0x98,
    0x74, 7, 0x10, 0x85, 0x80, 0x00, 0x00, 0x4E, 0x00,
    0x98, 2, 0x3e, 0x07,
    GC9A01A_TEON, 0,
    GC9A01A_INVON, 0,
    GC9A01A_SLPOUT, DELAY, 10, // Exit sleep
    GC9A01A_DISPON, DELAY, 150, // Display on
};

static void display_init(void) {
    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    busio_spi_obj_t *spi = &bus->inline_bus;
    common_hal_busio_spi_construct(
        spi,
        &pin_GPIO6,    // CLK
        &pin_GPIO7,    // MOSI
        NULL,           // MISO not connected
        false           // Not half-duplex
        );
    common_hal_busio_spi_never_reset(spi);
    bus->base.type = &fourwire_fourwire_type;
    common_hal_fourwire_fourwire_construct(
        bus,
        spi,
        &pin_GPIO2,    // DC
        &pin_GPIO10,     // CS
        NULL,           // RST
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
        240,            // height (after rotation)
        0,              // column start
        0,             // row start
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
        &pin_GPIO3,    // backlight pin
        NO_BRIGHTNESS_COMMAND,
        1.0f,           // brightness
        false,          // single_byte_bounds
        false,          // data_as_commands
        true,           // auto_refresh
        60,             // native_frames_per_second
        true,           // backlight_on_high
        false,          // SH1107_addressing
        5000           // backlight pwm frequency
        );
}

void board_init(void) {
    // Display
    display_init();
}
// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
