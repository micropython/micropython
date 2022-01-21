/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2011 Dan Halbert for Adafruit Industries
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

#include "shared-bindings/keypad/__init__.h"
#include "shared-bindings/keypad/Event.h"
#include "shared-bindings/keypad/EventQueue.h"
#include "shared-bindings/keypad/KeyMatrix.h"
#include "shared-bindings/keypad/Keys.h"
#include "shared-bindings/keypad/ShiftRegisterKeys.h"

//| """Support for scanning keys and key matrices
//|
//| The `keypad` module provides native support to scan sets of keys or buttons,
//| connected independently to individual pins,
//| connected to a shift register,
//| or connected in a row-and-column matrix.
//| """
//|

STATIC mp_rom_map_elem_t keypad_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),          MP_OBJ_NEW_QSTR(MP_QSTR_keypad) },
    { MP_ROM_QSTR(MP_QSTR_Event),             MP_OBJ_FROM_PTR(&keypad_event_type) },
    { MP_ROM_QSTR(MP_QSTR_EventQueue),        MP_OBJ_FROM_PTR(&keypad_eventqueue_type) },
    { MP_ROM_QSTR(MP_QSTR_KeyMatrix),         MP_OBJ_FROM_PTR(&keypad_keymatrix_type) },
    { MP_ROM_QSTR(MP_QSTR_Keys),              MP_OBJ_FROM_PTR(&keypad_keys_type) },
    { MP_ROM_QSTR(MP_QSTR_ShiftRegisterKeys), MP_OBJ_FROM_PTR(&keypad_shiftregisterkeys_type) },
};

STATIC MP_DEFINE_CONST_DICT(keypad_module_globals, keypad_module_globals_table);

const mp_obj_module_t keypad_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&keypad_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_keypad, keypad_module, CIRCUITPY_KEYPAD);
