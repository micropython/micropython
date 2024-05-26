// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
//
// SPDX-License-Identifier: MIT

#include <stdint.h>
#include <string.h>

#include "shared/runtime/context_manager_helpers.h"
#include "py/binary.h"
#include "py/mphal.h"
#include "py/nlr.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/touchio/TouchIn.h"
#include "shared-bindings/util.h"

//| class TouchIn:
//|     """Read the state of a capacitive touch sensor
//|
//|     Usage::
//|
//|        import touchio
//|        from board import *
//|
//|        touch = touchio.TouchIn(A1)
//|        while True:
//|            if touch.value:
//|                print("touched!")"""
//|

//|     def __init__(self, pin: microcontroller.Pin) -> None:
//|         """Use the TouchIn on the given pin.
//|
//|         :param ~microcontroller.Pin pin: the pin to read from"""
//|         ...
static mp_obj_t touchio_touchin_make_new(const mp_obj_type_t *type,
    size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check number of arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    // 1st argument is the pin
    const mcu_pin_obj_t *pin = validate_obj_is_free_pin(args[0], MP_QSTR_pin);

    touchio_touchin_obj_t *self = mp_obj_malloc(touchio_touchin_obj_t, &touchio_touchin_type);
    common_hal_touchio_touchin_construct(self, pin);

    return (mp_obj_t)self;
}

//|     def deinit(self) -> None:
//|         """Deinitialises the TouchIn and releases any hardware resources for reuse."""
//|         ...
static mp_obj_t touchio_touchin_deinit(mp_obj_t self_in) {
    touchio_touchin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_touchio_touchin_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(touchio_touchin_deinit_obj, touchio_touchin_deinit);

static void check_for_deinit(touchio_touchin_obj_t *self) {
    if (common_hal_touchio_touchin_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def __enter__(self) -> TouchIn:
//|         """No-op used by Context Managers."""
//|         ...
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
static mp_obj_t touchio_touchin_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_touchio_touchin_deinit(args[0]);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(touchio_touchin___exit___obj, 4, 4, touchio_touchin_obj___exit__);

//|     value: bool
//|     """Whether the touch pad is being touched or not. (read-only)
//|
//|     True when `raw_value` > `threshold`."""
static mp_obj_t touchio_touchin_obj_get_value(mp_obj_t self_in) {
    touchio_touchin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_touchio_touchin_get_value(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(touchio_touchin_get_value_obj, touchio_touchin_obj_get_value);

MP_PROPERTY_GETTER(touchio_touchin_value_obj,
    (mp_obj_t)&touchio_touchin_get_value_obj);


//|     raw_value: int
//|     """The raw touch measurement as an `int`. (read-only)"""
static mp_obj_t touchio_touchin_obj_get_raw_value(mp_obj_t self_in) {
    touchio_touchin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_touchio_touchin_get_raw_value(self));
}

MP_DEFINE_CONST_FUN_OBJ_1(touchio_touchin_get_raw_value_obj, touchio_touchin_obj_get_raw_value);

MP_PROPERTY_GETTER(touchio_touchin_raw_value_obj,
    (mp_obj_t)&touchio_touchin_get_raw_value_obj);


//|     threshold: Optional[int]
//|     """Minimum `raw_value` needed to detect a touch (and for `value` to be `True`).
//|
//|     When the **TouchIn** object is created, an initial `raw_value` is read from the pin,
//|     and then `threshold` is set to be 100 + that value.
//|
//|     You can adjust `threshold` to make the pin more or less sensitive::
//|
//|       import board
//|       import touchio
//|
//|       touch = touchio.TouchIn(board.A1)
//|       touch.threshold = 7300"""
//|
static mp_obj_t touchio_touchin_obj_get_threshold(mp_obj_t self_in) {
    touchio_touchin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_touchio_touchin_get_threshold(self));
}

MP_DEFINE_CONST_FUN_OBJ_1(touchio_touchin_get_threshold_obj, touchio_touchin_obj_get_threshold);

static mp_obj_t touchio_touchin_obj_set_threshold(mp_obj_t self_in, mp_obj_t threshold_obj) {
    touchio_touchin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    uint32_t new_threshold = mp_obj_get_int(threshold_obj);
    mp_arg_validate_int_range(new_threshold, 0, UINT16_MAX, MP_QSTR_threshold);
    common_hal_touchio_touchin_set_threshold(self, new_threshold);
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_2(touchio_touchin_set_threshold_obj, touchio_touchin_obj_set_threshold);

MP_PROPERTY_GETSET(touchio_touchin_threshold_obj,
    (mp_obj_t)&touchio_touchin_get_threshold_obj,
    (mp_obj_t)&touchio_touchin_set_threshold_obj);


static const mp_rom_map_elem_t touchio_touchin_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&touchio_touchin___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&touchio_touchin_deinit_obj) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_value), MP_ROM_PTR(&touchio_touchin_value_obj)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_raw_value), MP_ROM_PTR(&touchio_touchin_raw_value_obj)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_threshold), MP_ROM_PTR(&touchio_touchin_threshold_obj)},
};

static MP_DEFINE_CONST_DICT(touchio_touchin_locals_dict, touchio_touchin_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    touchio_touchin_type,
    MP_QSTR_TouchIn,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, touchio_touchin_make_new,
    locals_dict, &touchio_touchin_locals_dict
    );
