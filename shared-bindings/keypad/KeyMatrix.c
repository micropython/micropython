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
//|     def __init__(self, row_pins: Sequence[microcontroller.Pin], column_pins: Sequence[microcontroller.Pin], columns_to_anodes: bool = True, interval: float = 0.020, max_events: int = 64) -> None:
//|         """
//|         Create a `Keys` object that will scan the key matrix attached to the given row and column pins.
//|         There should not be any external pull-ups or pull-downs on the matrix:
//|         ``KeyMatrix`` enables internal pull-ups or pull-downs on the pins as necessary.
//|
//|         The keys are numbered sequentially from zero. A key number can be computed
//|         by ``row * len(column_pins) + column``.
//|
//|         An `EventQueue` is created when this object is created and is available in the `events` attribute.
//|
//|         :param Sequence[microcontroller.Pin] row_pins: The pins attached to the rows.
//|         :param Sequence[microcontroller.Pin] column_pins: The pins attached to the colums.
//|         :param bool columns_to_anodes: Default ``True``.
//|           If the matrix uses diodes, the diode anodes are typically connected to the column pins,
//|           and the cathodes should be connected to the row pins. If your diodes are reversed,
//|           set ``columns_to_anodes`` to ``False``.
//|         :param float interval: Scan keys no more often than ``interval`` to allow for debouncing.
//|           ``interval`` is in float seconds. The default is 0.020 (20 msecs).
//|         :param int max_events: maximum size of `events` `EventQueue`:
//|           maximum number of key transition events that are saved.
//|           Must be >= 1.
//|           If a new event arrives when the queue is full, the oldest event is discarded.
//|         """
//|         ...

STATIC mp_obj_t keypad_keymatrix_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    keypad_keymatrix_obj_t *self = m_new_obj(keypad_keymatrix_obj_t);
    self->base.type = &keypad_keymatrix_type;
    enum { ARG_row_pins, ARG_column_pins, ARG_columns_to_anodes, ARG_interval, ARG_max_events };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_row_pins, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_column_pins, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_columns_to_anodes, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
        { MP_QSTR_interval, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_max_events, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 64} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t row_pins = args[ARG_row_pins].u_obj;
    // mp_obj_len() will be >= 0.
    const size_t num_row_pins = (size_t)MP_OBJ_SMALL_INT_VALUE(mp_obj_len(row_pins));

    mp_obj_t column_pins = args[ARG_column_pins].u_obj;
    const size_t num_column_pins = (size_t)MP_OBJ_SMALL_INT_VALUE(mp_obj_len(column_pins));

    const mp_float_t interval =
        mp_arg_validate_obj_float_non_negative(args[ARG_interval].u_obj, 0.020f, MP_QSTR_interval);
    const size_t max_events = (size_t)mp_arg_validate_int_min(args[ARG_max_events].u_int, 1, MP_QSTR_max_events);

    mcu_pin_obj_t *row_pins_array[num_row_pins];
    mcu_pin_obj_t *column_pins_array[num_column_pins];

    for (size_t row = 0; row < num_row_pins; row++) {
        mcu_pin_obj_t *pin =
            validate_obj_is_free_pin(mp_obj_subscr(row_pins, MP_OBJ_NEW_SMALL_INT(row), MP_OBJ_SENTINEL));
        row_pins_array[row] = pin;
    }

    for (size_t column = 0; column < num_column_pins; column++) {
        mcu_pin_obj_t *pin =
            validate_obj_is_free_pin(mp_obj_subscr(column_pins, MP_OBJ_NEW_SMALL_INT(column), MP_OBJ_SENTINEL));
        column_pins_array[column] = pin;
    }

    common_hal_keypad_keymatrix_construct(self, num_row_pins, row_pins_array, num_column_pins, column_pins_array, args[ARG_columns_to_anodes].u_bool, interval, max_events);
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

//|     def reset(self) -> None:
//|         """Reset the internal state of the scanner to assume that all keys are now released.
//|         Any key that is already pressed at the time of this call will therefore immediately cause
//|         a new key-pressed event to occur.
//|         """
//|         ...
//|
STATIC mp_obj_t keypad_keymatrix_reset(mp_obj_t self_in) {
    keypad_keymatrix_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    common_hal_keypad_keymatrix_reset(self);
    return MP_ROM_NONE;
}
MP_DEFINE_CONST_FUN_OBJ_1(keypad_keymatrix_reset_obj, keypad_keymatrix_reset);

//|     key_count: int
//|     """The number of keys that are being scanned. (read-only)
//|     """
//|
STATIC mp_obj_t keypad_keymatrix_get_key_count(mp_obj_t self_in) {
    keypad_keymatrix_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    return MP_OBJ_NEW_SMALL_INT(common_hal_keypad_keymatrix_get_key_count(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(keypad_keymatrix_get_key_count_obj, keypad_keymatrix_get_key_count);

const mp_obj_property_t keypad_keymatrix_key_count_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&keypad_keymatrix_get_key_count_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

//|     def key_number_to_row_column(self, row: int, column: int) -> Tuple[int]:
//|         """Return the row and column for the given key number.
//|         The row is ``key_number // len(column_pins)``.
//|         The column is ``key_number % len(column_pins)``.
//|
//|         :return: ``(row, column)``
//|         :rtype: Tuple[int]
//|         """
//|         ...
//|
STATIC mp_obj_t keypad_keymatrix_key_number_to_row_column(mp_obj_t self_in, mp_obj_t key_number_in) {
    keypad_keymatrix_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    const mp_uint_t key_number = (mp_uint_t)mp_arg_validate_int_range(
        mp_obj_get_int(key_number_in),
        0, (mp_int_t)common_hal_keypad_keymatrix_get_key_count(self),
        MP_QSTR_key_number);

    mp_uint_t row;
    mp_uint_t column;
    common_hal_keypad_keymatrix_key_number_to_row_column(self, key_number, &row, &column);

    mp_obj_t row_column[2];
    row_column[0] = MP_OBJ_NEW_SMALL_INT(row);
    row_column[1] = MP_OBJ_NEW_SMALL_INT(column);

    return mp_obj_new_tuple(2, row_column);
}
MP_DEFINE_CONST_FUN_OBJ_2(keypad_keymatrix_key_number_to_row_column_obj, keypad_keymatrix_key_number_to_row_column);

//|     def row_column_to_key_number(self, row: int, column: int) -> int:
//|         """Return the key number for a given row and column.
//|         The key number is ``row * len(column_pins) + column``.
//|         """
//|         ...
//|
STATIC mp_obj_t keypad_keymatrix_row_column_to_key_number(mp_obj_t self_in, mp_obj_t row_in, mp_obj_t column_in) {
    keypad_keymatrix_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    const mp_uint_t row = (mp_uint_t)mp_arg_validate_int_range(
        mp_obj_get_int(row_in), 0, (mp_int_t)common_hal_keypad_keymatrix_get_row_count(self), MP_QSTR_row);

    const mp_int_t column = (mp_uint_t)mp_arg_validate_int_range(
        mp_obj_get_int(column_in), 0, (mp_int_t)common_hal_keypad_keymatrix_get_column_count(self), MP_QSTR_column);

    return MP_OBJ_NEW_SMALL_INT(
        (mp_int_t)common_hal_keypad_keymatrix_row_column_to_key_number(self, row, column));
}
MP_DEFINE_CONST_FUN_OBJ_3(keypad_keymatrix_row_column_to_key_number_obj, keypad_keymatrix_row_column_to_key_number);

//|     events: EventQueue
//|     """The `EventQueue` associated with this `Keys` object. (read-only)
//|     """
//|
STATIC mp_obj_t keypad_keymatrix_get_events(mp_obj_t self_in) {
    keypad_keymatrix_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

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
    { MP_ROM_QSTR(MP_QSTR_deinit),                   MP_ROM_PTR(&keypad_keymatrix_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__),                MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__),                 MP_ROM_PTR(&keypad_keymatrix___exit___obj) },

    { MP_ROM_QSTR(MP_QSTR_events),                   MP_ROM_PTR(&keypad_keymatrix_events_obj) },
    { MP_ROM_QSTR(MP_QSTR_key_count),                MP_ROM_PTR(&keypad_keymatrix_key_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset),                    MP_ROM_PTR(&keypad_keymatrix_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_key_number_to_row_column), MP_ROM_PTR(&keypad_keymatrix_key_number_to_row_column_obj) },
    { MP_ROM_QSTR(MP_QSTR_row_column_to_key_number), MP_ROM_PTR(&keypad_keymatrix_row_column_to_key_number_obj) },
};

STATIC MP_DEFINE_CONST_DICT(keypad_keymatrix_locals_dict, keypad_keymatrix_locals_dict_table);

const mp_obj_type_t keypad_keymatrix_type = {
    { &mp_type_type },
    .name = MP_QSTR_KeyMatrix,
    .make_new = keypad_keymatrix_make_new,
    .locals_dict = (mp_obj_t)&keypad_keymatrix_locals_dict,
};
