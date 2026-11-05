// This file is part of the CircuitPython project: https://circuitpython.org
// SPDX-FileCopyrightText: Copyright (c) 2026 Moises Trovo
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "common-hal/microcontroller/Pin.h"
#include "py/mphal.h"
#include "driver/gpio.h"

#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"

// esp-bsp bsp/esp-box-3/esp-box-3.c vendor_specific_init[], with SWRESET
// prepended and MADCTL set to 0xC8 for CircuitPython's landscape orientation.
// The BSP's own 0x08 is portrait; it also calls
// esp_lcd_panel_mirror(panel, true, true), which CircuitPython folds into
// MADCTL. If the image is ever mirrored or rotated, 0x36 is the knob.
uint8_t display_init_sequence[] = {
    0x01, 0x80, 0x96, // SWRESET, delay 150ms
    0xC8, 0x03, 0xFF, 0x93, 0x42, // vendor unlock
    0xC0, 0x02, 0x0E, 0x0E, // power control
    0xC5, 0x01, 0xD0, // VCOM
    0xC1, 0x01, 0x02,
    0xB4, 0x01, 0x02,
    0xE0, 0x0F, 0x00, 0x03, 0x08, 0x06, 0x13, 0x09, 0x39, 0x39, 0x48, 0x02, 0x0A, 0x08, 0x17, 0x17, 0x0F, // positive gamma
    0xE1, 0x0F, 0x00, 0x28, 0x29, 0x01, 0x0D, 0x03, 0x3F, 0x33, 0x52, 0x04, 0x0F, 0x0E, 0x37, 0x38, 0x0F, // negative gamma
    0xB1, 0x02, 0x00, 0x1B,
    0x36, 0x01, 0xC8, // MADCTL: landscape, BGR
    0x3A, 0x01, 0x55, // COLMOD: 16bpp
    0xB7, 0x01, 0x06,
    0x11, 0x80, 0x78, // SLPOUT, delay 120ms
    0x29, 0x80, 0x78, // DISPON, delay 120ms
};

void board_init(void) {
    // The panel reset (GPIO48, shared with the touch controller) is ACTIVE
    // HIGH. FourWire assumes active low and leaves the pin high, which holds
    // this panel in reset forever with no error. So assert it here, release
    // it low, and hand FourWire reset = None.
    config_pin_as_output_with_level(GPIO_NUM_48, true);
    mp_hal_delay_ms(10);
    config_pin_as_output_with_level(GPIO_NUM_48, false);
    mp_hal_delay_ms(120);

    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    busio_spi_obj_t *spi = &bus->inline_bus;
    common_hal_busio_spi_construct(spi, &pin_GPIO7, &pin_GPIO6, NULL, false);

    bus->base.type = &fourwire_fourwire_type;
    common_hal_fourwire_fourwire_construct(bus,
        spi,
        MP_OBJ_FROM_PTR(&pin_GPIO4), // LCD_DC
        MP_OBJ_FROM_PTR(&pin_GPIO5), // LCD_CS
        mp_const_none, // reset managed above, opposite polarity
        40000000, // Baudrate. BSP uses 40MHz; 24MHz is the fallback.
        0, // Polarity
        0); // Phase

    busdisplay_busdisplay_obj_t *display = &allocate_display()->display;
    display->base.type = &busdisplay_busdisplay_type;
    common_hal_busdisplay_busdisplay_construct(display,
        bus,
        320, // Width
        240, // Height
        0, // column start
        0, // row start
        0, // rotation
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
        &pin_GPIO47, // backlight pin
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

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
