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

#include "py/obj.h"
#include "py/enum.h"

#include "shared-bindings/keypad/State.h"

// Defines enum values like
// const cp_enum_obj_t state_JUST_PRESSED = ...
MAKE_ENUM_VALUE(keypad_state_type, state, JUST_PRESSED, STATE_JUST_PRESSED);
MAKE_ENUM_VALUE(keypad_state_type, state, STILL_PRESSED, STATE_STILL_PRESSED);
MAKE_ENUM_VALUE(keypad_state_type, state, PRESSED, STATE_PRESSED);
MAKE_ENUM_VALUE(keypad_state_type, state, JUST_RELEASED, STATE_JUST_RELEASED);
MAKE_ENUM_VALUE(keypad_state_type, state, STILL_RELEASED, STATE_STILL_RELEASED);
MAKE_ENUM_VALUE(keypad_state_type, state, RELEASED, STATE_RELEASED);

//| class State:
//|     """The state of a key, based on the last call to ``scan()``."""
//|
//|     JUST_PRESSED: State
//|     """The key transitioned from released to pressed."""
//|
//|     STILL_PRESSED: State
//|     """The key was already pressed, and continues to be pressed."""
//|
//|     PRESSED: State
//|     """The key is now pressed. Used to indicate states `JUST_PRESSED` and `STILL_PRESSED` inclusively."""
//|
//|     JUST_RELEASED: State
//|     """The key transitioned from pressed to released."""
//|
//|     STILL_RELEASED: State
//|     """The key was already released, and continues to be released."""
//|
//|     RELEASED: State
//|     """The key is now released. Used to indicate states `JUST_RELEASED` and `STILL_RELEASED` inclusively."""
//|
MAKE_ENUM_MAP(keypad_state) {
    MAKE_ENUM_MAP_ENTRY(state, JUST_PRESSED),
    MAKE_ENUM_MAP_ENTRY(state, STILL_PRESSED),
    MAKE_ENUM_MAP_ENTRY(state, PRESSED),
    MAKE_ENUM_MAP_ENTRY(state, JUST_RELEASED),
    MAKE_ENUM_MAP_ENTRY(state, STILL_RELEASED),
    MAKE_ENUM_MAP_ENTRY(state, RELEASED),
};
STATIC MP_DEFINE_CONST_DICT(keypad_state_locals_dict, keypad_state_locals_table);

MAKE_PRINTER(keypad, keypad_state);

// Defines keypad_state_type.
MAKE_ENUM_TYPE(keypad, State, keypad_state);
