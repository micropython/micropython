/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft
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

#include "shared-bindings/board/__init__.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "py/nlr.h"
#include "py/obj.h"

//| .. currentmodule:: microcontroller
//|
//| :class:`Pin` --- Pin reference
//| ------------------------------------------
//|
//| Identifies an IO pin on the microcontroller.
//|
//| .. class:: Pin
//|
//|    Identifies an IO pin on the microcontroller. They are fixed by the
//|    hardware so they cannot be constructed on demand. Instead, use
//|    `board` or `microcontroller.pin` to reference the desired pin.
//|

STATIC void mcu_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mcu_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // If the pin is in board, print its version there.
    const mp_map_t* board_map = &board_module_globals.map;
    for (uint8_t i = 0; i < board_map->alloc; i++) {
        if (board_map->table[i].value == self) {
            mp_printf(print, "%q.%q", MP_QSTR_board,
                      MP_OBJ_QSTR_VALUE(board_map->table[i].key));
            return;
        }
    }
    const mp_map_t* mcu_map = &mcu_pin_globals.map;
    for (uint8_t i = 0; i < mcu_map->alloc; i++) {
        if (mcu_map->table[i].value == self) {
            mp_printf(print, "%q.%q.%q", MP_QSTR_microcontroller, MP_QSTR_pin,
                      MP_OBJ_QSTR_VALUE(mcu_map->table[i].key));
            return;
        }
    }
}

const mp_obj_type_t mcu_pin_type = {
    { &mp_type_type },
    .name = MP_QSTR_Pin,
    .print = mcu_pin_print
};

void assert_pin(mp_obj_t obj, bool none_ok) {
    if ((obj != mp_const_none || !none_ok) && !MP_OBJ_IS_TYPE(obj, &mcu_pin_type)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "Expected a %q", mcu_pin_type.name));
    }
}

void assert_pin_free(const mcu_pin_obj_t* pin) {
    if (pin != NULL && pin != MP_OBJ_TO_PTR(mp_const_none) && !common_hal_mcu_pin_is_free(pin)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Pin %q in use", pin->name));
    }
}
