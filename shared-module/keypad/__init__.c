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

#include "shared-bindings/keypad/Keys.h"
#include "shared-bindings/keypad/KeyMatrix.h"
#include "shared-bindings/keypad/ShiftRegisterKeys.h"
#include "supervisor/shared/lock.h"
#include "supervisor/shared/tick.h"

supervisor_lock_t keypad_scanners_linked_list_lock;

void keypad_tick(void) {
    // Fast path. Return immediately there are no scanners.
    if (!MP_STATE_VM(keypad_scanners_linked_list)) {
        return;
    }

    // Skip scanning if someone else has the lock. Don't wait for the lock.
    if (supervisor_try_lock(&keypad_scanners_linked_list_lock)) {
        mp_obj_t scanner = MP_STATE_VM(keypad_scanners_linked_list);
        while (scanner) {
            if (mp_obj_is_type(scanner, &keypad_keys_type)) {
                keypad_keys_scan((keypad_keys_obj_t *)scanner);
            } else if (mp_obj_is_type(scanner, &keypad_keymatrix_type)) {
                keypad_keymatrix_scan((keypad_keymatrix_obj_t *)scanner);
            } else if (mp_obj_is_type(scanner, &keypad_shiftregisterkeys_type)) {
                keypad_shiftregisterkeys_scan((keypad_shiftregisterkeys_obj_t *)scanner);
            }

            scanner = ((keypad_scanner_obj_t *)scanner)->next;
        }
        supervisor_release_lock(&keypad_scanners_linked_list_lock);
    }
}

void keypad_reset(void) {
    if (MP_STATE_VM(keypad_scanners_linked_list)) {
        supervisor_disable_tick();
    }

    MP_STATE_VM(keypad_scanners_linked_list) = NULL;
    keypad_scanners_linked_list_lock = false;
}

// Register a Keys, KeyMatrix, etc. that will be scanned in the background
void keypad_register_scanner(keypad_scanner_obj_t *scanner) {
    supervisor_acquire_lock(&keypad_scanners_linked_list_lock);
    scanner->next = MP_STATE_VM(keypad_scanners_linked_list);
    MP_STATE_VM(keypad_scanners_linked_list) = scanner;
    supervisor_release_lock(&keypad_scanners_linked_list_lock);
}

// Remove scanner from the list of active scanners.
void keypad_deregister_scanner(keypad_scanner_obj_t *scanner) {
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
