/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Mark Komus
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

#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/objarray.h"

#include "shared-bindings/is31fl3741/IS31FL3741.h"
#include "shared-bindings/util.h"
#include "shared-module/displayio/__init__.h"
#include "shared-bindings/busio/I2C.h"

//| class IS31FL3741:
//|     """Driver for an IS31FL3741 device."""
//|
//|     def __init__(self, i2c: busio.I2C, *, addr: int = 0x30) -> None:
//|         """Create a IS31FL3741 object with the given attributes.
//|
//|         Designed to work low level or passed to and object such as
//|         :class:`~is31fl3741.IS31FL3741_FrameBuffer`.
//|
//|         :param ~busio.I2C i2c: I2C bus the IS31FL3741 is on
//|         :param int addr: device address"""
//|         ...
//|
STATIC mp_obj_t is31fl3741_IS31FL3741_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_i2c, ARG_addr };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_addr, MP_ARG_INT | MP_ARG_KW_ONLY, { .u_int = 0x30 } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t i2c = mp_arg_validate_type(args[ARG_i2c].u_obj, &busio_i2c_type, MP_QSTR_i2c_bus);

    is31fl3741_IS31FL3741_obj_t *self = m_new_obj(is31fl3741_IS31FL3741_obj_t);
    self->base.type = &is31fl3741_IS31FL3741_type;

    common_hal_is31fl3741_IS31FL3741_construct(self,
        MP_OBJ_TO_PTR(i2c),
        args[ARG_addr].u_int
        );

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Free the resources associated with this
//|         IS31FL3741 instance.  After deinitialization, no further operations
//|         may be performed."""
//|         ...
//|
STATIC mp_obj_t is31fl3741_IS31FL3741_deinit(mp_obj_t self_in) {
    is31fl3741_IS31FL3741_obj_t *self = (is31fl3741_IS31FL3741_obj_t *)self_in;
    common_hal_is31fl3741_IS31FL3741_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(is31fl3741_IS31FL3741_deinit_obj, is31fl3741_IS31FL3741_deinit);

//| def reset(self) -> None:
//|     """Resets the IS31FL3741 chip."""
//|     ...
//|
STATIC mp_obj_t is31fl3741_IS31FL3741_reset(mp_obj_t self_in) {
    is31fl3741_IS31FL3741_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_is31fl3741_send_reset(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(is31fl3741_IS31FL3741_reset_obj, is31fl3741_IS31FL3741_reset);

//| def enable(self) -> None:
//|     """Enables the IS31FL3741 chip."""
//|     ...
//|
STATIC mp_obj_t is31fl3741_IS31FL3741_enable(mp_obj_t self_in) {
    is31fl3741_IS31FL3741_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_is31fl3741_send_enable(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(is31fl3741_IS31FL3741_enable_obj, is31fl3741_IS31FL3741_enable);

//| def set_global_current(self, current: int) -> None:
//|     """Sets the global current of the IS31FL3741 chip.
//|
//|     :param int current: global current value 0x00 to 0xFF"""
//|     ...
//|
STATIC mp_obj_t is31fl3741_IS31FL3741_set_global_current(mp_obj_t self_in, mp_obj_t value) {
    is31fl3741_IS31FL3741_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t current = mp_obj_get_int(value);
    common_hal_is31fl3741_set_current(self, current);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(is31fl3741_IS31FL3741_set_global_current_obj, is31fl3741_IS31FL3741_set_global_current);

//| def set_led(self, led: int, value: int, page: int) -> None:
//|     """Resets the IS31FL3741 chip.
//|
//|     :param int led: which LED to set
//|     :param int value: value to set the LED to 0x00 to 0xFF
//|     :param int page: page to write to 0 or 2. If the LED is a >= 180
//|       the routine will automatically write to page 1 or 3 (instead
//|       of 0 or 2)"""
//|     ...
//|
STATIC mp_obj_t is31fl3741_IS31FL3741_set_led(size_t n_args, const mp_obj_t *args) {
    is31fl3741_IS31FL3741_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t led = mp_obj_get_int(args[1]);
    mp_int_t value = mp_obj_get_int(args[2]);
    mp_int_t page = mp_obj_get_int(args[3]);
    common_hal_is31fl3741_set_led(self, led, value, page);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(is31fl3741_IS31FL3741_set_led_obj, 4, 4, is31fl3741_IS31FL3741_set_led);

//| def write(mapping: Tuple[int, ...], buf: ReadableBuffer) -> None:
//|     """Write buf out on the I2C bus to the IS31FL3741.
//|
//|     :param ~Tuple[int, ...] mapping: map the pixels in the buffer to the order addressed by the driver chip
//|     :param ~_typing.ReadableBuffer buf: The bytes to clock out. No assumption is made about color order"""
//|     ...
STATIC mp_obj_t is31fl3741_IS31FL3741_write(mp_obj_t self_in, mp_obj_t mapping, mp_obj_t buffer) {
    is31fl3741_IS31FL3741_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (!mp_obj_is_tuple_compatible(mapping)) {
        mp_raise_ValueError(translate("Mapping must be a tuple"));
    }

    mp_obj_t *map_items;
    size_t map_len;
    mp_obj_tuple_get(mapping, &map_len, &map_items);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buffer, &bufinfo, MP_BUFFER_READ);

    common_hal_is31fl3741_write(self, map_items, (uint8_t *)bufinfo.buf, bufinfo.len);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_3(is31fl3741_IS31FL3741_write_obj, is31fl3741_IS31FL3741_write);

STATIC const mp_rom_map_elem_t is31fl3741_IS31FL3741_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&is31fl3741_IS31FL3741_deinit_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_write), (mp_obj_t)&is31fl3741_IS31FL3741_write_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_reset), (mp_obj_t)&is31fl3741_IS31FL3741_reset_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_enable), (mp_obj_t)&is31fl3741_IS31FL3741_enable_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_set_global_current), (mp_obj_t)&is31fl3741_IS31FL3741_set_global_current_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_set_led), (mp_obj_t)&is31fl3741_IS31FL3741_set_led_obj },
};
STATIC MP_DEFINE_CONST_DICT(is31fl3741_IS31FL3741_locals_dict, is31fl3741_IS31FL3741_locals_dict_table);

const mp_obj_type_t is31fl3741_IS31FL3741_type = {
    { &mp_type_type },
    .name = MP_QSTR_is31fl3741,
    .locals_dict = (mp_obj_dict_t *)&is31fl3741_IS31FL3741_locals_dict,
    .make_new = is31fl3741_IS31FL3741_make_new,
};
