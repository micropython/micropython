// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/keypad/Event.h"
#include "shared-bindings/keypad/EventQueue.h"
#include "shared-bindings/supervisor/__init__.h"
#include "shared-module/keypad/EventQueue.h"

// Key number is lower 15 bits of a 16-bit value.
#define EVENT_PRESSED (1 << 15)
#define EVENT_KEY_NUM_MASK ((1 << 15) - 1)

void common_hal_keypad_eventqueue_construct(keypad_eventqueue_obj_t *self, size_t max_events) {
    // Event queue is 16-bit values.
    ringbuf_alloc(&self->encoded_events, max_events * (sizeof(uint16_t) + sizeof(mp_obj_t)));
    self->overflowed = false;
    self->event_handler = NULL;
}

bool common_hal_keypad_eventqueue_get_into(keypad_eventqueue_obj_t *self, keypad_event_obj_t *event) {
    int encoded_event = ringbuf_get16(&self->encoded_events);
    if (encoded_event == -1) {
        return false;
    }

    mp_obj_t ticks;
    ringbuf_get_n(&self->encoded_events, (uint8_t *)&ticks, sizeof(ticks));
    // "Construct" using the existing event.
    common_hal_keypad_event_construct(event, encoded_event & EVENT_KEY_NUM_MASK, encoded_event & EVENT_PRESSED, ticks);
    return true;
}

mp_obj_t common_hal_keypad_eventqueue_get(keypad_eventqueue_obj_t *self) {
    keypad_event_obj_t *event = mp_obj_malloc(keypad_event_obj_t, &keypad_event_type);
    bool result = common_hal_keypad_eventqueue_get_into(self, event);
    if (result) {
        return event;
    }
    m_free(event);
    return MP_ROM_NONE;
}

bool common_hal_keypad_eventqueue_get_overflowed(keypad_eventqueue_obj_t *self) {
    return self->overflowed;
}

void common_hal_keypad_eventqueue_set_overflowed(keypad_eventqueue_obj_t *self, bool overflowed) {
    self->overflowed = overflowed;
}

void common_hal_keypad_eventqueue_clear(keypad_eventqueue_obj_t *self) {
    ringbuf_clear(&self->encoded_events);
    common_hal_keypad_eventqueue_set_overflowed(self, false);
}

size_t common_hal_keypad_eventqueue_get_length(keypad_eventqueue_obj_t *self) {
    return ringbuf_num_filled(&self->encoded_events);
}

void common_hal_keypad_eventqueue_set_event_handler(keypad_eventqueue_obj_t *self, void (*event_handler)(keypad_eventqueue_obj_t *)) {
    self->event_handler = event_handler;
}

bool keypad_eventqueue_record(keypad_eventqueue_obj_t *self, mp_uint_t key_number, bool pressed, mp_obj_t timestamp) {
    if (ringbuf_num_empty(&self->encoded_events) == 0) {
        // Queue is full. Set the overflow flag. The caller will decide what else to do.
        common_hal_keypad_eventqueue_set_overflowed(self, true);
        return false;
    }

    uint16_t encoded_event = key_number & EVENT_KEY_NUM_MASK;
    if (pressed) {
        encoded_event |= EVENT_PRESSED;
    }
    ringbuf_put16(&self->encoded_events, encoded_event);
    ringbuf_put_n(&self->encoded_events, (uint8_t *)&timestamp, sizeof(mp_obj_t));

    if (self->event_handler) {
        self->event_handler(self);
    }

    return true;
}
