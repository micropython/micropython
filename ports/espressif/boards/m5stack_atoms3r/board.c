// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Cade Roux
//
// SPDX-License-Identifier: MIT

#include "py/mphal.h"
#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/busio/I2C.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"
#include "shared-bindings/board/__init__.h"

#define DELAY 0x80

// The panel is an ST7735S (M5Stack's M5GFX, Panel_ST7735S and board_M5AtomS3R), mounted upside down: MADCTL MY|MX|BGR,
// and the 128 x 128 picture sits 2 columns and 3 rows into the controller's memory. (Units made before May 2026 have a
// GC9107 instead, which this sequence doesn't drive.)
uint8_t display_init_sequence[] = {
    0x01, 0 | DELAY, 150, // SWRESET
    0x11, 0 | DELAY, 255, // SLPOUT
    0xB1, 3, 0x01, 0x2C, 0x2D, // FRMCTR1
    0xB2, 3, 0x01, 0x2C, 0x2D, // FRMCTR2
    0xB3, 6, 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D, // FRMCTR3
    0xB4, 1, 0x07, // INVCTR
    0xC0, 3, 0xA2, 0x02, 0x84, // PWCTR1
    0xC1, 1, 0xC5, // PWCTR2
    0xC2, 2, 0x0A, 0x00, // PWCTR3
    0xC3, 2, 0x8A, 0x2A, // PWCTR4
    0xC4, 2, 0x8A, 0xEE, // PWCTR5
    0xC5, 1, 0x0E, // VMCTR1
    0xE0, 16, 0x02, 0x1C, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2D, 0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10, // GMCTRP1
    0xE1, 16, 0x03, 0x1D, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10, // GMCTRN1
    0x3A, 1, 0x05, // COLMOD: 16 bits a pixel
    0x36, 1, 0xC8, // MADCTL: MY | MX | BGR
    0x21, 0, // INVON
    0x13, 0 | DELAY, 10, // NORON
    0x29, 0 | DELAY, 100, // DISPON
};

// The backlight is the white channel of an LP5562 LED driver on the internal bus (GPIO0 SCL, GPIO45 SDA), not a pin
// (M5GFX: Light_M5StackAtomS3R). Enable it, run it on its internal clock with every LED under I2C control, and set the
// white channel's PWM. The bus is released afterwards, so board.I2C() can open it again.
#define LP5562_ADDRESS 0x30
#define LP5562_BRIGHTNESS 0xA0

static void backlight_on(void) {
    busio_i2c_obj_t i2c;
    common_hal_busio_i2c_construct(&i2c, &pin_GPIO0, &pin_GPIO45, 100000, 255);
    const uint8_t writes[][2] = {
        {0x00, 0x40}, // ENABLE: chip enable
        {0x08, 0x01}, // CONFIG: internal clock
        {0x70, 0x00}, // LED_MAP: every LED from its I2C PWM register
        {0x0E, LP5562_BRIGHTNESS}, // W_PWM
    };
    for (size_t i = 0; i < MP_ARRAY_SIZE(writes); i++) {
        common_hal_busio_i2c_write(&i2c, LP5562_ADDRESS, writes[i], 2);
        mp_hal_delay_ms(1);
    }
    common_hal_busio_i2c_deinit(&i2c);
}

void board_init(void) {
    backlight_on();

    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    busio_spi_obj_t *spi = &bus->inline_bus;
    common_hal_busio_spi_construct(spi, &pin_GPIO15, &pin_GPIO21, NULL, false);
    common_hal_busio_spi_never_reset(spi);
    bus->base.type = &fourwire_fourwire_type;

    common_hal_fourwire_fourwire_construct(
        bus,
        spi,
        MP_OBJ_FROM_PTR(&pin_GPIO42),    // DC
        MP_OBJ_FROM_PTR(&pin_GPIO14),    // CS
        MP_OBJ_FROM_PTR(&pin_GPIO48),    // RST
        40000000,       // baudrate
        0,              // polarity
        0               // phase
        );
    busdisplay_busdisplay_obj_t *display = &allocate_display()->display;
    display->base.type = &busdisplay_busdisplay_type;

    common_hal_busdisplay_busdisplay_construct(
        display,
        bus,
        128,            // width (after rotation)
        128,            // height (after rotation)
        2,              // column start
        3,              // row start
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
        NULL,           // backlight pin: the LP5562 (backlight_on above)
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

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
