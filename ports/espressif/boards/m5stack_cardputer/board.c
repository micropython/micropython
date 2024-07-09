// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "keymap.h"
#include "supervisor/board.h"
#include "supervisor/shared/serial.h"
#include "mpconfigboard.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"
#include "shared-bindings/board/__init__.h"
#include "shared-bindings/keypad_demux/DemuxKeyMatrix.h"
#include "shared-bindings/keypad/EventQueue.h"
#include "shared-bindings/keypad/Event.h"
#include "supervisor/shared/reload.h"
#include "py/runtime.h"
#include "py/ringbuf.h"
#include "shared/runtime/interrupt_char.h"

keypad_demux_demuxkeymatrix_obj_t board_keyboard;

void update_keyboard(keypad_eventqueue_obj_t *queue);
void keyboard_seq(const char *seq);

const mcu_pin_obj_t *row_addr_pins[] = {
    &pin_GPIO8,
    &pin_GPIO9,
    &pin_GPIO11,
};

const mcu_pin_obj_t *column_pins[] = {
    &pin_GPIO13,
    &pin_GPIO15,
    &pin_GPIO3,
    &pin_GPIO4,
    &pin_GPIO5,
    &pin_GPIO6,
    &pin_GPIO7
};

keypad_event_obj_t event;

char keystate[56];
ringbuf_t keyqueue;
char keybuf[32];

#define DELAY 0x80

uint8_t display_init_sequence[] = {
    // SWRESET and Delay 140ms
    0x01, 0 | DELAY, 140,
    // SLPOUT and Delay 10ms
    0x11, 0 | DELAY, 10,
    // COLMOD 65k colors and 16 bit 5-6-5
    0x3A, 1, 0x55,
    // INVON Iiversion on
    0x21, 0,
    // NORON normal operation (full update)
    0x13, 0,
    // MADCTL columns RTL, page/column reverse order
    0x36, 1, 0x60,
    // RAMCTRL color word little endian
    0xB0, 2, 0x00, 0xF8,
    // DIPON display on
    0x29, 0,
};


void board_init(void) {
    busio_spi_obj_t *spi = common_hal_board_create_spi(0);
    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    bus->base.type = &fourwire_fourwire_type;

    // see here for inspiration: https://github.com/m5stack/M5GFX/blob/33d7d3135e816a86a008fae8ab3757938cee95d2/src/M5GFX.cpp#L1350
    common_hal_fourwire_fourwire_construct(
        bus,
        spi,
        &pin_GPIO34,    // DC
        &pin_GPIO37,     // CS
        &pin_GPIO33,    // RST
        40000000,       // baudrate
        0,              // polarity
        0               // phase
        );
    busdisplay_busdisplay_obj_t *display = &allocate_display()->display;
    display->base.type = &busdisplay_busdisplay_type;

    common_hal_busdisplay_busdisplay_construct(
        display,
        bus,
        240,            // width (after rotation)
        135,            // height (after rotation)
        40,             // column start
        53,             // row start
        0,              // rotation
        16,             // color depth
        false,          // grayscale
        false,          // pixels in a byte share a row. Only valid for depths < 8
        1,              // bytes per cell. Only valid for depths < 8
        false,          // reverse_pixels_in_byte. Only valid for depths < 8
        false,          // reverse_pixels_in_word
        MIPI_COMMAND_SET_COLUMN_ADDRESS, // set column command
        MIPI_COMMAND_SET_PAGE_ADDRESS,   // set row command
        MIPI_COMMAND_WRITE_MEMORY_START, // write memory command
        display_init_sequence,
        sizeof(display_init_sequence),
        &pin_GPIO38,    // backlight pin
        NO_BRIGHTNESS_COMMAND,
        1.0f,           // brightness
        false,          // single_byte_bounds
        false,          // data_as_commands
        true,           // auto_refresh
        60,             // native_frames_per_second
        true,           // backlight_on_high
        false,          // SH1107_addressing
        350             // backlight pwm frequency
        );
}

void board_serial_init() {
    ringbuf_init(&keyqueue, (uint8_t *)keybuf, sizeof(keybuf));
    common_hal_keypad_demux_demuxkeymatrix_construct(
        &board_keyboard,     // self
        3,     // num_row_addr_pins
        row_addr_pins,     // row_addr_pins
        7,     // num_column_pins
        column_pins,     // column_pins
        0.01f,     // interval
        20,     // max_events
        2     // debounce_threshold
        );
    demuxkeymatrix_never_reset(&board_keyboard);
    common_hal_keypad_eventqueue_set_event_handler(board_keyboard.events, update_keyboard);

}

bool board_serial_connected() {
    return true;
}

uint32_t board_serial_bytes_available() {
    return ringbuf_num_filled(&keyqueue);
}

void keyboard_seq(const char *seq) {
    while (*seq) {
        ringbuf_put(&keyqueue, *seq++);
    }
}

void update_keyboard(keypad_eventqueue_obj_t *queue) {
    uint8_t ascii = 0;

    if (common_hal_keypad_eventqueue_get_length(queue) == 0) {
        return;
    }

    while (common_hal_keypad_eventqueue_get_into(queue, &event)) {
        if (event.pressed) {
            keystate[event.key_number] = 1;

            if (keystate[KEY_CTRL]) {
                if (keystate[KEY_ALT] && keystate[KEY_BACKSPACE]) {
                    reload_initiate(RUN_REASON_REPL_RELOAD);
                }
                ascii = keymap[event.key_number];
                if (ascii >= 'a' && ascii <= 'z') {
                    ascii -= 'a' - 1;
                }

                if (ascii == mp_interrupt_char) {
                    mp_sched_keyboard_interrupt();
                }
            } else if (keystate[KEY_SHIFT]) {
                ascii = keymap_shifted[event.key_number];
            } else if (keystate[KEY_FN] && event.key_number != KEY_FN) {
                switch (event.key_number | FN_MOD) {
                    case KEY_DOWN:
                        keyboard_seq("\e[B");
                        break;
                    case KEY_UP:
                        keyboard_seq("\e[A");
                        break;
                    case KEY_DELETE:
                        keyboard_seq("\e[3~");
                        break;
                    case KEY_LEFT:
                        keyboard_seq("\e[D");
                        break;
                    case KEY_RIGHT:
                        keyboard_seq("\e[C");
                        break;
                    case KEY_ESC:
                        ringbuf_put(&keyqueue, '\e');
                        break;
                }
            } else {
                ascii = keymap[event.key_number];
            }

            if (ascii > 0) {
                if (keystate[KEY_ALT]) {
                    ringbuf_put(&keyqueue, '\e');
                } else if (keystate[KEY_OPT]) {
                    ringbuf_put(&keyqueue, '\x10');
                }
                ringbuf_put(&keyqueue, ascii);
            }

        } else {
            keystate[event.key_number] = 0;
        }
    }
}

char board_serial_read() {
    return ringbuf_get(&keyqueue);
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.

// TODO: Should we turn off the display when asleep, in board_deinit() ?
