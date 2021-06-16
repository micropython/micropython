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

#include "py/runtime.h"
#include "shared-bindings/keypad/EventQueue.h"

//| class EventQueue:
//|     """A queue of `Event` objects, filled by a `keypad` scanner such as `Keys` or `KeyMatrix`.
//|
//|     You cannot create an instance of `EventQueue` directly. Each scanner creates an
//|     instance when it is created.
//|     """
//|     ...

//|     def next(self) -> Optional[Event]:
//|         """Return the next key transition event. Return ``None`` if no events are pending.
//|
//|         Note that the queue size is limited; see ``max_events`` in the constructor of
//|         a scanner such as `Keys` or `KeyMatrix`.
//|         If a new event arrives when the queue is full, the oldest event is discarded.
//|
//|         :return: the next queued key transition `Event`
//|         :rtype: Optional[Event]
//|         """
//|         ...
//|
STATIC mp_obj_t keypad_eventqueue_next(mp_obj_t self_in) {
    keypad_eventqueue_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return common_hal_keypad_eventqueue_next(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(keypad_eventqueue_next_obj, keypad_eventqueue_next);

//|     def clear(self) -> None:
//|         """Clear any queued key transition events.
//|         """
//|         ...
//|
STATIC mp_obj_t keypad_eventqueue_clear(mp_obj_t self_in) {
    keypad_eventqueue_obj_t *self = MP_OBJ_TO_PTR(self_in);

    common_hal_keypad_eventqueue_clear(self);
    return MP_ROM_NONE;
}
MP_DEFINE_CONST_FUN_OBJ_1(keypad_eventqueue_clear_obj, keypad_eventqueue_clear);

//|     def __bool__(self) -> bool:
//|         """``True`` if `len()` is greater than zero.
//|         This is an easy way to check if the queue is empty.
//|         """
//|         ...
//|
//|     def __len__(self) -> int:
//|         """Return the number of events currently in the queue. Used to implement ``len()``."""
//|         ...
//|
STATIC mp_obj_t keypad_eventqueue_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    keypad_eventqueue_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint16_t len = common_hal_keypad_eventqueue_get_length(self);
    switch (op) {
        case MP_UNARY_OP_BOOL:
            return mp_obj_new_bool(len != 0);
        case MP_UNARY_OP_LEN:
            return MP_OBJ_NEW_SMALL_INT(len);
        default:
            return MP_OBJ_NULL;      // op not supported
    }
}

STATIC const mp_rom_map_elem_t keypad_eventqueue_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&keypad_eventqueue_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_next),  MP_ROM_PTR(&keypad_eventqueue_next_obj) },
};

STATIC MP_DEFINE_CONST_DICT(keypad_eventqueue_locals_dict, keypad_eventqueue_locals_dict_table);

const mp_obj_type_t keypad_eventqueue_type = {
    { &mp_type_type },
    .name = MP_QSTR_Keys,
    .unary_op = keypad_eventqueue_unary_op,
    .locals_dict = (mp_obj_t)&keypad_eventqueue_locals_dict,
};
