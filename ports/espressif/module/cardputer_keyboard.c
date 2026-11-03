// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/objstr.h"
#include "py/runtime.h"

#include "supervisor/shared/serial.h"
#include "shared-bindings/keypad/EventQueue.h"
#include "shared-bindings/keypad_demux/DemuxKeyMatrix.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/keypad/EventQueue.h"
#include "shared-module/keypad_demux/DemuxKeyMatrix.h"
#include "supervisor/shared/reload.h"

#include "cardputer_keymap.h"

//| """M5Stack Cardputer keyboard integration.
//| """
//|
//| """The KEYBOARD object is an instance of DemuxKeyMatrix, configured with correct pins.
//| The pins cannot be used for any other purposes (even though exposed in the board module).
//| By default all keyboard events are consumed and routed to the standard input - there is
//| not much use of the KEYBOARD object in this configuration - just read the input via sys.stdin.
//|
//| If you need to manually process individual key up / key down events via KEYBOARD.events,
//| call `detach_serial()`.
//| """"
//| KEYBOARD: keypad_demux.DemuxKeymatrix
//|

keypad_demux_demuxkeymatrix_obj_t cardputer_keyboard;
static bool cardputer_keyboard_serial_attached = false;

// Forward declarations.
static void keyboard_seq(const char *seq);
static void update_keyboard(keypad_eventqueue_obj_t *queue);

//| def detach_serial() -> None:
//|     """Stops consuming keyboard events and routing them to sys.stdin."""
//|     ...
//|
static mp_obj_t detach_serial(void) {
    cardputer_keyboard_serial_attached = false;
    common_hal_keypad_eventqueue_set_event_handler(cardputer_keyboard.events, NULL);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(detach_serial_obj, detach_serial);

//| def attach_serial() -> None:
//|     """Starts consuming keyboard events and routing them to sys.stdin."""
//|     ...
//|
static mp_obj_t attach_serial(void) {
    common_hal_keypad_eventqueue_set_event_handler(cardputer_keyboard.events, update_keyboard);
    cardputer_keyboard_serial_attached = true;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(attach_serial_obj, attach_serial);

//| def key_to_char(key: int, shifted: bool) -> str | None:
//|     """Converts a key index to the respective key (with or without shift modifier).
//|     Returns None for functional & modifier keys or whenever not 0 <= key < 56.
//|     """
//|     ...
//|
static mp_obj_t key_to_char(mp_obj_t key_obj, mp_obj_t shifted_obj) {
    mp_int_t key = mp_obj_get_int(key_obj);
    if (key < 0 || key > (mp_int_t)(sizeof keymap / sizeof *keymap) || keymap[key] == 0) {
        return mp_const_none;
    } else if (shifted_obj == mp_const_true) {
        return mp_obj_new_str(&keymap_shifted[key], 1);
    } else {
        return mp_obj_new_str(&keymap[key], 1);
    }
}
static MP_DEFINE_CONST_FUN_OBJ_2(key_to_char_obj, key_to_char);

// Ring buffer of characters consumed from keyboard events (when serial attached)
static ringbuf_t keyqueue;
static char keybuf[32];

keypad_event_obj_t event;
static char keystate[56] = {0};

// Keyboard pins
static const mcu_pin_obj_t *row_addr_pins[] = {
    &pin_GPIO8,
    &pin_GPIO9,
    &pin_GPIO11,
};

static const mcu_pin_obj_t *column_pins[] = {
    &pin_GPIO13,
    &pin_GPIO15,
    &pin_GPIO3,
    &pin_GPIO4,
    &pin_GPIO5,
    &pin_GPIO6,
    &pin_GPIO7
};

static void cardputer_keyboard_init(void) {
    cardputer_keyboard.base.type = &keypad_demux_demuxkeymatrix_type;
    common_hal_keypad_demux_demuxkeymatrix_construct(
        &cardputer_keyboard,          // self
        MP_ARRAY_SIZE(row_addr_pins), // num_row_addr_pins
        row_addr_pins,                // row_addr_pins
        MP_ARRAY_SIZE(column_pins),   // num_column_pins
        column_pins,                  // column_pins
        true,                         // columns_to_anodes
        false,                        // transpose
        0.01f,                        // interval
        20,                           // max_events
        2,                            // debounce_threshold
        false                         // use_gc_allocator
        );

    demuxkeymatrix_never_reset(&cardputer_keyboard);
    ringbuf_init(&keyqueue, (uint8_t *)keybuf, sizeof(keybuf));
    attach_serial();
}

// Overrides the weakly linked function from supervisor/shared/serial.c
void board_serial_init(void) {
    cardputer_keyboard_init();
}

// Overrides the weakly linked function from supervisor/shared/serial.c
bool board_serial_connected(void) {
    return cardputer_keyboard_serial_attached;
}

// Overrides the weakly linked function from supervisor/shared/serial.c
uint32_t board_serial_bytes_available(void) {
    if (cardputer_keyboard_serial_attached) {
        return ringbuf_num_filled(&keyqueue);
    } else {
        return 0;
    }
}

// Overrides the weakly linked function from supervisor/shared/serial.c
char board_serial_read(void) {
    if (cardputer_keyboard_serial_attached) {
        return ringbuf_get(&keyqueue);
    } else {
        return 0;
    }
}

static void keyboard_seq(const char *seq) {
    while (*seq) {
        ringbuf_put(&keyqueue, *seq++);
    }
}

static void update_keyboard(keypad_eventqueue_obj_t *queue) {
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
                switch (event.key_number | FN_MOD)
                {
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

static const mp_rom_map_elem_t cardputer_keyboard_module_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_cardputer_keyboard)},
    {MP_ROM_QSTR(MP_QSTR_KEYBOARD), MP_ROM_PTR(&cardputer_keyboard)},
    {MP_ROM_QSTR(MP_QSTR_attach_serial), MP_ROM_PTR(&attach_serial_obj)},
    {MP_ROM_QSTR(MP_QSTR_detach_serial), MP_ROM_PTR(&detach_serial_obj)},
    {MP_ROM_QSTR(MP_QSTR_key_to_char), MP_ROM_PTR(&key_to_char_obj)},
};
MP_DEFINE_CONST_DICT(cardputer_keyboard_module_globals, cardputer_keyboard_module_globals_table);

const mp_obj_module_t cardputer_keyboard_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&cardputer_keyboard_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_cardputer_keyboard, cardputer_keyboard_module);
