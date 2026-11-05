// This file is part of the CircuitPython project: https://circuitpython.org
// SPDX-FileCopyrightText: Copyright (c) 2026 Przemyslaw Patrick Socha
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/digitalio/DigitalInOut.h"

#include "shared-bindings/qspibus/QSPIBus.h"
#include "shared-bindings/busdisplay/BusDisplay.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"

// RM690B0 AMOLED initialization sequence.
// Format: command byte, length | 0x80 (if delay), data bytes..., [delay ms]
// Based on vendor recommendations, tested with Waveshare 2.41" AMOLED panel.
// Non-const to match upstream common_hal_busdisplay_busdisplay_construct signature.
static uint8_t display_init_sequence[] = {
    // Page select and configuration
    0xFE, 0x01, 0x20,             // Enter user command mode
    0x26, 0x01, 0x0A,             // Bias setting
    0x24, 0x01, 0x80,             // Source output control
    0xFE, 0x01, 0x13,             // Page 13
    0xEB, 0x01, 0x0E,             // Vendor command
    0xFE, 0x01, 0x00,             // Return to page 0
    // Display configuration
    0x3A, 0x01, 0x55,             // COLMOD: 16-bit RGB565
    0xC2, 0x81, 0x00, 0x0A,      // Vendor command + 10ms delay
    0x35, 0x00,                   // Tearing effect line on (no data)
    0x51, 0x81, 0x00, 0x0A,      // Brightness control + 10ms delay
    // Power on
    0x11, 0x80, 0x50,             // Sleep out + 80ms delay
    // Display window (MV=1: CASET→rows, RASET→cols)
    0x2A, 0x04, 0x00, 0x10, 0x01, 0xD1,  // CASET: 16..465 (450px + 16 offset)
    0x2B, 0x04, 0x00, 0x00, 0x02, 0x57,  // RASET: 0..599  (600px)
    // Enable display
    0x29, 0x80, 0x0A,             // Display on + 10ms delay
    // Memory access: MV=1, ML=1 for landscape
    0x36, 0x81, 0x30, 0x0A,      // MADCTL + 10ms delay
    // Brightness
    0x51, 0x01, 0xFF,             // Set brightness to maximum
};

void board_init(void) {
    // 0. Enable display power before any bus/display init.
    digitalio_digitalinout_obj_t power_pin;
    power_pin.base.type = &digitalio_digitalinout_type;
    common_hal_digitalio_digitalinout_construct(&power_pin, CIRCUITPY_LCD_POWER);
    common_hal_digitalio_digitalinout_set_value(&power_pin, true);
    // Allow power rail to settle before reset/init.
    mp_hal_delay_ms(200);

    // 1. Allocate and construct QSPI bus
    qspibus_qspibus_obj_t *bus = &allocate_display_bus_or_raise()->qspi_bus;
    bus->base.type = &qspibus_qspibus_type;

    common_hal_qspibus_qspibus_construct(bus,
        CIRCUITPY_LCD_CLK,    // clock
        CIRCUITPY_LCD_D0,     // data0
        CIRCUITPY_LCD_D1,     // data1
        CIRCUITPY_LCD_D2,     // data2
        CIRCUITPY_LCD_D3,     // data3
        CIRCUITPY_LCD_CS,     // cs
        NULL,                 // dcx (not used, QSPI uses encoded commands)
        CIRCUITPY_LCD_RESET,  // reset
        40000000);            // 40 MHz

    // 2. Allocate and construct BusDisplay with RM690B0 init sequence.
    //    Physical panel: 450 cols × 600 rows.
    //    MADCTL MV=1 swaps row/col → logical 600×450 landscape.
    busdisplay_busdisplay_obj_t *display = &allocate_display_or_raise()->display;
    display->base.type = &busdisplay_busdisplay_type;

    common_hal_busdisplay_busdisplay_construct(display,
        bus,
        600,          // width  (logical, after MV=1 swap)
        450,          // height (logical, after MV=1 swap)
        0,            // colstart
        16,           // rowstart (physical row offset)
        0,            // rotation
        16,           // color_depth (RGB565)
        false,        // grayscale
        false,        // pixels_in_byte_share_row
        1,            // bytes_per_cell
        false,        // reverse_pixels_in_byte
        false,        // reverse_bytes_in_word
        MIPI_COMMAND_SET_COLUMN_ADDRESS,    // set_column_command
        MIPI_COMMAND_SET_PAGE_ADDRESS,      // set_row_command
        MIPI_COMMAND_WRITE_MEMORY_START,    // write_ram_command
        display_init_sequence,
        sizeof(display_init_sequence),
        NULL,         // backlight_pin (AMOLED — no backlight GPIO)
        0x51,         // brightness_command
        1.0f,         // brightness
        false,        // single_byte_bounds
        false,        // data_as_commands
        true,         // auto_refresh
        60,           // native_frames_per_second
        true,         // backlight_on_high
        false,        // SH1107_addressing
        50000);       // backlight_pwm_frequency
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
