/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include <stdint.h>

#include "lib/utils/context_manager_helpers.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/runtime0.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/bitbangio/OneWire.h"
#include "shared-bindings/util.h"

//| class OneWire:
//|     """Lowest-level of the Maxim OneWire protocol
//|
//|     :class:`~bitbangio.OneWire` implements the timing-sensitive foundation of
//|     the Maxim (formerly Dallas Semi) OneWire protocol.
//|
//|     Protocol definition is here: https://www.maximintegrated.com/en/app-notes/index.mvp/id/126"""
//|
//|     def __init__(self, pin: microcontroller.Pin) -> None:
//|
//|         """Create a OneWire object associated with the given pin. The object
//|         implements the lowest level timing-sensitive bits of the protocol.
//|
//|         :param ~microcontroller.Pin pin: Pin to read pulses from.
//|
//|         Read a short series of pulses::
//|
//|           import bitbangio
//|           import board
//|
//|           onewire = bitbangio.OneWire(board.D7)
//|           onewire.reset()
//|           onewire.write_bit(True)
//|           onewire.write_bit(False)
//|           print(onewire.read_bit())"""
//|         ...
//|
STATIC mp_obj_t bitbangio_onewire_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_pin };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pin, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mcu_pin_obj_t *pin = validate_obj_is_free_pin(args[ARG_pin].u_obj);

    bitbangio_onewire_obj_t *self = m_new_obj(bitbangio_onewire_obj_t);
    self->base.type = &bitbangio_onewire_type;

    shared_module_bitbangio_onewire_construct(self, pin);
    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Deinitialize the OneWire bus and release any hardware resources for reuse."""
//|         ...
//|
STATIC mp_obj_t bitbangio_onewire_deinit(mp_obj_t self_in) {
    bitbangio_onewire_obj_t *self = MP_OBJ_TO_PTR(self_in);
    shared_module_bitbangio_onewire_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bitbangio_onewire_deinit_obj, bitbangio_onewire_deinit);

STATIC void check_for_deinit(bitbangio_onewire_obj_t *self) {
    if (shared_module_bitbangio_onewire_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def __enter__(self) -> OneWire:
//|         """No-op used by Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
STATIC mp_obj_t bitbangio_onewire_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    shared_module_bitbangio_onewire_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(bitbangio_onewire___exit___obj, 4, 4, bitbangio_onewire_obj___exit__);

//|     def reset(self) -> bool:
//|         """Reset the OneWire bus"""
//|         ...
//|
STATIC mp_obj_t bitbangio_onewire_obj_reset(mp_obj_t self_in) {
    bitbangio_onewire_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    return mp_obj_new_bool(shared_module_bitbangio_onewire_reset(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(bitbangio_onewire_reset_obj, bitbangio_onewire_obj_reset);

//|     def read_bit(self) -> bool:
//|         """Read in a bit
//|
//|         :returns: bit state read
//|         :rtype: bool"""
//|         ...
//|
STATIC mp_obj_t bitbangio_onewire_obj_read_bit(mp_obj_t self_in) {
    bitbangio_onewire_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    return mp_obj_new_bool(shared_module_bitbangio_onewire_read_bit(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(bitbangio_onewire_read_bit_obj, bitbangio_onewire_obj_read_bit);

//|     def write_bit(self, value: bool) -> None:
//|         """Write out a bit based on value."""
//|         ...
//|
STATIC mp_obj_t bitbangio_onewire_obj_write_bit(mp_obj_t self_in, mp_obj_t bool_obj) {
    bitbangio_onewire_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    shared_module_bitbangio_onewire_write_bit(self, mp_obj_is_true(bool_obj));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(bitbangio_onewire_write_bit_obj, bitbangio_onewire_obj_write_bit);

STATIC const mp_rom_map_elem_t bitbangio_onewire_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&bitbangio_onewire_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&bitbangio_onewire___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&bitbangio_onewire_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_bit), MP_ROM_PTR(&bitbangio_onewire_read_bit_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_bit), MP_ROM_PTR(&bitbangio_onewire_write_bit_obj) },
};
STATIC MP_DEFINE_CONST_DICT(bitbangio_onewire_locals_dict, bitbangio_onewire_locals_dict_table);

const mp_obj_type_t bitbangio_onewire_type = {
    { &mp_type_type },
    .name = MP_QSTR_OneWire,
    .make_new = bitbangio_onewire_make_new,
    .locals_dict = (mp_obj_dict_t *)&bitbangio_onewire_locals_dict,
};
