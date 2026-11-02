// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"

#include "mpconfigboard.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"
#include "shared-bindings/board/__init__.h"
#include "supervisor/shared/board.h"
#include "inky-shared.h"

#define DELAY 0x80

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


// This is an SPD1656 control chip. The display is a 5.7" ACeP EInk.

const uint8_t display_start_sequence[] = {
    0x01, 4, 0x37, 0x00, 0x23, 0x23, // power setting
    0x00, 2, 0xe3, 0x08, // panel setting (PSR, 0xe3: no rotation)
    0x03, 1, 0x00, // PFS
    0x06, 3, 0xc7, 0xc7, 0x1d, // booster
    0x30, 1, 0x3c, // PLL setting
    0x41, 1, 0x00, // TSE
    0x50, 1, 0x37, // vcom and data interval setting
    0x60, 1, 0x22, // tcon setting
    0x61, 4, 0x02, 0x58, 0x01, 0xc0, // tres
    0xe3, 1, 0xaa, // PWS
    0x04, DELAY | 0, 0xc8, // VCM DC and delay 200ms
};

const uint8_t display_stop_sequence[] = {
    0x02, 1, 0x00, // power off
    0x07, 1, 0xa5 // deep sleep
};

const uint8_t refresh_sequence[] = {
    0x12, 0x00
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
    args.width = 600;
    args.height = 448;
    args.ram_width = 640;
    args.ram_height = 480;
    args.write_black_ram_command = 0x10;
    args.refresh_sequence = refresh_sequence;
    args.refresh_sequence_len = sizeof(refresh_sequence);
    args.refresh_time = 28.0;
    args.busy_pin = NULL;
    args.seconds_per_frame = 40.0;
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
