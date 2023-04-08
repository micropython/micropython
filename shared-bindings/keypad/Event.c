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
#include "shared-bindings/supervisor/__init__.h"

//| class Event:
//|     """A key transition event."""
//|
//|     def __init__(
//|         self, key_number: int = 0, pressed: bool = True, timestamp: Optional[int] = None
//|     ) -> None:
//|         """Create a key transition event, which reports a key-pressed or key-released transition.
//|
//|         :param int key_number: The key number.
//|         :param bool pressed: ``True`` if the key was pressed; ``False`` if it was released.
//|         :param int timestamp: The time in milliseconds that the keypress occurred in the `supervisor.ticks_ms` time system.  If specified as None, the current value of `supervisor.ticks_ms` is used.
//|         """
//|         ...
STATIC mp_obj_t keypad_event_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    keypad_event_obj_t *self = m_new_obj(keypad_event_obj_t);
    self->base.type = &keypad_event_type;
    enum { ARG_key_number, ARG_pressed, ARG_timestamp };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_key_number, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_pressed, MP_ARG_BOOL, {.u_bool = true} },
        { MP_QSTR_timestamp, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mp_uint_t key_number =
        (mp_uint_t)mp_arg_validate_int_min(args[ARG_key_number].u_int, 0, MP_QSTR_key_number);

    mp_obj_t timestamp = args[ARG_timestamp].u_obj;
    if (timestamp == mp_const_none) {
        timestamp = supervisor_ticks_ms();
    }

    (void)mp_obj_get_int_truncated(timestamp); // ensure that timestamp is an integer
    common_hal_keypad_event_construct(self, key_number, args[ARG_pressed].u_bool, timestamp);
    return MP_OBJ_FROM_PTR(self);
}

//|     key_number: int
//|     """The key number."""
STATIC mp_obj_t keypad_event_get_key_number(mp_obj_t self_in) {
    keypad_event_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_keypad_event_get_key_number(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(keypad_event_get_key_number_obj, keypad_event_get_key_number);

MP_PROPERTY_GETTER(keypad_event_key_number_obj,
    (mp_obj_t)&keypad_event_get_key_number_obj);

//|     pressed: bool
//|     """``True`` if the event represents a key down (pressed) transition.
//|     The opposite of `released`.
//|     """
STATIC mp_obj_t keypad_event_get_pressed(mp_obj_t self_in) {
    keypad_event_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(common_hal_keypad_event_get_pressed(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(keypad_event_get_pressed_obj, keypad_event_get_pressed);

MP_PROPERTY_GETTER(keypad_event_pressed_obj,
    (mp_obj_t)&keypad_event_get_pressed_obj);

//|     released: bool
//|     """``True`` if the event represents a key up (released) transition.
//|     The opposite of `pressed`.
//|     """
STATIC mp_obj_t keypad_event_get_released(mp_obj_t self_in) {
    keypad_event_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(common_hal_keypad_event_get_released(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(keypad_event_get_released_obj, keypad_event_get_released);

MP_PROPERTY_GETTER(keypad_event_released_obj,
    (mp_obj_t)&keypad_event_get_released_obj);

//|     timestamp: int
//|     """The timestamp."""
STATIC mp_obj_t keypad_event_get_timestamp(mp_obj_t self_in) {
    keypad_event_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_keypad_event_get_timestamp(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(keypad_event_get_timestamp_obj, keypad_event_get_timestamp);

MP_PROPERTY_GETTER(keypad_event_timestamp_obj,
    (mp_obj_t)&keypad_event_get_timestamp_obj);


//|     def __eq__(self, other: object) -> bool:
//|         """Two `Event` objects are equal if their `key_number`
//|         and `pressed`/`released` values are equal.
//|         Note that this does not compare the event timestamps.
//|         """
//|         ...
STATIC mp_obj_t keypad_event_binary_op(mp_binary_op_t op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    switch (op) {
        case MP_BINARY_OP_EQUAL:
            if (mp_obj_is_type(rhs_in, &keypad_event_type)) {
                keypad_event_obj_t *lhs = MP_OBJ_TO_PTR(lhs_in);
                keypad_event_obj_t *rhs = MP_OBJ_TO_PTR(rhs_in);
                return mp_obj_new_bool(
                    (common_hal_keypad_event_get_key_number(lhs) ==
                        common_hal_keypad_event_get_key_number(rhs)) &&
                    (common_hal_keypad_event_get_pressed(lhs) ==
                        common_hal_keypad_event_get_pressed(rhs))
                    );
            } else {
                return mp_const_false;
            }

        default:
            return MP_OBJ_NULL; // op not supported
    }
}

//|     def __hash__(self) -> int:
//|         """Returns a hash for the `Event`, so it can be used in dictionaries, etc..
//|
//|         Note that as events with different timestamps compare equal, they also hash to the same value.
//|         """
//|         ...
//|
STATIC mp_obj_t keypad_event_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    keypad_event_obj_t *self = MP_OBJ_TO_PTR(self_in);
    switch (op) {
        case MP_UNARY_OP_HASH: {
            const mp_int_t key_number = common_hal_keypad_event_get_key_number(self);
            const bool pressed = common_hal_keypad_event_get_pressed(self);
            return MP_OBJ_NEW_SMALL_INT((pressed << 15) + key_number);
        }
        default:
            return MP_OBJ_NULL; // op not supported
    }
}

STATIC void keypad_event_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    keypad_event_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<Event: key_number %d %q>",
        common_hal_keypad_event_get_key_number(self),
        common_hal_keypad_event_get_pressed(self) ? MP_QSTR_pressed : MP_QSTR_released);
}

STATIC const mp_rom_map_elem_t keypad_event_locals_dict_table[] = {
    // Properties
    { MP_ROM_QSTR(MP_QSTR_key_number), MP_ROM_PTR(&keypad_event_key_number_obj) },
    { MP_ROM_QSTR(MP_QSTR_pressed),    MP_ROM_PTR(&keypad_event_pressed_obj) },
    { MP_ROM_QSTR(MP_QSTR_released),   MP_ROM_PTR(&keypad_event_released_obj) },
    { MP_ROM_QSTR(MP_QSTR_timestamp),   MP_ROM_PTR(&keypad_event_timestamp_obj) },
};
STATIC MP_DEFINE_CONST_DICT(keypad_event_locals_dict, keypad_event_locals_dict_table);

const mp_obj_type_t keypad_event_type = {
    { &mp_type_type },
    .flags = MP_TYPE_FLAG_EXTENDED,
    .name = MP_QSTR_Event,
    .make_new = keypad_event_make_new,
    .print = keypad_event_print,
    .locals_dict = (mp_obj_dict_t *)&keypad_event_locals_dict,
    MP_TYPE_EXTENDED_FIELDS(
        .unary_op = keypad_event_unary_op,
        .binary_op = keypad_event_binary_op,
        ),
};
