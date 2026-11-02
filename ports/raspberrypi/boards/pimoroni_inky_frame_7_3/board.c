// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"

#include "mpconfigboard.h"
#include "shared-bindings/board/__init__.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"
#include "supervisor/shared/board.h"
#include "inky-shared.h"

digitalio_digitalinout_obj_t enable_pin_obj;
static volatile uint32_t reset_button_state = 0;

// This function runs BEFORE main() via constructor attribute!
// This is the key to early and fast SR state readout.
// Priority 101 = runs very early
// Forward declaration to satisfy -Wmissing-prototypes
static void pre_init(void) __attribute__((constructor(101)));

#define SR_PIN_MASK ((0b1 << 8) | (0b1 << 9) | (0b1 << 10))
#define SR_PIN_DIR ((0b1 << 8) | (0b1 << 9))
#define SR_PIN_VALUE ((0b1 << 9))
#define SR_CLK (8)
#define SR_DATA (10)
#define ENABLE_3V3_PIN (2)
#define LED_CONN (7)

// Early init:
//   - latch power
//   - save state of shift-register
//
// Buttons A-E physically turn on the device, so this is *not* wake
// from deep-sleep. They are wired behind a shift register.
// Save the state of the SR early so we know which button
// was pressed to turn on the device.

static void pre_init(void) {
    // Immediately latch power on
    enable_pin_obj.base.type = &digitalio_digitalinout_type;
    common_hal_digitalio_digitalinout_construct(&enable_pin_obj, &pin_GPIO2);
    common_hal_digitalio_digitalinout_switch_to_output(&enable_pin_obj,
        true, DRIVE_MODE_PUSH_PULL);
    // save state of shift-register
    gpio_init_mask(SR_PIN_MASK);
    gpio_set_dir_masked(SR_PIN_MASK, SR_PIN_DIR);
    gpio_put_masked(SR_PIN_MASK, SR_PIN_VALUE);
    uint8_t state = 0u;
    for (uint8_t i = 0u; i < 8; i++) {
        gpio_put(SR_CLK, true);
        if (gpio_get(SR_DATA)) {
            state |= (0b1 << i);
        }
        gpio_put(SR_CLK, false);
    }
    reset_button_state = state;

    // Turn on LED_CONN so the user knows it is ok to release the button.
    // The LED is only on for a short time, since it is automatically turned
    // off during board-init.
    gpio_init(LED_CONN);
    gpio_put(LED_CONN, true);
    gpio_set_dir(LED_CONN, GPIO_OUT);
}

static mp_obj_t _get_reset_state(void) {
    return mp_obj_new_int(reset_button_state);
}
MP_DEFINE_CONST_FUN_OBJ_0(get_reset_state_obj, _get_reset_state);

static mp_obj_t _on_reset_pressed(mp_obj_t in) {
    uint8_t bit_number = mp_obj_get_int(in);
    return mp_obj_new_bool(
        (reset_button_state & (1 << bit_number)) != 0);
}
MP_DEFINE_CONST_FUN_OBJ_1(on_reset_pressed_obj, _on_reset_pressed);

static mp_obj_t _power_off(void) {
    gpio_put(ENABLE_3V3_PIN, false);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(power_off_obj, _power_off);

#define DELAY 0x80

// This is a 7.3" ACeP EInk.

enum reg {
    PSR = 0x00,
    PWR = 0x01,
    POF = 0x02,
    PFS = 0x03,
    PON = 0x04,
    BTST1 = 0x05,
    BTST2 = 0x06,
    DSLP = 0x07,
    BTST3 = 0x08,
    DTM1 = 0x10,
    DSP = 0x11,
    DRF = 0x12,
    IPC = 0x13,
    PLL = 0x30,
    TSC = 0x40,
    TSE = 0x41,
    TSW = 0x42,
    TSR = 0x43,
    CDI = 0x50,
    LPD = 0x51,
    TCON = 0x60,
    TRES = 0x61,
    DAM = 0x65,
    REV = 0x70,
    FLG = 0x71,
    AMV = 0x80,
    VV = 0x81,
    VDCS = 0x82,
    T_VDCS = 0x84,
    AGID = 0x86,
    CMDH = 0xAA,
    CCSET =0xE0,
    PWS = 0xE3,
    TSSET = 0xE6 // E5 or E6
};

const uint8_t display_start_sequence[] = {
    CMDH, 6, 0x49, 0x55, 0x20, 0x08, 0x09, 0x18,
    PWR, 6, 0x3F, 0x00, 0x32, 0x2A, 0x0E, 0x2A,
    PSR, 2, 0x5F, 0x69,
    PFS, 4, 0x00, 0x54, 0x00, 0x44,
    BTST1, 4, 0x40, 0x1F, 0x1F, 0x2C,
    BTST2, 4, 0x6F, 0x1F, 0x16, 0x25,
    BTST3, 4, 0x6F, 0x1F, 0x1F, 0x22,
    IPC, 2, 0x00, 0x04,
    PLL, 1, 0x02,
    TSE, 1, 0x00,
    CDI, 1, 0x3F,
    TCON, 2, 0x02, 0x00,
    TRES, 4, 0x03, 0x20, 0x01, 0xE0,
    VDCS, 1, 0x1E,
    T_VDCS, 1, 0x00,
    AGID, 1, 0x00,
    PWS, 1, 0x2F,
    CCSET, 1, 0x00,
    TSSET, 1, 0x00,
    PON, DELAY, 0xc8,
};

const uint8_t display_stop_sequence[] = {
    POF, 0,
};

const uint8_t refresh_sequence[] = {
    DRF, 1, 0x00,
};

void board_init(void) {
    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    busio_spi_obj_t *spi = common_hal_board_create_spi(0);

    bus->base.type = &fourwire_fourwire_type;
    common_hal_fourwire_fourwire_construct(bus,
        spi,
        MP_OBJ_FROM_PTR(&pin_GPIO28), // EPD_DC Command or data
        MP_OBJ_FROM_PTR(&pin_GPIO17), // EPD_CS Chip select
        MP_OBJ_FROM_PTR(&pin_GPIO27), // EPD_RST Reset
        1000000, // Baudrate
        0, // Polarity
        0); // Phase

    epaperdisplay_epaperdisplay_obj_t *display = &allocate_display()->epaper_display;
    display->base.type = &epaperdisplay_epaperdisplay_type;

    epaperdisplay_construct_args_t args = EPAPERDISPLAY_CONSTRUCT_ARGS_DEFAULTS;
    args.bus = bus;
    args.start_sequence = display_start_sequence;
    args.start_sequence_len = sizeof(display_start_sequence);
    args.start_up_time = 1.0;
    args.stop_sequence = display_stop_sequence;
    args.stop_sequence_len = sizeof(display_stop_sequence);
    args.width = 800;
    args.height = 480;
    args.ram_width = 800;
    args.ram_height = 480;
    args.rotation = 180;
    args.write_black_ram_command = 0x10;
    args.refresh_sequence = refresh_sequence;
    args.refresh_sequence_len = sizeof(refresh_sequence);
    args.refresh_time = 28.0;
    args.busy_pin = NULL;
    args.seconds_per_frame = 30.0;
    args.acep = true;
    common_hal_epaperdisplay_epaperdisplay_construct(display, &args);
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.

bool board_reset_pin_number(uint8_t pin_number) {
    if (pin_number == ENABLE_3V3_PIN) {
        return true;
    }
    return false;
}
