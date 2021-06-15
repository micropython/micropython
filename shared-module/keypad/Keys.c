/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Dan Halbert for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/gc.h"
#include "py/runtime.h"
#include "shared-bindings/keypad/Event.h"
#include "shared-bindings/keypad/Keys.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "supervisor/port.h"
#include "supervisor/shared/lock.h"
#include "supervisor/shared/tick.h"

static supervisor_lock_t keypad_keys_linked_list_lock;

#define DEBOUNCE_TICKS (20)

// Top bit of 16-bit event indicates pressed or released. Rest is key_num.
#define EVENT_PRESSED (1 << 15)
#define EVENT_RELEASED (0)
#define EVENT_KEY_NUM_MASK (~EVENT_PRESSED)

void common_hal_keypad_keys_construct(keypad_keys_obj_t *self, mp_uint_t num_pins, mcu_pin_obj_t *pins[], bool value_when_pressed, bool pull, size_t max_events) {
    mp_obj_t dios[num_pins];

    for (size_t i = 0; i < num_pins; i++) {
        digitalio_digitalinout_obj_t *dio = m_new_obj(digitalio_digitalinout_obj_t);
        dio->base.type = &digitalio_digitalinout_type;
        common_hal_digitalio_digitalinout_construct(dio, pins[i]);
        if (pull) {
            common_hal_digitalio_digitalinout_set_pull(dio, value_when_pressed ? PULL_DOWN : PULL_UP);
        }
        dios[i] = dio;
    }

    self->digitalinouts = mp_obj_new_tuple(num_pins, dios);
    self->currently_pressed = (bool *)gc_alloc(sizeof(bool) * num_pins, false, false);
    self->previously_pressed = (bool *)gc_alloc(sizeof(bool) * num_pins, false, false);
    self->value_when_pressed = value_when_pressed;

    // Event queue is 16-bit values.
    ringbuf_alloc(self->encoded_events, max_events * 2, false);

    // Add self to the list of active Keys objects.

    supervisor_acquire_lock(&keypad_keys_linked_list_lock);
    self->next = MP_STATE_VM(keypad_keys_linked_list);
    MP_STATE_VM(keypad_keys_linked_list) = self;
    supervisor_release_lock(&keypad_keys_linked_list_lock);
}

void common_hal_keypad_keys_deinit(keypad_keys_obj_t *self) {
    if (common_hal_keypad_keys_deinited(self)) {
        return;
    }

    for (size_t key = 0; key < common_hal_keypad_keys_num_keys(self); key++) {
        common_hal_digitalio_digitalinout_deinit(self->digitalinouts->items[key]);
    }
    self->digitalinouts = MP_ROM_NONE;

    // Remove self from the list of active Keys objects.

    supervisor_acquire_lock(&keypad_keys_linked_list_lock);
    if (MP_STATE_VM(keypad_keys_linked_list) == self) {
        // I'm at the front; splice myself out.
        MP_STATE_VM(keypad_keys_linked_list) = self->next;
    } else {
        keypad_keys_obj_t *current = MP_STATE_VM(keypad_keys_linked_list);
        while (current) {
            if (current->next == self) {
                // Splice myself out.
                current->next = self->next;
                break;
            }
            current = current->next;
        }
    }
    supervisor_release_lock(&keypad_keys_linked_list_lock);
}

bool common_hal_keypad_keys_deinited(keypad_keys_obj_t *self) {
    return self->digitalinouts == MP_ROM_NONE;
}

size_t common_hal_keypad_keys_num_keys(keypad_keys_obj_t *self) {
    return self->digitalinouts->len;
}
bool common_hal_keypad_keys_pressed(keypad_keys_obj_t *self, mp_uint_t key_num) {
    return self->currently_pressed[key_num];
}

mp_obj_t common_hal_keypad_keys_next_event(keypad_keys_obj_t *self) {
    int encoded_event = ringbuf_get16(self->encoded_events);
    if (encoded_event == -1) {
        return MP_ROM_NONE;
    }

    keypad_event_obj_t *event = m_new_obj(keypad_event_obj_t);
    self->base.type = &keypad_event_type;
    common_hal_keypad_event_construct(event, encoded_event & EVENT_KEY_NUM_MASK, encoded_event & EVENT_PRESSED);
    return MP_OBJ_FROM_PTR(event);
}

void common_hal_keypad_keys_clear_events(keypad_keys_obj_t *self) {
    ringbuf_clear(self->encoded_events);
}

static void keypad_keys_scan(keypad_keys_obj_t *self) {
    uint64_t now = port_get_raw_ticks(NULL);
    if (now - self->last_scan_ticks < DEBOUNCE_TICKS) {
        // Too soon. Wait longer to debounce.
        return;
    }

    self->last_scan_ticks = now;

    for (mp_uint_t key_num = 0; key_num < common_hal_keypad_keys_num_keys(self); key_num++) {
        // Remember the previous up/down state.
        const bool previous = self->currently_pressed[key_num];
        self->previously_pressed[key_num] = previous;

        // Get the current state.
        const bool current = common_hal_digitalio_digitalinout_get_value(self->digitalinouts->items[key_num]) ==
            self->value_when_pressed;
        self->currently_pressed[key_num] = current;

        // Record any transitions.
        if (previous != current) {
            if (ringbuf_num_empty(self->encoded_events) == 0) {
                // Discard oldest if full.
                ringbuf_get16(self->encoded_events);
            }
            ringbuf_put16(self->encoded_events, key_num | (current ? EVENT_PRESSED : EVENT_RELEASED));
        }
    }
}

void keypad_keys_tick(void) {
    // Fast path.
    if (!MP_STATE_VM(keypad_keys_linked_list)) {
        return;
    }

    if (supervisor_try_lock(&keypad_keys_linked_list_lock)) {
        keypad_keys_obj_t *keypad_keys = MP_STATE_VM(keypad_keys_linked_list);
        while (keypad_keys) {
            keypad_keys_scan(keypad_keys);
            keypad_keys = keypad_keys->next;
        }
        supervisor_release_lock(&keypad_keys_linked_list_lock);
    }
}

void keypad_keys_reset(void) {
    if (MP_STATE_VM(keypad_keys_linked_list)) {
        supervisor_disable_tick();
    }

    MP_STATE_VM(keypad_keys_linked_list) = NULL;
    keypad_keys_linked_list_lock = false;
}
