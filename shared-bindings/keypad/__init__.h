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

#ifndef SHARED_BINDINGS_KEYPAD_H
#define SHARED_BINDINGS_KEYPAD_H

#include "py/obj.h"
#include "py/objproperty.h"
#include "shared-module/keypad/__init__.h"

bool common_hal_keypad_deinited(void *self);
void common_hal_keypad_generic_reset(void *self);
size_t common_hal_keypad_generic_get_key_count(void *self);
mp_obj_t common_hal_keypad_generic_get_events(void *self);

MP_DECLARE_CONST_FUN_OBJ_1(keypad_generic_reset_obj);

extern const mp_obj_property_t keypad_generic_events_obj;
extern const mp_obj_property_t keypad_generic_key_count_obj;

#endif // SHARED_BINDINGS_KEYPAD_H
