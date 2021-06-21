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

#include "lib/utils/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/keypad/Event.h"
#include "shared-bindings/keypad/KeyMatrix.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"

//| class KeyMatrix:
//|     """Manage a 2D matrix of keys with row and column pins."""
//|
//|     def __init__(self, row_pins: Sequence[microcontroller.Pin], col_pins: Sequence[microcontroller.Pin], columns_to_anodes: bool = True, interval: float = 0.020, max_events: int = 64) -> None:
//|         """
//|         Create a `Keys` object that will scan the key matrix attached to the given row and column pins.
//|         There should not be any pull-ups or pull-downs on the matrix.
//|
//|         The keys are numbered sequentially from zero. A key number can be computed
//|         by ``row * len(col_pins) + col``.
//|
//|         An `EventQueue` is created when this object is created and is available in the `events` attribute.
//|
//|         The keys are debounced by waiting about 20 msecs before reporting a transition.
//|
//|         :param Sequence[microcontroller.Pin] row_pins: The pins attached to the rows.
//|         :param Sequence[microcontroller.Pin] col_pins: The pins attached to the colums.
//|         :param bool columns_to_anodes: Default ``True``.
//|           If the matrix uses diodes, the diode anodes are typically connected to the column pins,
//|           and the cathodes should be connected to the row pins. If your diodes are reversed,
//|           set ``columns_to_anodes`` to ``False``.
//|         :param float interval: Scan keys no more often
//|           to allow for debouncing. Given in seconds.
//|         :param int max_events: maximum size of `events` `EventQueue`:
//|           maximum number of key transition events that are saved.
//|           Must be >= 1.
//|           If a new event arrives when the queue is full, the oldest event is discarded.
//|         """
//|         ...

STATIC mp_obj_t keypad_keymatrix_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    keypad_keymatrix_obj_t *self = m_new_obj(keypad_keymatrix_obj_t);
    self->base.type = &keypad_keymatrix_type;
    enum { ARG_row_pins, ARG_col_pins, ARG_columns_to_anodes, ARG_interval, ARG_max_events };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_row_pins, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_col_pins, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_columns_to_anodes, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
        { MP_QSTR_interval, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_max_events, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 64} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t row_pins = args[ARG_row_pins].u_obj;
    // mp_obj_len() will be >= 0.
    const size_t num_row_pins = (size_t)MP_OBJ_SMALL_INT_VALUE(mp_obj_len(row_pins));

    mp_obj_t col_pins = args[ARG_col_pins].u_obj;
    const size_t num_col_pins = (size_t)MP_OBJ_SMALL_INT_VALUE(mp_obj_len(col_pins));

    const mp_float_t interval =
        mp_arg_validate_obj_float_non_negative(args[ARG_interval].u_obj, 0.020f, MP_QSTR_interval);
    const size_t max_events = (size_t)mp_arg_validate_int_min(args[ARG_max_events].u_int, 1, MP_QSTR_max_events);

    mcu_pin_obj_t *row_pins_array[num_row_pins];
    mcu_pin_obj_t *col_pins_array[num_col_pins];

    for (size_t row = 0; row < num_row_pins; row++) {
        mcu_pin_obj_t *pin =
            validate_obj_is_free_pin(mp_obj_subscr(row_pins, MP_OBJ_NEW_SMALL_INT(row), MP_OBJ_SENTINEL));
        row_pins_array[row] = pin;
    }

    for (size_t col = 0; col < num_col_pins; col++) {
        mcu_pin_obj_t *pin =
            validate_obj_is_free_pin(mp_obj_subscr(col_pins, MP_OBJ_NEW_SMALL_INT(col), MP_OBJ_SENTINEL));
        col_pins_array[col] = pin;
    }

    common_hal_keypad_keymatrix_construct(self, num_row_pins, row_pins_array, num_col_pins, col_pins_array, args[ARG_columns_to_anodes].u_bool, interval, max_events);
    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Stop scanning and release the pins."""
//|         ...
//|
STATIC mp_obj_t keypad_keymatrix_deinit(mp_obj_t self_in) {
    keypad_keymatrix_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_keypad_keymatrix_deinit(self);
    return MP_ROM_NONE;
}
MP_DEFINE_CONST_FUN_OBJ_1(keypad_keymatrix_deinit_obj, keypad_keymatrix_deinit);

//|     def __enter__(self) -> KeyMatrix:
//|         """No-op used by Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
STATIC mp_obj_t keypad_keymatrix___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_keypad_keymatrix_deinit(args[0]);
    return MP_ROM_NONE;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(keypad_keymatrix___exit___obj, 4, 4, keypad_keymatrix___exit__);

STATIC void check_for_deinit(keypad_keymatrix_obj_t *self) {
    if (common_hal_keypad_keymatrix_deinited(self)) {
        raise_deinited_error();
    }
}

//|     num_keys: int
//|     """The number of keys that are being scanned. (read-only)
//|     """
//|
STATIC mp_obj_t keypad_keymatrix_get_num_keys(mp_obj_t self_in) {
    keypad_keymatrix_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_keypad_keymatrix_get_num_keys(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(keypad_keymatrix_get_num_keys_obj, keypad_keymatrix_get_num_keys);

const mp_obj_property_t keypad_keymatrix_num_keys_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&keypad_keymatrix_get_num_keys_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

//|     def key_num(self, row: int, col: int) -> int:
//|         """Return the key number for a given row and column.
//|         The key number is calculated by ``row * len(col_pins) + col``.
//|         """
//|         ...
//|
STATIC mp_obj_t keypad_keymatrix_key_num(mp_obj_t self_in, mp_obj_t row_in, mp_obj_t col_in) {
    keypad_keymatrix_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    const mp_uint_t row = (mp_uint_t)mp_arg_validate_int_range(
        mp_obj_get_int(row_in), 0, (mp_int_t)common_hal_keypad_keymatrix_get_num_rows(self), MP_QSTR_row);

    const mp_int_t col = (mp_uint_t)mp_arg_validate_int_range(
        mp_obj_get_int(col_in), 0, (mp_int_t)common_hal_keypad_keymatrix_get_num_cols(self), MP_QSTR_col);

    return MP_OBJ_NEW_SMALL_INT(
        (mp_int_t)common_hal_keypad_keymatrix_key_num(self, row, col));
}
MP_DEFINE_CONST_FUN_OBJ_3(keypad_keymatrix_key_num_obj, keypad_keymatrix_key_num);

//|     def pressed(self, key_num: int) -> None:
//|         """Return ``True`` if the given key is pressed. This is a debounced read
//|         of the key state which bypasses the `events` `EventQueue`.
//|         """
//|         ...
//|
STATIC mp_obj_t keypad_keymatrix_pressed(mp_obj_t self_in, mp_obj_t key_num_in) {
    keypad_keymatrix_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    mp_uint_t key_num = mp_arg_validate_int_range(
        mp_obj_get_int(key_num_in), 0, (mp_int_t)common_hal_keypad_keymatrix_get_num_keys(self), MP_QSTR_key_num);

    return mp_obj_new_bool(common_hal_keypad_keymatrix_pressed(self, (mp_uint_t)key_num));
}
MP_DEFINE_CONST_FUN_OBJ_2(keypad_keymatrix_pressed_obj, keypad_keymatrix_pressed);

//|     def store_states(self, states: _typing.WriteableBuffer) -> None:
//|         """Write the state of all the keys into ``states``.
//|         Write a ``1`` if pressed, and ``0`` if released.
//|         The ``length`` of ``states`` must be `num_keys`.
//|         This is a debounced read of the state of all the keys, and bypasses the `events` `EventQueue`.
//|         The read is done atomically.
//|         """
//|         ...
//|
STATIC mp_obj_t keypad_keymatrix_store_states(mp_obj_t self_in, mp_obj_t pressed) {
    keypad_keymatrix_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(pressed, &bufinfo, MP_BUFFER_WRITE);
    if (bufinfo.typecode != 'b' && bufinfo.typecode != 'B' && bufinfo.typecode != BYTEARRAY_TYPECODE) {
        mp_raise_ValueError_varg(translate("%q must store bytes"), MP_QSTR_states);
    }

    (void)mp_arg_validate_length_with_name(bufinfo.len, common_hal_keypad_keymatrix_get_num_keys(self),
        MP_QSTR_states, MP_QSTR_num_keys);

    common_hal_keypad_keymatrix_store_states(self, (uint8_t *)bufinfo.buf);
    return MP_ROM_NONE;
}
MP_DEFINE_CONST_FUN_OBJ_2(keypad_keymatrix_store_states_obj, keypad_keymatrix_store_states);

//|     events: EventQueue
//|     """The `EventQueue` associated with this `Keys` object. (read-only)
//|     """
//|
STATIC mp_obj_t keypad_keymatrix_get_events(mp_obj_t self_in) {
    keypad_keymatrix_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_keypad_keymatrix_get_events(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(keypad_keymatrix_get_events_obj, keypad_keymatrix_get_events);

const mp_obj_property_t keypad_keymatrix_events_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&keypad_keymatrix_get_events_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

STATIC const mp_rom_map_elem_t keypad_keymatrix_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit),       MP_ROM_PTR(&keypad_keymatrix_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__),    MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__),     MP_ROM_PTR(&keypad_keymatrix___exit___obj) },

    { MP_ROM_QSTR(MP_QSTR_events),       MP_ROM_PTR(&keypad_keymatrix_events_obj) },
    { MP_ROM_QSTR(MP_QSTR_key_num),      MP_ROM_PTR(&keypad_keymatrix_key_num_obj) },
    { MP_ROM_QSTR(MP_QSTR_num_keys),     MP_ROM_PTR(&keypad_keymatrix_num_keys_obj) },
    { MP_ROM_QSTR(MP_QSTR_pressed),      MP_ROM_PTR(&keypad_keymatrix_pressed_obj) },
    { MP_ROM_QSTR(MP_QSTR_store_states), MP_ROM_PTR(&keypad_keymatrix_store_states_obj) },
};

STATIC MP_DEFINE_CONST_DICT(keypad_keymatrix_locals_dict, keypad_keymatrix_locals_dict_table);

const mp_obj_type_t keypad_keymatrix_type = {
    { &mp_type_type },
    .name = MP_QSTR_KeyMatrix,
    .make_new = keypad_keymatrix_make_new,
    .locals_dict = (mp_obj_t)&keypad_keymatrix_locals_dict,
};
