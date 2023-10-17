/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#include <string.h>
#include "shared-bindings/keypad/__init__.h"
#include "shared-bindings/keypad/EventQueue.h"
#include "shared-bindings/keypad/Keys.h"
#include "shared-bindings/keypad/KeyMatrix.h"
#include "shared-bindings/keypad/ShiftRegisterKeys.h"
#include "shared-bindings/supervisor/__init__.h"
#include "supervisor/port.h"
#include "supervisor/shared/lock.h"
#include "supervisor/shared/tick.h"

supervisor_lock_t keypad_scanners_linked_list_lock;
static void keypad_scan_now(keypad_scanner_obj_t *self, uint64_t now);
static void keypad_scan_maybe(keypad_scanner_obj_t *self, uint64_t now);

void keypad_tick(void) {
    // Fast path. Return immediately there are no scanners.
    if (!MP_STATE_VM(keypad_scanners_linked_list)) {
        return;
    }

    // Skip scanning if someone else has the lock. Don't wait for the lock.
    if (supervisor_try_lock(&keypad_scanners_linked_list_lock)) {
        uint64_t now = port_get_raw_ticks(NULL);
        mp_obj_t scanner = MP_STATE_VM(keypad_scanners_linked_list);
        while (scanner) {
            keypad_scan_maybe(scanner, now);
            scanner = ((keypad_scanner_obj_t *)scanner)->next;
        }
        supervisor_release_lock(&keypad_scanners_linked_list_lock);
    }
}

void keypad_reset(void) {
    while (MP_STATE_VM(keypad_scanners_linked_list)) {
        keypad_deregister_scanner(MP_STATE_VM(keypad_scanners_linked_list));
    }
}

// Register a Keys, KeyMatrix, etc. that will be scanned in the background
void keypad_register_scanner(keypad_scanner_obj_t *scanner) {
    supervisor_acquire_lock(&keypad_scanners_linked_list_lock);
    scanner->next = MP_STATE_VM(keypad_scanners_linked_list);
    MP_STATE_VM(keypad_scanners_linked_list) = scanner;
    supervisor_release_lock(&keypad_scanners_linked_list_lock);

    // One more request for ticks.
    supervisor_enable_tick();
}

// Remove scanner from the list of active scanners.
void keypad_deregister_scanner(keypad_scanner_obj_t *scanner) {
    // One less request for ticks.
    supervisor_disable_tick();

    supervisor_acquire_lock(&keypad_scanners_linked_list_lock);
    if (MP_STATE_VM(keypad_scanners_linked_list) == scanner) {
        // Scanner is at the front; splice it out.
        MP_STATE_VM(keypad_scanners_linked_list) = scanner->next;
        scanner->next = NULL;
    } else {
        keypad_scanner_obj_t *current = MP_STATE_VM(keypad_scanners_linked_list);
        while (current) {
            if (current->next == scanner) {
                // Splice myself out.
                current->next = scanner->next;
                scanner->next = NULL;
                break;
            }
            current = current->next;
        }
    }
    supervisor_release_lock(&keypad_scanners_linked_list_lock);
}

void keypad_construct_common(keypad_scanner_obj_t *self, mp_float_t interval, size_t max_events) {
    size_t key_count = common_hal_keypad_generic_get_key_count(self);
    self->currently_pressed = (bool *)m_malloc(sizeof(bool) * key_count);
    self->previously_pressed = (bool *)m_malloc(sizeof(bool) * key_count);

    self->interval_ticks = (mp_uint_t)(interval * 1024);   // interval * 1000 * (1024/1000)

    keypad_eventqueue_obj_t *events = mp_obj_malloc(keypad_eventqueue_obj_t, &keypad_eventqueue_type);
    common_hal_keypad_eventqueue_construct(events, max_events);
    self->events = events;

    // Add self to the list of active keypad scanners.
    keypad_register_scanner(self);
    keypad_scan_now(self, port_get_raw_ticks(NULL));
}

static void keypad_scan_now(keypad_scanner_obj_t *self, uint64_t now) {
    self->next_scan_ticks = now + self->interval_ticks;
    self->funcs->scan_now(self, supervisor_ticks_ms());
}

static void keypad_scan_maybe(keypad_scanner_obj_t *self, uint64_t now) {
    if (now < self->next_scan_ticks) {
        return;
    }
    keypad_scan_now(self, now);
}

void common_hal_keypad_generic_reset(void *self_in) {
    keypad_scanner_obj_t *self = self_in;
    size_t key_count = common_hal_keypad_generic_get_key_count(self);
    memset(self->previously_pressed, false, key_count);
    memset(self->currently_pressed, false, key_count);
    keypad_scan_now(self, port_get_raw_ticks(NULL));
}

void common_hal_keypad_deinit_core(void *self_in) {
    keypad_scanner_obj_t *self = self_in;
    self->events = NULL;
}

bool common_hal_keypad_deinited(void *self_in) {
    keypad_scanner_obj_t *self = self_in;
    return !self->events;
}

size_t common_hal_keypad_generic_get_key_count(void *self_in) {
    keypad_scanner_obj_t *self = self_in;
    return self->funcs->get_key_count(self);
}

mp_obj_t common_hal_keypad_generic_get_events(void *self_in) {
    keypad_scanner_obj_t *self = self_in;
    return self->events;
}

MP_REGISTER_ROOT_POINTER(mp_obj_t keypad_scanners_linked_list);
