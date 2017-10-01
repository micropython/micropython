/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#include <string.h>

#include "lib/utils/context_manager_helpers.h"
#include "py/binary.h"
#include "py/mphal.h"
#include "py/nlr.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/touchio/TouchIn.h"

//| .. currentmodule:: touchio
//|
//| :class:`TouchIn` -- Read the state of a capacitive touch sensor
//| ===================================================================
//|
//| Usage::
//|
//|    import touchio
//|    from board import *
//|
//|    touch = touchio.TouchIn(A1)
//|    while True:
//|        if touch.value:
//|            print("touched!")
//|

//| .. class:: TouchIn(pin)
//|
//|   Use the TouchIn on the given pin.
//|
//|   :param ~microcontroller.Pin pin: the pin to read from
//|
STATIC mp_obj_t touchio_touchin_make_new(const mp_obj_type_t *type,
        mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check number of arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    // 1st argument is the pin
    mp_obj_t pin_obj = args[0];
    assert_pin(pin_obj, false);

    touchio_touchin_obj_t *self = m_new_obj(touchio_touchin_obj_t);
    self->base.type = &touchio_touchin_type;
    const mcu_pin_obj_t *pin = MP_OBJ_TO_PTR(pin_obj);
    common_hal_touchio_touchin_construct(self, pin);

    return (mp_obj_t) self;
}

//|   .. method:: deinit()
//|
//|      Deinitialises the TouchIn and releases any hardware resources for reuse.
//|
STATIC mp_obj_t touchio_touchin_deinit(mp_obj_t self_in) {
    touchio_touchin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_touchio_touchin_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(touchio_touchin_deinit_obj, touchio_touchin_deinit);

//|   .. method:: __enter__()
//|
//|      No-op used by Context Managers.
//|
//  Provided by context manager helper.

//|   .. method:: __exit__()
//|
//|      Automatically deinitializes the hardware when exiting a context. See
//|      :ref:`lifetime-and-contextmanagers` for more info.
//|
STATIC mp_obj_t touchio_touchin_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_touchio_touchin_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(touchio_touchin___exit___obj, 4, 4, touchio_touchin_obj___exit__);

//|   .. attribute:: value
//|
//|     Whether the touch pad is being touched or not.
//|     True if `raw_value` > `threshold`.
//|
//|     :return: True when touched, False otherwise.
//|     :rtype: bool
//|
STATIC mp_obj_t touchio_touchin_obj_get_value(mp_obj_t self_in) {
   touchio_touchin_obj_t *self = MP_OBJ_TO_PTR(self_in);
   return mp_obj_new_bool(common_hal_touchio_touchin_get_value(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(touchio_touchin_get_value_obj, touchio_touchin_obj_get_value);

const mp_obj_property_t touchio_touchin_value_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&touchio_touchin_get_value_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};


//|   .. attribute:: raw_value
//|
//|     The raw touch measurement. Not settable.
//|
//|     :return: an integer >= 0
//|     :rtype: int
//|
STATIC mp_obj_t touchio_touchin_obj_get_raw_value(mp_obj_t self_in) {
	touchio_touchin_obj_t *self = MP_OBJ_TO_PTR(self_in);
	return MP_OBJ_NEW_SMALL_INT(common_hal_touchio_touchin_get_raw_value(self));
}

MP_DEFINE_CONST_FUN_OBJ_1(touchio_touchin_get_raw_value_obj, touchio_touchin_obj_get_raw_value);

const mp_obj_property_t touchio_touchin_raw_value_obj = {
	.base.type = &mp_type_property,
	.proxy = {(mp_obj_t)&touchio_touchin_get_raw_value_obj,
                  (mp_obj_t)&mp_const_none_obj,
                  (mp_obj_t)&mp_const_none_obj},
 };


//|   .. attribute:: threshold
//|
//|     `value` will return True if `raw_value` is greater than than this threshold.
//|     When the **TouchIn** object is created, an initial `raw_value` is read from the pin,
//|     and then `threshold` is set to be 100 + that value.
//|
//|     You can set the threshold to a different value to make the pin more or less sensitive.
//|
//|     :return: an integer >= 0
//|     :rtype: int
//|
STATIC mp_obj_t touchio_touchin_obj_get_threshold(mp_obj_t self_in) {
	touchio_touchin_obj_t *self = MP_OBJ_TO_PTR(self_in);
	return MP_OBJ_NEW_SMALL_INT(common_hal_touchio_touchin_get_threshold(self));
}

MP_DEFINE_CONST_FUN_OBJ_1(touchio_touchin_get_threshold_obj, touchio_touchin_obj_get_threshold);

STATIC mp_obj_t touchio_touchin_obj_set_threshold(mp_obj_t self_in, mp_obj_t threshold_obj) {
	touchio_touchin_obj_t *self = MP_OBJ_TO_PTR(self_in);
        uint32_t new_threshold = mp_obj_get_int(threshold_obj);
        if (new_threshold < 0 || new_threshold > UINT16_MAX) {
            // I would use MP_STRINGIFY(UINT16_MAX), but that prints "0xffff" instead of 65536.
            mp_raise_ValueError("threshold must be in the range 0-65536");
        }
	common_hal_touchio_touchin_set_threshold(self, new_threshold);
        return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_2(touchio_touchin_set_threshold_obj, touchio_touchin_obj_set_threshold);

const mp_obj_property_t touchio_touchin_threshold_obj = {
	.base.type = &mp_type_property,
	.proxy = {(mp_obj_t)&touchio_touchin_get_threshold_obj,
                  (mp_obj_t)&touchio_touchin_set_threshold_obj,
                  (mp_obj_t)&mp_const_none_obj},
 };


STATIC const mp_rom_map_elem_t touchio_touchin_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&touchio_touchin___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&touchio_touchin_deinit_obj) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_value), MP_ROM_PTR(&touchio_touchin_value_obj)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_raw_value), MP_ROM_PTR(&touchio_touchin_raw_value_obj)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_threshold), MP_ROM_PTR(&touchio_touchin_threshold_obj)},
};

STATIC MP_DEFINE_CONST_DICT(touchio_touchin_locals_dict, touchio_touchin_locals_dict_table);

const mp_obj_type_t touchio_touchin_type = {
    { &mp_type_type },
    .name = MP_QSTR_TouchIn,
    .make_new = touchio_touchin_make_new,
    .locals_dict = (mp_obj_t)&touchio_touchin_locals_dict,
};
