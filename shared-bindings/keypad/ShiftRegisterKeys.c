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
#include "shared-bindings/keypad/ShiftRegisterKeys.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"

//| class ShiftRegisterKeys:
//|     """Manage a set of keys attached to an incoming shift register."""
//|
//|     def __init__(self, *, clock: microcontroller.Pin, data: microcontroller.Pin, latch: microcontroller.Pin, value_to_latch: bool = True, key_count: int, value_when_pressed: bool, interval: float = 0.020, max_events: int = 64) -> None:
//|         """
//|         Create a `Keys` object that will scan keys attached to a parallel-in serial-out shift register
//|         like the 74HC165 or CD4021.
//|         Note that you may chain shift registers to load in as many values as you need.
//|
//|         Key number 0 is the first (or more properly, the zero-th) bit read. In the
//|         74HC165, this bit is labeled ``Q7``. Key number 1 will be the value of ``Q6``, etc.
//|
//|         An `EventQueue` is created when this object is created and is available in the `events` attribute.
//|
//|         :param microcontroller.Pin clock: The shift register clock pin.
//|           The shift register should clock on a low-to-high transition.
//|         :param microcontroller.Pin data: the incoming shift register data pin
//|         :param microcontroller.Pin latch:
//|           Pin used to latch parallel data going into the shift register.
//|         :param bool value_to_latch: Pin state to latch data being read.
//|           ``True`` if the data is latched when ``latch`` goes high
//|           ``False`` if the data is latched when ``latch goes low.
//|           The default is ``True``, which is how the 74HC165 operates. The CD4021 latch is the opposite.
//|           Once the data is latched, it will be shifted out by toggling the clock pin.
//|         :param int key_count: number of data lines to clock in
//|         :param bool value_when_pressed: ``True`` if the pin reads high when the key is pressed.
//|           ``False`` if the pin reads low (is grounded) when the key is pressed.
//|         :param float interval: Scan keys no more often than ``interval`` to allow for debouncing.
//|           ``interval`` is in float seconds. The default is 0.020 (20 msecs).
//|         :param int max_events: maximum size of `events` `EventQueue`:
//|           maximum number of key transition events that are saved.
//|           Must be >= 1.
//|           If a new event arrives when the queue is full, the oldest event is discarded.
//|         """
//|         ...

STATIC mp_obj_t keypad_shiftregisterkeys_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    keypad_shiftregisterkeys_obj_t *self = m_new_obj(keypad_shiftregisterkeys_obj_t);
    self->base.type = &keypad_shiftregisterkeys_type;
    enum { ARG_clock, ARG_data, ARG_latch, ARG_value_to_latch, ARG_key_count, ARG_value_when_pressed, ARG_interval, ARG_max_events };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_clock, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_data, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_latch, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_value_to_latch, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
        { MP_QSTR_key_count, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_value_when_pressed, MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_BOOL },
        { MP_QSTR_interval, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_max_events, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 64} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mcu_pin_obj_t *clock = validate_obj_is_free_pin(args[ARG_clock].u_obj);
    mcu_pin_obj_t *data = validate_obj_is_free_pin(args[ARG_data].u_obj);
    mcu_pin_obj_t *latch = validate_obj_is_free_pin(args[ARG_latch].u_obj);
    const bool value_to_latch = args[ARG_value_to_latch].u_bool;

    const size_t key_count = (size_t)mp_arg_validate_int_min(args[ARG_key_count].u_int, 1, MP_QSTR_key_count);
    const bool value_when_pressed = args[ARG_value_when_pressed].u_bool;
    const mp_float_t interval =
        mp_arg_validate_obj_float_non_negative(args[ARG_interval].u_obj, 0.020f, MP_QSTR_interval);
    const size_t max_events = (size_t)mp_arg_validate_int_min(args[ARG_max_events].u_int, 1, MP_QSTR_max_events);

    common_hal_keypad_shiftregisterkeys_construct(
        self, clock, data, latch, value_to_latch, key_count, value_when_pressed, interval, max_events);

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Stop scanning and release the pins."""
//|         ...
//|
STATIC mp_obj_t keypad_shiftregisterkeys_deinit(mp_obj_t self_in) {
    keypad_shiftregisterkeys_obj_t *self = MP_OBJ_TO_PTR(self_in);

    common_hal_keypad_shiftregisterkeys_deinit(self);
    return MP_ROM_NONE;
}
MP_DEFINE_CONST_FUN_OBJ_1(keypad_shiftregisterkeys_deinit_obj, keypad_shiftregisterkeys_deinit);

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
STATIC mp_obj_t keypad_shiftregisterkeys___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_keypad_shiftregisterkeys_deinit(args[0]);
    return MP_ROM_NONE;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(keypad_shiftregisterkeys___exit___obj, 4, 4, keypad_shiftregisterkeys___exit__);

STATIC void check_for_deinit(keypad_shiftregisterkeys_obj_t *self) {
    if (common_hal_keypad_shiftregisterkeys_deinited(self)) {
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
STATIC mp_obj_t keypad_shiftregisterkeys_reset(mp_obj_t self_in) {
    keypad_shiftregisterkeys_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    common_hal_keypad_shiftregisterkeys_reset(self);
    return MP_ROM_NONE;
}
MP_DEFINE_CONST_FUN_OBJ_1(keypad_shiftregisterkeys_reset_obj, keypad_shiftregisterkeys_reset);

//|     key_count: int
//|     """The number of keys that are being scanned. (read-only)
//|     """
//|
STATIC mp_obj_t keypad_shiftregisterkeys_get_key_count(mp_obj_t self_in) {
    keypad_shiftregisterkeys_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    return MP_OBJ_NEW_SMALL_INT(common_hal_keypad_shiftregisterkeys_get_key_count(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(keypad_shiftregisterkeys_get_key_count_obj, keypad_shiftregisterkeys_get_key_count);

const mp_obj_property_t keypad_shiftregisterkeys_key_count_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&keypad_shiftregisterkeys_get_key_count_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

//|     events: EventQueue
//|     """The `EventQueue` associated with this `Keys` object. (read-only)
//|     """
//|
STATIC mp_obj_t keypad_shiftregisterkeys_get_events(mp_obj_t self_in) {
    keypad_shiftregisterkeys_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    return common_hal_keypad_shiftregisterkeys_get_events(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(keypad_shiftregisterkeys_get_events_obj, keypad_shiftregisterkeys_get_events);

const mp_obj_property_t keypad_shiftregisterkeys_events_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&keypad_shiftregisterkeys_get_events_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

STATIC const mp_rom_map_elem_t keypad_shiftregisterkeys_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit),       MP_ROM_PTR(&keypad_shiftregisterkeys_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__),    MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__),     MP_ROM_PTR(&keypad_shiftregisterkeys___exit___obj) },

    { MP_ROM_QSTR(MP_QSTR_events),       MP_ROM_PTR(&keypad_shiftregisterkeys_events_obj) },
    { MP_ROM_QSTR(MP_QSTR_key_count),    MP_ROM_PTR(&keypad_shiftregisterkeys_key_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset),        MP_ROM_PTR(&keypad_shiftregisterkeys_reset_obj) },
};

STATIC MP_DEFINE_CONST_DICT(keypad_shiftregisterkeys_locals_dict, keypad_shiftregisterkeys_locals_dict_table);

const mp_obj_type_t keypad_shiftregisterkeys_type = {
    { &mp_type_type },
    .name = MP_QSTR_ShiftRegisterKeys,
    .make_new = keypad_shiftregisterkeys_make_new,
    .locals_dict = (mp_obj_t)&keypad_shiftregisterkeys_locals_dict,
};
