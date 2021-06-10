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

#include "py/enum.h"
#include "py/objproperty.h"
#include "shared-bindings/keypad/Keys.h"
#include "shared-bindings/keypad/State.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "py/runtime.h"

//| class Keys:
//|     """Manage a set of independent keys."""
//|
//|     def __init__(self, pins: Sequence[microcontroller.Pin], *, level_when_pressed: bool, pull: bool = True) -> None:
//|         """
//|         Create a `Keys` object that will scan keys attached to the given sequence of pins.
//|         Each key is independent and attached to its own pin.
//|
//|         :param Sequence[microcontroller.Pin] pins: The pins attached to the keys.
//|           The key numbers correspond to indices into this sequence.
//|         :param bool value_when_pressed: ``True`` if the pin reads high when the key is pressed.
//|           ``False`` if the pin reads low (is grounded) when the key is pressed.
//|           All the pins must be connected in the same way.
//|         :param bool pull: ``True`` if an internal pull-up or pull-down should be
//|            enabled on each pin. A pull-up will be used if ``value_when_pressed`` is ``False``;
//|            a pull-down will be used if it is ``True``.
//|            If an external pull is already provided for all the pins, you can set ``pull`` to ``False``.
//|            However, enabling an internal pull when an external one is already present is not a problem;
//|            it simply uses slightly more current.
//|
//|         Calls `scan()` once before returning, to initialize internal state.
//|         """
//|         ...

STATIC mp_obj_t keypad_keys_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    keypad_keys_obj_t *self = m_new_obj(keypad_keys_obj_t);
    self->base.type = &keypad_keys_type;
    enum { ARG_pins, ARG_value_when_pressed, ARG_pull };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pins, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_value_when_pressed, MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_BOOL },
        { MP_QSTR_pull, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t pins = args[ARG_pins].u_obj;
    // mp_obj_len() will be >= 0.
    const size_t num_pins = (size_t)MP_OBJ_SMALL_INT_VALUE(mp_obj_len(pins));
    const bool value_when_pressed = args[ARG_value_when_pressed].u_bool;

    mcu_pin_obj_t *pins_array[num_pins];

    for (mp_uint_t i = 0; i < num_pins; i++) {
        mcu_pin_obj_t *pin =
            validate_obj_is_free_pin(mp_obj_subscr(pins, MP_OBJ_NEW_SMALL_INT(i), MP_OBJ_SENTINEL));
        pins_array[i] = pin;
    }

    common_hal_keypad_keys_construct(self, num_pins, pins_array, value_when_pressed, args[ARG_pull].u_bool);
    common_hal_keypad_keys_scan(self);
    return MP_OBJ_FROM_PTR(self);
}

//|     def scan(self) -> bool:
//|         """Scan the keys and record which are newly pressed, still pressed,
//|         newly released, and still released. If not enough time has elapsed since
//|         the last scan for debouncing, do nothing and return ``False``.
//|
//|         :return: ``True`` if sufficient time has elapsed for debouncing (about 20 msecs),
//|           otherwise ``False``.
//|         :rtype: bool
//|         """
//|         ...
//|
STATIC mp_obj_t keypad_keys_scan(mp_obj_t self_in) {
    keypad_keys_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_bool(common_hal_keypad_keys_scan(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(keypad_keys_scan_obj, keypad_keys_scan);

//|     def state(self, key_num: int) -> keypad.State:
//|         """Return the state for the given ``key_num``, based
//|         on the results of the most recent `scan()`.
//|
//|         :param int key_num: Key number: corresponds to the sequence of pins
//|         :return: state of key number ``key_num``
//|         :rtype: keypad.State: One of `State.JUST_PRESSED`, `State.STILL_PRESSED`,
//|           `State.JUST_RELEASED`, or `State.STILL_RELEASED`.
//|           The inclusive states `State.PRESSED` and `State.RELEASED` will *not* be returned.
//|         """
//|         ...
//|
STATIC mp_obj_t keypad_keys_state(mp_obj_t self_in, mp_obj_t key_num_obj) {
    keypad_keys_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t key_num = mp_obj_int_get_checked(key_num_obj);
    if (key_num < 0 || (mp_uint_t)key_num >= common_hal_keypad_keys_length(self)) {
        mp_raise_ValueError_varg(translate("%q out of range"), MP_QSTR_key_num);
    }

    return cp_enum_find(&keypad_state_type, common_hal_keypad_keys_state(self, (mp_uint_t)key_num));
}
MP_DEFINE_CONST_FUN_OBJ_2(keypad_keys_state_obj, keypad_keys_state);

//|     def keys_with_state(self, state: State, into_list: List[Optional[int]]) -> None:
//|         """Store key numbers of keys with state ``state`` in ``into_list``.
//|         The states checked are based on the results of the most recent `scan()`.
//|
//|         You can use the inclusive states `State.PRESSED` and `State.RELEASED`.
//|         `State.PRESSED` includes states `State.JUST_PRESSED` and `State.STILL_PRESSED`.
//|         `State.RELEASED` includes `State.JUST_RELEASED` and `State.STILL_RELEASED`.
//|
//|         The key numbers are stored in ``into_list`` consecutively, up to ``len(into_list)``.
//|         The ``into_list`` is not extended if there are more keys with the given
//|         state than list slots. Instead, leftover key numbers are discarded.
//|         If there are fewer keys with the given state, the rest of ``into_list``
//|         is padded with ``None``. For example,
//|         if four keys are being monitored, and only key numbers 0 and 2 have the given state,
//|         ``into_list`` will be set to ``[0, 2, None, None]``. You can iterate over
//|         ``into_list`` and stop when you find the first ``None``.
//|         """
//|         ...
//|
STATIC mp_obj_t keypad_keys_keys_with_state(mp_obj_t self_in, mp_obj_t state_in, mp_obj_t into_list_in) {
    keypad_keys_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (!mp_obj_is_type(state_in, &keypad_state_type)) {
        mp_raise_ValueError_varg(translate("Expected a %q"), keypad_state_type.name);
    }

    if (!mp_obj_is_type(into_list_in, &mp_type_list)) {
        mp_raise_ValueError_varg(translate("Expected a %q"), mp_type_list.name);
    }

    int state = cp_enum_value(&keypad_state_type, state_in);
    mp_obj_list_t *into_list = MP_OBJ_TO_PTR(into_list_in);

    common_hal_keypad_keys_keys_with_state(self, state, into_list);

    return MP_ROM_NONE;
}
MP_DEFINE_CONST_FUN_OBJ_3(keypad_keys_keys_with_state_obj, keypad_keys_keys_with_state);

STATIC const mp_rom_map_elem_t keypad_keys_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_keys_with_state),  MP_ROM_PTR(&keypad_keys_keys_with_state_obj) },
    { MP_ROM_QSTR(MP_QSTR_scan),             MP_ROM_PTR(&keypad_keys_scan_obj) },
    { MP_ROM_QSTR(MP_QSTR_state),            MP_ROM_PTR(&keypad_keys_state_obj) },
};

STATIC MP_DEFINE_CONST_DICT(keypad_keys_locals_dict, keypad_keys_locals_dict_table);

const mp_obj_type_t keypad_keys_type = {
    { &mp_type_type },
    .name = MP_QSTR_Keys,
    .make_new = keypad_keys_make_new,
    .locals_dict = (mp_obj_t)&keypad_keys_locals_dict,
};
