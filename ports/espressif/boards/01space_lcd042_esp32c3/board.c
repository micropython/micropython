// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
// SPDX-FileCopyrightText: Copyright (c) 2021 skieast/Bruce Segal
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"
#include "shared-bindings/i2cdisplaybus/I2CDisplayBus.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"
#include "shared-bindings/busio/I2C.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "supervisor/board.h"
#include "supervisor/shared/board.h"
#include "shared-bindings/board/__init__.h"

uint8_t display_init_sequence[] = { // SSD1306
    0xAE, 0, // DISPLAY_OFF
    0x20, 1, 0x00, // Set memory addressing to horizontal mode.
    0x81, 1, 0xcf, // set contrast control
    0xA1, 0, // Column 127 is segment 0
    0xA6, 0, // Normal display
    0xc8, 0, // Normal display
    0xA8, 1, 0x3f, // Mux ratio is 1/64
    0xd5, 1, 0x80, // Set divide ratio
    0xd9, 1, 0xf1, // Set pre-charge period
    0xda, 1, 0x12, // Set com configuration
    0xdb, 1, 0x40, // Set vcom configuration
    0x8d, 1, 0x14, // Enable charge pump
    0xAF, 0, // DISPLAY_ON
};

void board_init(void) {
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    // What we would do if it wasn't the shared board I2C: (for reference)
    // busio_i2c_obj_t *i2c = &allocate_display_bus()->i2cdisplay_bus.inline_bus;
    // common_hal_busio_i2c_construct(i2c, &pin_GPIO23, &pin_GPIO22, 100000, 0);
    // common_hal_busio_i2c_never_reset(i2c);

    i2cdisplaybus_i2cdisplaybus_obj_t *bus = &allocate_display_bus()->i2cdisplay_bus;
    bus->base.type = &i2cdisplaybus_i2cdisplaybus_type;
    common_hal_i2cdisplaybus_i2cdisplaybus_construct(bus,
        i2c,
        0x3c,
        NULL
        );

    busdisplay_busdisplay_obj_t *display = &allocate_display()->display;
    display->base.type = &busdisplay_busdisplay_type;
    common_hal_busdisplay_busdisplay_construct(display,
        bus,
        72, // Width
        40, // Height
        28, // column start
        28, // row start
        0, // rotation
        1, // Color depth
        true, // grayscale
        false, // pixels in byte share row. Only used with depth < 8
        1, // bytes per cell. Only valid for depths < 8
        false, // reverse_pixels_in_byte. Only valid for depths < 8
        true, // reverse_pixels_in_word
        0x21, // Set column command
        0x22, // Set row command
        44, // Write ram command
        display_init_sequence,
        sizeof(display_init_sequence),
        NULL, // no backlight pin
        0x81, // brightness command
        1.0f, // brightness
        true, // single_byte_bounds
        true, // data as commands
        true, // auto_refresh
        60, // native_frames_per_second
        true, // backlight_on_high
        false, // SH1107_addressing
        0); // backlight pwm frequency
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
