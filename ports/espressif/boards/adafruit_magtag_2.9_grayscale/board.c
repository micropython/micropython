// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"

#include "mpconfigboard.h"
#include "driver/gpio.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-module/displayio/__init__.h"
#include "supervisor/shared/board.h"

#include "esp_log.h"

static const char *TAG = "board";

#define DELAY 0x80

// This is an ILO373 control chip. The display is a 2.9" grayscale EInk.

const uint8_t il0373_display_start_sequence[] = {
    0x01, 5, 0x03, 0x00, 0x2b, 0x2b, 0x13, // power setting
    0x06, 3, 0x17, 0x17, 0x17, // booster soft start
    0x04, DELAY, 200, // power on and wait 200 ms
    0x00, 1, 0x7f, // panel setting
    0x50, 1, 0x97, // CDI setting
    0x30, 1, 0x3c, // PLL set to 50 Hx (M = 7, N = 4)
    0x61, 3, 0x80, 0x01, 0x28, // Resolution
    0x82, DELAY | 1, 0x12, 50, // VCM DC and delay 50ms

    // Look up tables for voltage sequence for pixel transition
    // Common voltage
    0x20, 0x2a,
    0x00, 0x0a, 0x00, 0x00, 0x00, 0x01,
    0x60, 0x14, 0x14, 0x00, 0x00, 0x01,
    0x00, 0x14, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x13, 0x0a, 0x01, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    // White to white
    0x21, 0x2a,
    0x40, 0x0a, 0x00, 0x00, 0x00, 0x01,
    0x90, 0x14, 0x14, 0x00, 0x00, 0x01,
    0x10, 0x14, 0x0a, 0x00, 0x00, 0x01,
    0xa0, 0x13, 0x01, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    // Black to white
    0x22, 0x2a,
    0x40, 0x0a, 0x00, 0x00, 0x00, 0x01,
    0x90, 0x14, 0x14, 0x00, 0x00, 0x01,
    0x00, 0x14, 0x0a, 0x00, 0x00, 0x01,
    0x99, 0x0c, 0x01, 0x03, 0x04, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    // White to black
    0x23, 0x2a,
    0x40, 0x0a, 0x00, 0x00, 0x00, 0x01,
    0x90, 0x14, 0x14, 0x00, 0x00, 0x01,
    0x00, 0x14, 0x0a, 0x00, 0x00, 0x01,
    0x99, 0x0b, 0x04, 0x04, 0x01, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    // Black to black
    0x24, 0x2a,
    0x80, 0x0a, 0x00, 0x00, 0x00, 0x01,
    0x90, 0x14, 0x14, 0x00, 0x00, 0x01,
    0x20, 0x14, 0x0a, 0x00, 0x00, 0x01,
    0x50, 0x13, 0x01, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t il0373_display_stop_sequence[] = {
    0x50, 0x01, 0x17,  // CDI Setting
    0x82, 0x01, 0x00,  // VCM DC to -0.1V
    0x02, 0x00  // Power off
};

const uint8_t il0373_display_refresh_sequence[] = {
    0x12, 0x00
};


// This is an SSD1680 control chip. The display is a 2.9" grayscale EInk.
const uint8_t ssd1680_display_start_sequence[] = {
    0x12, DELAY, 0x00, 0x14, // soft reset and wait 20ms
    0x11, 0x00, 0x01, 0x03, // Ram data entry mode
    0x3c, 0x00, 0x01, 0x03, // border color
    0x2c, 0x00, 0x01, 0x28, // Set vcom voltage
    0x03, 0x00, 0x01, 0x17, // Set gate voltage
    0x04, 0x00, 0x03, 0x41, 0xae, 0x32, // Set source voltage
    0x4e, 0x00, 0x01, 0x01, // ram x count
    0x4f, 0x00, 0x02, 0x00, 0x00, // ram y count
    0x01, 0x00, 0x03, 0x27, 0x01, 0x00, // set display size
    0x32, 0x00, 0x99, // Update waveforms
    0x2a, 0x60, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // VS L0
    0x20, 0x60, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // VS L1
    0x28, 0x60, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // VS L2
    0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // VS L3
    0x00, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // VS L4
    0x00, 0x02, 0x00, 0x05, 0x14, 0x00, 0x00,     // TP, SR, RP of Group0
    0x1E, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x01,     // TP, SR, RP of Group1
    0x00, 0x02, 0x00, 0x05, 0x14, 0x00, 0x00,     // TP, SR, RP of Group2
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // TP, SR, RP of Group3
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // TP, SR, RP of Group4
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // TP, SR, RP of Group5
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // TP, SR, RP of Group6
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // TP, SR, RP of Group7
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // TP, SR, RP of Group8
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // TP, SR, RP of Group9
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // TP, SR, RP of Group10
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // TP, SR, RP of Group11
    0x24, 0x22, 0x22, 0x22, 0x23, 0x32, 0x00, 0x00, 0x00,     // FR, XON
    0x22, 0x00, 0x01, 0xc7 // display update mode
};

// FPC-7519rev.b panels (User ID byte 0x44 or 0xca) need colstart=8 and tuned VCOM + LUT.
// LUT: Good Display reference (GxEPD2_4G / GDEM029T94) with VS rows reversed (L0↔L3, L1↔L2).
// Reason: CircuitPython maps luma 0→L0 and luma 255→L3. On this panel VSH1(0x40) drives WHITE and
// VSL(0x20) drives BLACK, so L0 must carry the black-driving waveform and L3 the white-driving
// waveform — opposite from GxEPD2_4G's Arduino convention (where index 0 = white constant).
// VS=0x48 = VSH1/GND/VSL/GND alternating for DC balance.
// VCOM=0x24 empirically tuned for FPC-7519rev.b contrast. Both 0x44 and 0xca share this sequence.
const uint8_t ssd1680_fpc7519_display_start_sequence[] = {
    0x12, DELAY, 0x00, 0x14, // soft reset and wait 20ms
    0x11, 0x00, 0x01, 0x03, // Ram data entry mode
    0x3c, 0x00, 0x01, 0x03, // border color
    0x2c, 0x00, 0x01, 0x24, // Set vcom voltage (0x24 tuned for FPC-7519rev.b contrast)
    0x03, 0x00, 0x01, 0x17, // Set gate voltage
    0x04, 0x00, 0x03, 0x41, 0xae, 0x32, // Set source voltage
    0x4e, 0x00, 0x01, 0x01, // ram x count
    0x4f, 0x00, 0x02, 0x00, 0x00, // ram y count
    0x01, 0x00, 0x03, 0x27, 0x01, 0x00, // set display size
    0x32, 0x00, 0x99, // Update waveforms (153 bytes follow)
    // VS rows: only L0↔L3 are swapped relative to GxEPD2_4G; L1 and L2 stay in place.
    // Reason: CircuitPython luma maps 0→L0, 64-127→L2, 128-191→L1, 255→L3.
    //   GxEPD2 uses L0=white-driver, L3=black-driver (opposite of CircuitPython for extremes).
    //   GxEPD2 L1=lighter-gray, L2=darker-gray; CircuitPython L1=mid-bright, L2=mid-dark — same ordering.
    // So: swap only L0↔L3 (polarity); L1 and L2 keep their GxEPD2 positions (gradient preserved).
    // 0x48 = VSH1/GND/VSL/GND alternating — better DC balance than 0x60 (VSH1/VSL/GND/GND)
    0x20, 0x48, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // VS L0 (darkest/black) ← GxEPD2 L3
    0x08, 0x48, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // VS L1 (mid-light)     ← GxEPD2 L1 (luma 128-191 → lighter)
    0x02, 0x48, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // VS L2 (mid-dark)      ← GxEPD2 L2 (luma 64-127 → darker)
    0x40, 0x48, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // VS L3 (lightest/white) ← GxEPD2 L0
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // VS L4 VCOM
    // Timing groups — Good Display reference timing
    0x0A, 0x19, 0x00, 0x03, 0x08, 0x00, 0x00,     // Group0: 10+25 / 3+8 frames activation
    0x14, 0x01, 0x00, 0x14, 0x01, 0x00, 0x03,     // Group1: 20+1 / 20+1 frames, RP=3 repeats
    0x0A, 0x03, 0x00, 0x08, 0x19, 0x00, 0x00,     // Group2: 10+3 / 8+25 frames (mirror of G0)
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,     // Group3: 1 frame settle
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // Group4
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // Group5
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // Group6
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // Group7
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // Group8
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // Group9
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // Group10
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // Group11
    0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00,     // XON
    0x22, 0x00, 0x01, 0xc7 // display update mode
};

const uint8_t ssd1680_display_stop_sequence[] = {
    0x10, DELAY, 0x01, 0x01, 0x64
};

const uint8_t ssd1680_display_refresh_sequence[] = {
    0x20, 0x00, 0x00
};


typedef enum {
    DISPLAY_IL0373,
    DISPLAY_SSD1680_COLSTART_0,
    DISPLAY_SSD1680_COLSTART_8, // FPC-7519rev.b (User ID 0x44 or 0xca)
} display_type_t;

static display_type_t detect_display_type(void) {
    // Bitbang 4-wire SPI with a bidirectional data line to read the first byte of register 0x2e,
    // which is the 10-byte USER ID.
    // NOTE: the SSD1680 drives its response back on the MOSI/DATA line (GPIO35) in half-duplex
    // mode, NOT on the separate MISO line (GPIO37). Read with GPIO35 switched to input.
    // On the IL0373 it will return 0xff because it's not a valid register.
    // With SSD1680, we have seen two types:
    // 1. The first batch of displays, labeled "FPC-A005 20.06.15 TRX", which needs colstart=0.
    //    These have 10 bytes of zeros in the User ID.
    // 2. Later panels, labeled "FPC-7519rev.b", which need colstart=8 and a tuned LUT/VCOM.
    //    Two controller variants exist within this panel generation:
    //      User ID [0x44, 0x0, 0x4, 0x0, 0x25, 0x0, 0x1, 0x78, 0x2b, 0xe]
    //      User ID [0xca, 0xfe, 0x0, 0x16, 0x80, 0x0, 0x75, 0x1, 0x0, 0x98]
    //    Both carry the same ribbon label and show the same display characteristics.
    // So let's distinguish just by the first byte.
    digitalio_digitalinout_obj_t data;
    digitalio_digitalinout_obj_t clock;
    digitalio_digitalinout_obj_t chip_select;
    digitalio_digitalinout_obj_t data_command;
    digitalio_digitalinout_obj_t reset;
    data.base.type = &digitalio_digitalinout_type;
    clock.base.type = &digitalio_digitalinout_type;
    chip_select.base.type = &digitalio_digitalinout_type;
    data_command.base.type = &digitalio_digitalinout_type;
    reset.base.type = &digitalio_digitalinout_type;

    common_hal_digitalio_digitalinout_construct(&data, &pin_GPIO35);
    common_hal_digitalio_digitalinout_construct(&clock, &pin_GPIO36);
    common_hal_digitalio_digitalinout_construct(&chip_select, &pin_GPIO8);
    common_hal_digitalio_digitalinout_construct(&data_command, &pin_GPIO7);
    common_hal_digitalio_digitalinout_construct(&reset, &pin_GPIO6);

    // Set CS and DC low
    common_hal_digitalio_digitalinout_switch_to_output(&chip_select, false, DRIVE_MODE_PUSH_PULL);
    common_hal_digitalio_digitalinout_switch_to_output(&data_command, false, DRIVE_MODE_PUSH_PULL);
    common_hal_digitalio_digitalinout_switch_to_output(&data, false, DRIVE_MODE_PUSH_PULL);
    common_hal_digitalio_digitalinout_switch_to_output(&clock, false, DRIVE_MODE_PUSH_PULL);

    // Pulse RESET low to wake SSD1680 from deep sleep (entered via stop_sequence on prior run).
    // SSD1680 ignores all SPI commands while in deep sleep; only a hardware reset exits it.
    common_hal_digitalio_digitalinout_switch_to_output(&reset, false, DRIVE_MODE_PUSH_PULL);
    common_hal_mcu_delay_us(200);
    common_hal_digitalio_digitalinout_set_value(&reset, true);
    common_hal_mcu_delay_us(10000); // 10ms for controller to come out of reset

    uint8_t status_read = 0x2e;  // SSD1680 User ID register. Not a valid register on IL0373.
    for (int i = 0; i < 8; i++) {
        common_hal_digitalio_digitalinout_set_value(&data, (status_read & (1 << (7 - i))) != 0);
        common_hal_digitalio_digitalinout_set_value(&clock, true);
        common_hal_digitalio_digitalinout_set_value(&clock, false);
    }

    // Set DC high for data and switch to input with pull-up in case the SSD1680 doesn't send any
    // data back (as it should.)
    common_hal_digitalio_digitalinout_switch_to_input(&data, PULL_UP);
    common_hal_digitalio_digitalinout_set_value(&data_command, true);
    uint8_t status = 0;
    for (int bit = 0; bit < 8; bit++) {
        status <<= 1;
        if (common_hal_digitalio_digitalinout_get_value(&data)) {
            status |= 1;
        }
        common_hal_digitalio_digitalinout_set_value(&clock, true);
        common_hal_digitalio_digitalinout_set_value(&clock, false);
    }

    // Set CS high
    common_hal_digitalio_digitalinout_set_value(&chip_select, true);

    common_hal_digitalio_digitalinout_deinit(&data);
    common_hal_digitalio_digitalinout_deinit(&clock);
    common_hal_digitalio_digitalinout_deinit(&chip_select);
    common_hal_digitalio_digitalinout_deinit(&data_command);
    common_hal_digitalio_digitalinout_deinit(&reset);

    switch (status) {
        case 0xff:
            return DISPLAY_IL0373;
        case 0x00:
            return DISPLAY_SSD1680_COLSTART_0;
        default:    // unknown SSD1680 variant — assume newer panel needs colstart=8
        case 0x44:
        case 0xca:
            return DISPLAY_SSD1680_COLSTART_8;
    }
}

void board_init(void) {
    display_type_t display_type = detect_display_type();

    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    busio_spi_obj_t *spi = &bus->inline_bus;
    common_hal_busio_spi_construct(spi, &pin_GPIO36, &pin_GPIO35, NULL, false);

    bus->base.type = &fourwire_fourwire_type;
    common_hal_fourwire_fourwire_construct(bus,
        spi,
        MP_OBJ_FROM_PTR(&pin_GPIO7), // EPD_DC Command or data
        MP_OBJ_FROM_PTR(&pin_GPIO8), // EPD_CS Chip select
        MP_OBJ_FROM_PTR(&pin_GPIO6), // EPD_RST Reset
        4000000, // Baudrate
        0, // Polarity
        0); // Phase

    epaperdisplay_epaperdisplay_obj_t *display = &allocate_display()->epaper_display;
    display->base.type = &epaperdisplay_epaperdisplay_type;

    if (display_type == DISPLAY_IL0373) {
        epaperdisplay_construct_args_t args = EPAPERDISPLAY_CONSTRUCT_ARGS_DEFAULTS;
        args.bus = bus;
        args.start_sequence = il0373_display_start_sequence;
        args.start_sequence_len = sizeof(il0373_display_start_sequence);
        args.stop_sequence = il0373_display_stop_sequence;
        args.stop_sequence_len = sizeof(il0373_display_stop_sequence);
        args.width = 296;
        args.height = 128;
        args.ram_width = 160;
        args.ram_height = 296;
        args.rotation = 270;
        args.write_black_ram_command = 0x10;
        args.write_color_ram_command = 0x13;
        args.refresh_sequence = il0373_display_refresh_sequence;
        args.refresh_sequence_len = sizeof(il0373_display_refresh_sequence);
        args.refresh_time = 1.0;
        args.busy_pin = &pin_GPIO5;
        args.seconds_per_frame = 5.0;
        args.grayscale = true;
        common_hal_epaperdisplay_epaperdisplay_construct(display, &args);
    } else {
        epaperdisplay_construct_args_t args = EPAPERDISPLAY_CONSTRUCT_ARGS_DEFAULTS;
        args.bus = bus;
        if (display_type == DISPLAY_SSD1680_COLSTART_8) {
            args.colstart = 8;
            args.start_sequence = ssd1680_fpc7519_display_start_sequence;
            args.start_sequence_len = sizeof(ssd1680_fpc7519_display_start_sequence);
        } else {
            args.start_sequence = ssd1680_display_start_sequence;
            args.start_sequence_len = sizeof(ssd1680_display_start_sequence);
        }
        args.stop_sequence = ssd1680_display_stop_sequence;
        args.stop_sequence_len = sizeof(ssd1680_display_stop_sequence);
        args.width = 296;
        args.height = 128;
        args.ram_width = 250;
        args.ram_height = 296;
        args.rotation = 270;
        args.set_column_window_command = 0x44;
        args.set_row_window_command = 0x45;
        args.set_current_column_command = 0x4e;
        args.set_current_row_command = 0x4f;
        args.write_black_ram_command = 0x24;
        args.write_color_ram_command = 0x26;
        args.refresh_sequence = ssd1680_display_refresh_sequence;
        args.refresh_sequence_len = sizeof(ssd1680_display_refresh_sequence);
        args.refresh_time = 1.0;
        args.busy_pin = &pin_GPIO5;
        args.busy_state = true;
        args.seconds_per_frame = 5.0;
        args.grayscale = true;
        args.two_byte_sequence_length = true;
        args.address_little_endian = true;
        common_hal_epaperdisplay_epaperdisplay_construct(display, &args);
    }
}

bool espressif_board_reset_pin_number(gpio_num_t pin_number) {
    // Pin 16 is speaker enable and it's pulled down on the board. We don't want
    // to pull it high because then we'll compete with the external pull down.
    // So, reset without any pulls internally.
    if (pin_number == 16) {
        gpio_config_t cfg = {
            .pin_bit_mask = BIT64(16),
            .mode = GPIO_MODE_DISABLE,
            // The pin is externally pulled down, so we don't need to pull it.
            .pull_up_en = false,
            .pull_down_en = false,
            .intr_type = GPIO_INTR_DISABLE,
        };
        gpio_config(&cfg);
        return true;
    }
    // Pin 4 is used for voltage monitoring, so don't reset
    if (pin_number == 4) {
        return true;
    }
    return false;
}

void board_deinit(void) {
    epaperdisplay_epaperdisplay_obj_t *display = &displays[0].epaper_display;
    if (display->base.type == &epaperdisplay_epaperdisplay_type) {
        size_t i = 0;
        while (common_hal_epaperdisplay_epaperdisplay_get_busy(display)) {
            RUN_BACKGROUND_TASKS;
            i++;
        }
        ESP_LOGI(TAG, "waited %d iterations for display", i);
    } else {
        ESP_LOGI(TAG, "didn't wait for display");
    }
    common_hal_displayio_release_displays();
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
