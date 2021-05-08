/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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
#include "shared-bindings/rotaryio/IncrementalEncoder.h"
#include "shared-bindings/util.h"

//| class IncrementalEncoder:
//|     """IncrementalEncoder determines the relative rotational position based on two series of pulses."""
//|
//|     def __init__(self, pin_a: microcontroller.Pin, pin_b: microcontroller.Pin) -> None:
//|         """Create an IncrementalEncoder object associated with the given pins. It tracks the positional
//|         state of an incremental rotary encoder (also known as a quadrature encoder.) Position is
//|         relative to the position when the object is contructed.
//|
//|         :param ~microcontroller.Pin pin_a: First pin to read pulses from.
//|         :param ~microcontroller.Pin pin_b: Second pin to read pulses from.
//|
//|         For example::
//|
//|           import rotaryio
//|           import time
//|           from board import *
//|
//|           enc = rotaryio.IncrementalEncoder(D1, D2)
//|           last_position = None
//|           while True:
//|               position = enc.position
//|               if last_position == None or position != last_position:
//|                   print(position)
//|               last_position = position"""
//|         ...
//|
STATIC mp_obj_t rotaryio_incrementalencoder_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_pin_a, ARG_pin_b };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pin_a, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_pin_b, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mcu_pin_obj_t *pin_a = validate_obj_is_free_pin(args[ARG_pin_a].u_obj);
    const mcu_pin_obj_t *pin_b = validate_obj_is_free_pin(args[ARG_pin_b].u_obj);

    rotaryio_incrementalencoder_obj_t *self = m_new_obj(rotaryio_incrementalencoder_obj_t);
    self->base.type = &rotaryio_incrementalencoder_type;

    common_hal_rotaryio_incrementalencoder_construct(self, pin_a, pin_b);

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Deinitializes the IncrementalEncoder and releases any hardware resources for reuse."""
//|         ...
//|
STATIC mp_obj_t rotaryio_incrementalencoder_deinit(mp_obj_t self_in) {
    rotaryio_incrementalencoder_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_rotaryio_incrementalencoder_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(rotaryio_incrementalencoder_deinit_obj, rotaryio_incrementalencoder_deinit);

STATIC void check_for_deinit(rotaryio_incrementalencoder_obj_t *self) {
    if (common_hal_rotaryio_incrementalencoder_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def __enter__(self) -> IncrementalEncoder:
//|         """No-op used by Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
STATIC mp_obj_t rotaryio_incrementalencoder_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_rotaryio_incrementalencoder_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rotaryio_incrementalencoder___exit___obj, 4, 4, rotaryio_incrementalencoder_obj___exit__);


//|     position: int
//|     """The current position in terms of pulses. The number of pulses per rotation is defined by the
//|     specific hardware."""
//|
STATIC mp_obj_t rotaryio_incrementalencoder_obj_get_position(mp_obj_t self_in) {
    rotaryio_incrementalencoder_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    return mp_obj_new_int(common_hal_rotaryio_incrementalencoder_get_position(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(rotaryio_incrementalencoder_get_position_obj, rotaryio_incrementalencoder_obj_get_position);

STATIC mp_obj_t rotaryio_incrementalencoder_obj_set_position(mp_obj_t self_in, mp_obj_t new_position) {
    rotaryio_incrementalencoder_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    common_hal_rotaryio_incrementalencoder_set_position(self, mp_obj_get_int(new_position));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(rotaryio_incrementalencoder_set_position_obj, rotaryio_incrementalencoder_obj_set_position);

const mp_obj_property_t rotaryio_incrementalencoder_position_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&rotaryio_incrementalencoder_get_position_obj,
              (mp_obj_t)&rotaryio_incrementalencoder_set_position_obj,
              MP_ROM_NONE},
};

STATIC const mp_rom_map_elem_t rotaryio_incrementalencoder_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&rotaryio_incrementalencoder_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&rotaryio_incrementalencoder___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_position), MP_ROM_PTR(&rotaryio_incrementalencoder_position_obj) },
};
STATIC MP_DEFINE_CONST_DICT(rotaryio_incrementalencoder_locals_dict, rotaryio_incrementalencoder_locals_dict_table);

const mp_obj_type_t rotaryio_incrementalencoder_type = {
    { &mp_type_type },
    .name = MP_QSTR_IncrementalEncoder,
    .make_new = rotaryio_incrementalencoder_make_new,
    .locals_dict = (mp_obj_dict_t *)&rotaryio_incrementalencoder_locals_dict,
};
