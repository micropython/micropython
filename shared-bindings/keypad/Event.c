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
#include "py/objproperty.h"
#include "py/runtime.h"

#include "shared-bindings/keypad/Event.h"

//| class Event:
//|     """A key transition event."""
//|     def __init__(self, key_num: int, pressed: bool) -> None:
//|         """Create a key transition event, which reports a key-pressed or key-released transition.
//|
//|         :param int key_num: the key number
//|         :param bool pressed: ``True`` if the key was pressed; ``False`` if it was released.
//|         """
//|         ...
//|

STATIC mp_obj_t keypad_event_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    keypad_event_obj_t *self = m_new_obj(keypad_event_obj_t);
    self->base.type = &keypad_event_type;
    enum { ARG_key_num, ARG_pressed };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_key_num, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_pressed, MP_ARG_REQUIRED | MP_ARG_BOOL },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mp_int_t key_num = args[ARG_key_num].u_int;
    if (key_num < 0) {
        mp_raise_ValueError_varg(translate("%q must be > 0"), MP_QSTR_key_num);
    }

    common_hal_keypad_event_construct(self, (mp_uint_t)key_num, args[ARG_pressed].u_bool);
    return MP_OBJ_FROM_PTR(self);
}

//|     key_num: int
//|     """The key number."""
//|
STATIC mp_obj_t keypad_event_obj_get_key_num(mp_obj_t self_in) {
    keypad_event_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_keypad_event_get_key_num(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(keypad_event_get_key_num_obj, keypad_event_obj_get_key_num);

const mp_obj_property_t keypad_event_key_num_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&keypad_event_get_key_num_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

//|     pressed: bool
//|     """True if event represents a key down (pressed) transition."""
//|
STATIC mp_obj_t keypad_event_obj_get_pressed(mp_obj_t self_in) {
    keypad_event_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(common_hal_keypad_event_get_pressed(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(keypad_event_get_pressed_obj, keypad_event_obj_get_pressed);

const mp_obj_property_t keypad_event_pressed_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&keypad_event_get_pressed_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

//|     released: bool
//|     """True if event represents a key up (released) transition."""
//|
STATIC mp_obj_t keypad_event_obj_get_released(mp_obj_t self_in) {
    keypad_event_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(common_hal_keypad_event_get_released(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(keypad_event_get_released_obj, keypad_event_obj_get_released);

const mp_obj_property_t keypad_event_released_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&keypad_event_get_released_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

STATIC const mp_rom_map_elem_t keypad_event_locals_dict_table[] = {
    // Properties
    { MP_ROM_QSTR(MP_QSTR_key_num),  MP_ROM_PTR(&keypad_event_key_num_obj) },
    { MP_ROM_QSTR(MP_QSTR_pressed),  MP_ROM_PTR(&keypad_event_pressed_obj) },
    { MP_ROM_QSTR(MP_QSTR_released), MP_ROM_PTR(&keypad_event_released_obj) },
};
STATIC MP_DEFINE_CONST_DICT(keypad_event_locals_dict, keypad_event_locals_dict_table);

const mp_obj_type_t keypad_event_type = {
    { &mp_type_type },
    .name = MP_QSTR_UART,
    .make_new = keypad_event_make_new,
    .locals_dict = (mp_obj_dict_t *)&keypad_event_locals_dict,
};
