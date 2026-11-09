// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Bob Abeles
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"

#include "mpconfigboard.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"
#include "supervisor/shared/board.h"
#include "supervisor/board.h"
#include "badger2350-shared.h"

#include "hardware/gpio.h"
#include "hardware/structs/iobank0.h"

digitalio_digitalinout_obj_t i2c_power_en_pin_obj;
static volatile uint32_t reset_button_state = 0;

// Forward declaration to satisfy -Wmissing-prototypes
static void preinit_button_state(void) __attribute__((constructor(101)));


// pin definitions
// Button pin definitions for Badger2350
#define SW_A_PIN       7
#define SW_B_PIN       9
#define SW_C_PIN      10
#define SW_DOWN_PIN    6
#define SW_UP_PIN     11

static const uint8_t _sw_pin_nrs[] = {
    SW_A_PIN, SW_B_PIN, SW_C_PIN, SW_DOWN_PIN, SW_UP_PIN
};

// Mask of all front button pins
#define SW_MASK ((1 << SW_A_PIN) | (1 << SW_B_PIN) | (1 << SW_C_PIN) | \
    (1 << SW_DOWN_PIN) | (1 << SW_UP_PIN))

// This function runs BEFORE main() via constructor attribute!
// This is the key to fast button state detection.
// Priority 101 = runs very early

static void preinit_button_state(void) {
    // Configure button pins as inputs with pull-downs using direct register access
    // This is faster than SDK functions and works before full init

    for (size_t i = 0; i < sizeof(_sw_pin_nrs); i++) {
        uint8_t pin_nr = _sw_pin_nrs[i];
        // Set as input
        sio_hw->gpio_oe_clr = 1u << pin_nr;
        // enable pull-ups
        pads_bank0_hw->io[pin_nr] = PADS_BANK0_GPIO0_IE_BITS |
            PADS_BANK0_GPIO0_PUE_BITS;
        // Set GPIO function
        iobank0_hw->io[pin_nr].ctrl = 5;  // SIO function
    }

    // Small delay for pins to settle (just a few cycles)
    for (volatile int i = 0; i < 100; i++) {
        __asm volatile ("nop");
    }

    // Capture button states NOW - before anything else runs
    reset_button_state = ~sio_hw->gpio_in & SW_MASK;
}

static mp_obj_t _get_reset_state(void) {
    return mp_obj_new_int(reset_button_state);
}
MP_DEFINE_CONST_FUN_OBJ_0(get_reset_state_obj, _get_reset_state);

static mp_obj_t _on_reset_pressed(mp_obj_t pin_in) {
    mcu_pin_obj_t *pin = MP_OBJ_TO_PTR(pin_in);
    return mp_obj_new_bool(
        (reset_button_state & (1 << pin->number)) != 0);
}
MP_DEFINE_CONST_FUN_OBJ_1(on_reset_pressed_obj, _on_reset_pressed);

// The display uses an SSD1680 control chip.
uint8_t _start_sequence[] = {
    0x12, 0x80, 0x00, 0x14,             // soft reset and wait 20ms
    0x11, 0x00, 0x01, 0x03,             // Ram data entry mode
    0x3c, 0x00, 0x01, 0x03,             // border color
    0x2c, 0x00, 0x01, 0x20,             // Set vcom voltage (Pimoroni: 0x28)
    0x03, 0x00, 0x01, 0x17,             // Set gate voltage
    0x04, 0x00, 0x03, 0x41, 0xae, 0x32, // Set source voltage
    0x4e, 0x00, 0x01, 0x00,             // ram x count
    0x4f, 0x00, 0x02, 0x00, 0x00,       // ram y count
    0x01, 0x00, 0x03, 0x07, 0x01, 0x00, // set display size
    0x3f, 0x00, 0x01, 0x22,             // EOPT normal (0x22)
    0x32, 0x00, 0x99,                   // Update waveforms

    // LUT-START: offset 48
    // VS rows: only L0-L3 are swapped relative to GxEPD2_4G;
    // L1 and L2 stay in place.
    // Reason: CircuitPython luma maps 0 L0, 64-127 L2, 128-191 L1, 255 L3.
    // GxEPD2 uses L0=white-driver, L3=black-driver
    // (opposite of CircuitPython for extremes).
    // GxEPD2 L1=lighter-gray, L2=darker-gray
    // CircuitPython: L1=mid-bright, L2=mid-dark — same ordering.
    // So: swap only L0-L3 (polarity);
    //     L1 and L2 keep their GxEPD2 positions (gradient preserved).
    //
    // Note: 0x48: VSH1/GND/VSL/GND alternating — better DC balance than
    //       0x60 (VSH1/VSL/GND/GND)

    // 5x12 = 60 bytes
    // VS L0 (darkest/black) == GxEPD2 L3
    0x20, 0x48, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // VS L1 (mid-light) == GxEPD2 L1 (luma 128-191 lighter)
    0x08, 0x48, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // VS L2 (mid-dark) == GxEPD2 L2 (luma 64-127 darker)
    0x02, 0x48, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // VS L3 (lightest/white) == GxEPD2 L0
    0x40, 0x48, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // VS L4 VCOM
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    // 12x7 = 84 (144 total)
    // Timing groups — Good Display reference timing
    // offset 108: Group0: 10+25 / 3+8 frames activation
    0x0A, 0x19, 0x00, 0x03, 0x08, 0x00, 0x00,
    // offset 115: Group1: 20+1 / 20+1 frames, RP=0 repeats
    0x14, 0x01, 0x00, 0x14, 0x01, 0x00, 0x00,
    // offset 122: Group2: 10+3 / 8+25 frames (mirror of G0)
    0x0A, 0x03, 0x00, 0x08, 0x19, 0x00, 0x00,

    // offset 129
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Group3
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Group4
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Group5
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Group6
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Group7
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Group8
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Group9
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Group10
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Group11

    // 12x0.5 = 6, (150 total)
    0x22, 0x22, 0x22, 0x22, 0x22, 0x22,       // FR
    // 3x1 = 3, (153 total)
    0x00, 0x00, 0x00,                         // XON
    // LUT-END: offset 201
    0x22, 0x00, 0x01, 0xc7,              // display update mode
};

const uint8_t _stop_sequence[] = {
    0x10, 0x80, 0x01, 0x01, 0x64               // DSM deep sleep mode 1
};

const uint8_t _refresh_sequence[] = {
    0x20, 0x00, 0x00                           // ADUS
};

// Change update speed.

// Pimoroni sets the repeat-count for Group0-Group2 and uses:
// speed=0 == slow ... 3 == fast and calculates the LUT repeat count as 3-speed

// This implementation only changes the repeat-count in the LUT-timing of Group1.
// Default is SPEED=3.

#define RP_OFFSET 121

static mp_obj_t _set_update_speed(mp_obj_t speed_in) {
    mp_int_t speed = mp_obj_get_int(speed_in);
    uint8_t count = (uint8_t)3 - (uint8_t)(speed & 3);
    _start_sequence[RP_OFFSET] = count;
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(set_update_speed_obj, _set_update_speed);

void board_init(void) {
    // Drive the I2C_POWER_EN pin high
    i2c_power_en_pin_obj.base.type = &digitalio_digitalinout_type;
    common_hal_digitalio_digitalinout_construct(
        &i2c_power_en_pin_obj, &pin_GPIO27);
    common_hal_digitalio_digitalinout_switch_to_output(
        &i2c_power_en_pin_obj, true, DRIVE_MODE_PUSH_PULL);

    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    busio_spi_obj_t *spi = &bus->inline_bus;
    common_hal_busio_spi_construct(spi, &pin_GPIO18, &pin_GPIO19, NULL, false);

    bus->base.type = &fourwire_fourwire_type;
    common_hal_fourwire_fourwire_construct(bus,
        spi,
        MP_OBJ_FROM_PTR(&pin_GPIO20), // EPD_DC Command or data
        MP_OBJ_FROM_PTR(&pin_GPIO17), // EPD_CS Chip select
        MP_OBJ_FROM_PTR(&pin_GPIO21), // EPD_RST Reset
        12000000, // Baudrate
        0, // Polarity
        0); // Phase

    // create and configure display
    epaperdisplay_epaperdisplay_obj_t *display = &allocate_display()->epaper_display;
    display->base.type = &epaperdisplay_epaperdisplay_type;

    epaperdisplay_construct_args_t args = EPAPERDISPLAY_CONSTRUCT_ARGS_DEFAULTS;
    args.bus = bus;
    args.start_sequence = _start_sequence;
    args.start_sequence_len = sizeof(_start_sequence);
    args.stop_sequence = _stop_sequence;
    args.stop_sequence_len = sizeof(_stop_sequence);
    args.width = 264;
    args.height = 176;
    args.ram_width = 250;
    args.ram_height = 296;
    args.rotation = 270;
    args.set_column_window_command = 0x44;
    args.set_row_window_command = 0x45;
    args.set_current_column_command = 0x4e;
    args.set_current_row_command = 0x4f;
    args.write_black_ram_command = 0x24;
    args.write_color_ram_command = 0x26;
    args.refresh_sequence = _refresh_sequence;
    args.refresh_sequence_len = sizeof(_refresh_sequence);
    args.refresh_time = 1.0;
    args.busy_pin = &pin_GPIO16;
    args.busy_state = true;
    args.seconds_per_frame = 3.0;
    args.grayscale = true;
    args.two_byte_sequence_length = true;
    args.address_little_endian = true;
    common_hal_epaperdisplay_epaperdisplay_construct(display, &args);
}

void board_deinit(void) {
    epaperdisplay_epaperdisplay_obj_t *display = &displays[0].epaper_display;
    if (display->base.type == &epaperdisplay_epaperdisplay_type) {
        while (common_hal_epaperdisplay_epaperdisplay_get_busy(display)) {
            RUN_BACKGROUND_TASKS;
        }
    }
    common_hal_displayio_release_displays();
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
