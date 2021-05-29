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
#include "py/objproperty.h"
#include "shared-bindings/keypad/Keys.h"
#include "shared-bindings/keypad/State.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "py/runtime.h"

void common_hal_keypad_keys_construct(keypad_keys_obj_t *self, mp_uint_t num_pins, mcu_pin_obj_t *pins[], bool value_when_pressed, bool pull) {
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

}

void common_hal_keypad_keys_keys_with_state(keypad_keys_obj_t *self, int state, mp_obj_list_t *list_into) {
    const size_t list_length = list_into->len;

    size_t next_list_slot = 0;

    for (mp_uint_t key_num = 0; key_num < common_hal_keypad_keys_length(self); key_num++) {
        if (next_list_slot >= list_length) {
            // List is full.
            break;
        }

        bool store_key = false;
        switch (state) {
            case STATE_JUST_PRESSED:
                store_key = !self->previously_pressed[key_num] && self->currently_pressed[key_num];
                break;
            case STATE_STILL_PRESSED:
                store_key = self->previously_pressed[key_num] && self->currently_pressed[key_num];
                break;
            case STATE_PRESSED:
                store_key = self->currently_pressed[key_num];
                break;
            case STATE_JUST_RELEASED:
                store_key = self->previously_pressed[key_num] && !self->currently_pressed[key_num];
                break;
            case STATE_STILL_RELEASED:
                store_key = !self->previously_pressed[key_num] && !self->currently_pressed[key_num];
                break;
            case STATE_RELEASED:
                store_key = !self->currently_pressed[key_num];
                break;
        }

        if (store_key) {
            mp_obj_list_store(list_into, MP_OBJ_NEW_SMALL_INT(next_list_slot),
                MP_OBJ_NEW_SMALL_INT(key_num));
            next_list_slot++;
        }

        for (size_t unused_slot = next_list_slot; unused_slot < list_length; unused_slot++) {
            mp_obj_list_store(list_into, MP_OBJ_NEW_SMALL_INT(unused_slot),
                MP_ROM_NONE);
        }
    }
}

size_t common_hal_keypad_keys_length(keypad_keys_obj_t *self) {
    return self->digitalinouts->len;
}

void common_hal_keypad_keys_scan(keypad_keys_obj_t *self) {
    for (mp_uint_t key_num = 0; key_num < common_hal_keypad_keys_length(self); key_num++) {
        self->previously_pressed[key_num] = self->currently_pressed[key_num];
        self->currently_pressed[key_num] =
            common_hal_digitalio_digitalinout_get_value(self->digitalinouts->items[key_num]) ==
            self->value_when_pressed;
    }
}

mp_int_t common_hal_keypad_keys_state(keypad_keys_obj_t *self, mp_uint_t key_num) {
    if (self->currently_pressed[key_num]) {
        if (self->previously_pressed[key_num]) {
            return STATE_STILL_PRESSED;
        } else {
            return STATE_JUST_PRESSED;
        }
    } else {
        if (self->previously_pressed[key_num]) {
            return STATE_JUST_RELEASED;
        } else {
            return STATE_STILL_RELEASED;
        }
    }
}
