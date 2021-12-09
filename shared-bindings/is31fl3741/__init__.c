/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#include "shared-bindings/is31fl3741/__init__.h"

#include <stdint.h>
#include <stdbool.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/busio/I2C.h"
#include "shared-bindings/is31fl3741/IS31FL3741.h"


//| """Low-level is31fl3741 implementation
//|
//| The `is31fl3741_write` module contains a helper method to write out bytes in
//| over the I2C bus.
//|
//| def is31fl3741_write(i2c: busio.I2c, addr: int, mapping: Tuple[int, ...], buf: ReadableBuffer) -> None:
//|     """Write buf out on the given I2C bus.
//|
//|     :param ~busio.I2C i2c: the I2C bus to output with
//|     :param ~int addr: the I2C address of the IS31FL3741 device
//|     :param ~Tuple[int, ...] mapping: map the pixels in the buffer to the order addressed by the driver chip
//|     :param ~_typing.ReadableBuffer buf: The bytes to clock out. No assumption is made about color order"""
//|     ...
STATIC mp_obj_t is31fl3741_is31fl3741_write(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_i2c, ARG_addr, ARG_mapping, ARG_buffer };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c,     MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_addr,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0x30 } },
        { MP_QSTR_mapping, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_buffer,  MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t i2c = mp_arg_validate_type(args[ARG_i2c].u_obj, &busio_i2c_type, MP_QSTR_i2c_bus);

    if (!mp_obj_is_tuple_compatible(args[ARG_mapping].u_obj)) {
        mp_raise_ValueError(translate("Mapping must be a tuple"));
    }

    mp_obj_t *map_items;
    size_t map_len;
    mp_obj_tuple_get(args[ARG_mapping].u_obj, &map_len, &map_items);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_READ);

    common_hal_is31fl3741_write(i2c, args[ARG_addr].u_int, map_items, (uint8_t *)bufinfo.buf, bufinfo.len);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(is31fl3741_is31fl3741_write_obj, 0, is31fl3741_is31fl3741_write);

//| def is31fl3741_init(i2c: busio.I2c, addr: int) -> None:
//|     """Initialize the IS31FL3741 device.
//|
//|     :param ~busio.I2C i2c: the I2C bus to output with
//|     :param ~int addr: the I2C address of the IS31FL3741 device
//|     ...
STATIC mp_obj_t is31fl3741_is31fl3741_init(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_i2c, ARG_addr };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c,     MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_addr,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0x30 } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t i2c = mp_arg_validate_type(args[ARG_i2c].u_obj, &busio_i2c_type, MP_QSTR_i2c_bus);

    common_hal_is31fl3741_init(i2c, args[ARG_addr].u_int);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(is31fl3741_is31fl3741_init_obj, 0, is31fl3741_is31fl3741_init);

STATIC const mp_rom_map_elem_t is31fl3741_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_is31fl3741) },
    { MP_ROM_QSTR(MP_QSTR_IS31FL3741), MP_ROM_PTR(&is31fl3741_IS31FL3741_type) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_is31fl3741_init), (mp_obj_t)&is31fl3741_is31fl3741_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_is31fl3741_write), (mp_obj_t)&is31fl3741_is31fl3741_write_obj },
};

STATIC MP_DEFINE_CONST_DICT(is31fl3741_module_globals, is31fl3741_module_globals_table);

const mp_obj_module_t is31fl3741_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&is31fl3741_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_is31fl3741, is31fl3741_module, CIRCUITPY_IS31FL3741);
