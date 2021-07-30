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
#include "shared-bindings/keypad/Keys.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"

//| class Keys:
//|     """Manage a set of independent keys."""
//|
//|     def __init__(self, pins: Sequence[microcontroller.Pin], *, value_when_pressed: bool, pull: bool = True, interval: float = 0.020, max_events: int = 64) -> None:
//|         """
//|         Create a `Keys` object that will scan keys attached to the given sequence of pins.
//|         Each key is independent and attached to its own pin.
//|
//|         An `EventQueue` is created when this object is created and is available in the `events` attribute.
//|
//|         :param Sequence[microcontroller.Pin] pins: The pins attached to the keys.
//|           The key numbers correspond to indices into this sequence.
//|         :param bool value_when_pressed: ``True`` if the pin reads high when the key is pressed.
//|           ``False`` if the pin reads low (is grounded) when the key is pressed.
//|           All the pins must be connected in the same way.
//|         :param bool pull: ``True`` if an internal pull-up or pull-down should be
//|           enabled on each pin. A pull-up will be used if ``value_when_pressed`` is ``False``;
//|           a pull-down will be used if it is ``True``.
//|           If an external pull is already provided for all the pins, you can set ``pull`` to ``False``.
//|           However, enabling an internal pull when an external one is already present is not a problem;
//|           it simply uses slightly more current.
//|         :param float interval: Scan keys no more often than ``interval`` to allow for debouncing.
//|           ``interval`` is in float seconds. The default is 0.020 (20 msecs).
//|         :param int max_events: maximum size of `events` `EventQueue`:
//|           maximum number of key transition events that are saved.
//|           Must be >= 1.
//|           If a new event arrives when the queue is full, the oldest event is discarded.
//|         """
//|         ...

STATIC mp_obj_t keypad_keys_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    keypad_keys_obj_t *self = m_new_obj(keypad_keys_obj_t);
    self->base.type = &keypad_keys_type;
    enum { ARG_pins, ARG_value_when_pressed, ARG_pull, ARG_interval, ARG_max_events };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pins, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_value_when_pressed, MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_BOOL },
        { MP_QSTR_pull, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
        { MP_QSTR_interval, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL } },
        { MP_QSTR_max_events, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 64} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t pins = args[ARG_pins].u_obj;
    // mp_obj_len() will be >= 0.
    const size_t num_pins = (size_t)MP_OBJ_SMALL_INT_VALUE(mp_obj_len(pins));

    const bool value_when_pressed = args[ARG_value_when_pressed].u_bool;
    const mp_float_t interval =
        mp_arg_validate_obj_float_non_negative(args[ARG_interval].u_obj, 0.020f, MP_QSTR_interval);
    const size_t max_events = (size_t)mp_arg_validate_int_min(args[ARG_max_events].u_int, 1, MP_QSTR_max_events);

    mcu_pin_obj_t *pins_array[num_pins];

    for (mp_uint_t i = 0; i < num_pins; i++) {
        mcu_pin_obj_t *pin =
            validate_obj_is_free_pin(mp_obj_subscr(pins, MP_OBJ_NEW_SMALL_INT(i), MP_OBJ_SENTINEL));
        pins_array[i] = pin;
    }

    common_hal_keypad_keys_construct(self, num_pins, pins_array, value_when_pressed, args[ARG_pull].u_bool, interval, max_events);

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Stop scanning and release the pins."""
//|         ...
//|
STATIC mp_obj_t keypad_keys_deinit(mp_obj_t self_in) {
    keypad_keys_obj_t *self = MP_OBJ_TO_PTR(self_in);

    common_hal_keypad_keys_deinit(self);
    return MP_ROM_NONE;
}
MP_DEFINE_CONST_FUN_OBJ_1(keypad_keys_deinit_obj, keypad_keys_deinit);

//|     def __enter__(self) -> Keys:
//|         """No-op used by Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
STATIC mp_obj_t keypad_keys___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_keypad_keys_deinit(args[0]);
    return MP_ROM_NONE;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(keypad_keys___exit___obj, 4, 4, keypad_keys___exit__);

STATIC void check_for_deinit(keypad_keys_obj_t *self) {
    if (common_hal_keypad_keys_deinited(self)) {
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
STATIC mp_obj_t keypad_keys_reset(mp_obj_t self_in) {
    keypad_keys_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    common_hal_keypad_keys_reset(self);
    return MP_ROM_NONE;
}
MP_DEFINE_CONST_FUN_OBJ_1(keypad_keys_reset_obj, keypad_keys_reset);

//|     key_count: int
//|     """The number of keys that are being scanned. (read-only)
//|     """
//|
STATIC mp_obj_t keypad_keys_get_key_count(mp_obj_t self_in) {
    keypad_keys_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    return MP_OBJ_NEW_SMALL_INT(common_hal_keypad_keys_get_key_count(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(keypad_keys_get_key_count_obj, keypad_keys_get_key_count);

const mp_obj_property_t keypad_keys_key_count_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&keypad_keys_get_key_count_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

//|     events: EventQueue
//|     """The `EventQueue` associated with this `Keys` object. (read-only)
//|     """
//|
STATIC mp_obj_t keypad_keys_get_events(mp_obj_t self_in) {
    keypad_keys_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    return common_hal_keypad_keys_get_events(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(keypad_keys_get_events_obj, keypad_keys_get_events);

const mp_obj_property_t keypad_keys_events_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&keypad_keys_get_events_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

STATIC const mp_rom_map_elem_t keypad_keys_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit),       MP_ROM_PTR(&keypad_keys_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__),    MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__),     MP_ROM_PTR(&keypad_keys___exit___obj) },

    { MP_ROM_QSTR(MP_QSTR_events),       MP_ROM_PTR(&keypad_keys_events_obj) },
    { MP_ROM_QSTR(MP_QSTR_key_count),    MP_ROM_PTR(&keypad_keys_key_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset),        MP_ROM_PTR(&keypad_keys_reset_obj) },
};

STATIC MP_DEFINE_CONST_DICT(keypad_keys_locals_dict, keypad_keys_locals_dict_table);

const mp_obj_type_t keypad_keys_type = {
    { &mp_type_type },
    .name = MP_QSTR_Keys,
    .make_new = keypad_keys_make_new,
    .locals_dict = (mp_obj_t)&keypad_keys_locals_dict,
};
