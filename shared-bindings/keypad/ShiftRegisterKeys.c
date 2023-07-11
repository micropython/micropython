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

#include "shared/runtime/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/keypad/__init__.h"
#include "shared-bindings/keypad/Event.h"
#include "shared-bindings/keypad/ShiftRegisterKeys.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"

//| class ShiftRegisterKeys:
//|     """Manage a set of keys attached to an incoming shift register."""
//|
//|     def __init__(
//|         self,
//|         *,
//|         clock: microcontroller.Pin,
//|         data: Union[microcontroller.Pin, Sequence[microcontroller.Pin]],
//|         latch: microcontroller.Pin,
//|         value_to_latch: bool = True,
//|         key_count: Union[int, Sequence[int]],
//|         value_when_pressed: bool,
//|         interval: float = 0.020,
//|         max_events: int = 64
//|     ) -> None:
//|         """
//|         Create a `Keys` object that will scan keys attached to a parallel-in serial-out shift register
//|         like the 74HC165 or CD4021.
//|         Note that you may chain shift registers to load in as many values as you need.
//|         Furthermore, you can put multiple shift registers in parallel and share clock and latch.
//|
//|         Key number 0 is the first (or more properly, the zero-th) bit read. In the
//|         74HC165, this bit is labeled ``Q7``. Key number 1 will be the value of ``Q6``, etc.
//|         With multiple data pins, key numbers of the next pin are sequentially to the current pin.
//|
//|         An `EventQueue` is created when this object is created and is available in the `events` attribute.
//|
//|         :param microcontroller.Pin clock: The shift register clock pin.
//|           The shift register should clock on a low-to-high transition.
//|         :param Union[microcontroller.Pin, Sequence[microcontroller.Pin]] data: the incoming shift register data pin(s)
//|         :param microcontroller.Pin latch:
//|           Pin used to latch parallel data going into the shift register.
//|         :param bool value_to_latch: Pin state to latch data being read.
//|           ``True`` if the data is latched when ``latch`` goes high
//|           ``False`` if the data is latched when ``latch`` goes low.
//|           The default is ``True``, which is how the 74HC165 operates. The CD4021 latch is the opposite.
//|           Once the data is latched, it will be shifted out by toggling the clock pin.
//|         :param Union[int, Sequence[int]] key_count: number of data lines to clock in (per data pin)
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

STATIC mp_obj_t keypad_shiftregisterkeys_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    #if CIRCUITPY_KEYPAD_SHIFTREGISTERKEYS
    keypad_shiftregisterkeys_obj_t *self = m_new_obj(keypad_shiftregisterkeys_obj_t);
    self->base.type = &keypad_shiftregisterkeys_type;
    enum { ARG_clock, ARG_data, ARG_latch, ARG_value_to_latch, ARG_key_count, ARG_value_when_pressed, ARG_interval, ARG_max_events };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_clock, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_data, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_latch, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_value_to_latch, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
        { MP_QSTR_key_count, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_value_when_pressed, MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_BOOL },
        { MP_QSTR_interval, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_max_events, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 64} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    size_t num_data_pins;

    if (mp_obj_is_type(args[ARG_data].u_obj, &mcu_pin_type)) {
        num_data_pins = 1;
    } else {
        num_data_pins = (size_t)MP_OBJ_SMALL_INT_VALUE(mp_obj_len(args[ARG_data].u_obj));
    }

    const mcu_pin_obj_t *data_pins_array[num_data_pins];

    if (mp_obj_is_type(args[ARG_data].u_obj, &mcu_pin_type)) {
        const mcu_pin_obj_t *datapin = validate_obj_is_free_pin(args[ARG_data].u_obj, MP_QSTR_data);
        data_pins_array[0] = datapin;
    } else {
        for (size_t pin = 0; pin < num_data_pins; pin++) {
            const mcu_pin_obj_t *datapin =
                validate_obj_is_free_pin(mp_obj_subscr(args[ARG_data].u_obj, MP_OBJ_NEW_SMALL_INT(pin), MP_OBJ_SENTINEL), MP_QSTR_data);
            data_pins_array[pin] = datapin;
        }
    }

    size_t num_key_counts;

    if (mp_obj_is_int(args[ARG_key_count].u_obj)) {
        num_key_counts = 1;
    } else {
        num_key_counts = (size_t)MP_OBJ_SMALL_INT_VALUE(mp_obj_len(args[ARG_key_count].u_obj));
    }

    mp_arg_validate_length(num_key_counts, num_data_pins, MP_QSTR_key_count);

    size_t key_count_array[num_key_counts];

    if (mp_obj_is_int(args[ARG_key_count].u_obj)) {
        const size_t key_count = (size_t)mp_arg_validate_int_min(args[ARG_key_count].u_int, 1, MP_QSTR_key_count);
        key_count_array[0] = key_count;
    } else {
        for (size_t kc = 0; kc < num_key_counts; kc++) {
            mp_int_t mpint = mp_obj_get_int(mp_obj_subscr(args[ARG_key_count].u_obj, MP_OBJ_NEW_SMALL_INT(kc), MP_OBJ_SENTINEL));
            const size_t key_count = (size_t)mp_arg_validate_int_min(mpint, 1, MP_QSTR_key_count);
            key_count_array[kc] = key_count;
        }
    }

    const mcu_pin_obj_t *clock = validate_obj_is_free_pin(args[ARG_clock].u_obj, MP_QSTR_clock);
    const mcu_pin_obj_t *latch = validate_obj_is_free_pin(args[ARG_latch].u_obj, MP_QSTR_latch);
    const bool value_to_latch = args[ARG_value_to_latch].u_bool;

    const bool value_when_pressed = args[ARG_value_when_pressed].u_bool;
    const mp_float_t interval =
        mp_arg_validate_obj_float_non_negative(args[ARG_interval].u_obj, 0.020f, MP_QSTR_interval);
    const size_t max_events = (size_t)mp_arg_validate_int_min(args[ARG_max_events].u_int, 1, MP_QSTR_max_events);

    common_hal_keypad_shiftregisterkeys_construct(
        self, clock, num_data_pins, data_pins_array, latch, value_to_latch, num_key_counts, key_count_array, value_when_pressed, interval, max_events);

    return MP_OBJ_FROM_PTR(self);

    #else
    mp_raise_NotImplementedError_varg(translate("%q"), MP_QSTR_ShiftRegisterKeys);
    #endif
}

#if CIRCUITPY_KEYPAD_SHIFTREGISTERKEYS
//|     def deinit(self) -> None:
//|         """Stop scanning and release the pins."""
//|         ...
STATIC mp_obj_t keypad_shiftregisterkeys_deinit(mp_obj_t self_in) {
    keypad_shiftregisterkeys_obj_t *self = MP_OBJ_TO_PTR(self_in);

    common_hal_keypad_shiftregisterkeys_deinit(self);
    return MP_ROM_NONE;
}
MP_DEFINE_CONST_FUN_OBJ_1(keypad_shiftregisterkeys_deinit_obj, keypad_shiftregisterkeys_deinit);

//|     def __enter__(self) -> Keys:
//|         """No-op used by Context Managers."""
//|         ...
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
STATIC mp_obj_t keypad_shiftregisterkeys___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_keypad_shiftregisterkeys_deinit(args[0]);
    return MP_ROM_NONE;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(keypad_shiftregisterkeys___exit___obj, 4, 4, keypad_shiftregisterkeys___exit__);

//|     def reset(self) -> None:
//|         """Reset the internal state of the scanner to assume that all keys are now released.
//|         Any key that is already pressed at the time of this call will therefore immediately cause
//|         a new key-pressed event to occur.
//|         """
//|         ...

//|     key_count: int
//|     """The total number of keys that are being scanned. (read-only)
//|     """

//|     events: EventQueue
//|     """The `EventQueue` associated with this `Keys` object. (read-only)
//|     """
//|

STATIC const mp_rom_map_elem_t keypad_shiftregisterkeys_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit),       MP_ROM_PTR(&keypad_shiftregisterkeys_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__),    MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__),     MP_ROM_PTR(&keypad_shiftregisterkeys___exit___obj) },

    { MP_ROM_QSTR(MP_QSTR_events),       MP_ROM_PTR(&keypad_generic_events_obj) },
    { MP_ROM_QSTR(MP_QSTR_key_count),    MP_ROM_PTR(&keypad_generic_key_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset),        MP_ROM_PTR(&keypad_generic_reset_obj) },
};

STATIC MP_DEFINE_CONST_DICT(keypad_shiftregisterkeys_locals_dict, keypad_shiftregisterkeys_locals_dict_table);
#endif

const mp_obj_type_t keypad_shiftregisterkeys_type = {
    { &mp_type_type },
    .name = MP_QSTR_ShiftRegisterKeys,
    .make_new = keypad_shiftregisterkeys_make_new,
    #if CIRCUITPY_KEYPAD_SHIFTREGISTERKEYS
    .locals_dict = (mp_obj_t)&keypad_shiftregisterkeys_locals_dict,
    #endif
};
