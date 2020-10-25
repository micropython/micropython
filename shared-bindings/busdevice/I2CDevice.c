/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft
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

// This file contains all of the Python API definitions for the
// busio.I2C class.

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/busdevice/I2CDevice.h"
#include "shared-bindings/util.h"
#include "shared-module/busdevice/I2CDevice.h"

#include "lib/utils/buffer_helper.h"
#include "lib/utils/context_manager_helpers.h"
#include "py/runtime.h"
#include "supervisor/shared/translate.h"

//| class I2CDevice:
//|     """Two wire serial protocol"""
//|
//|     def __init__(self, i2c, device_address, probe=True) -> None:
//|
//|         ...
//|
STATIC mp_obj_t busdevice_i2cdevice_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    busdevice_i2cdevice_obj_t *self = m_new_obj(busdevice_i2cdevice_obj_t);
    self->base.type = &busdevice_i2cdevice_type;
    enum { ARG_i2c, ARG_device_address, ARG_probe };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_device_address, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_probe, MP_ARG_BOOL, {.u_bool = true} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    busio_i2c_obj_t* i2c = args[ARG_i2c].u_obj;

    common_hal_busdevice_i2cdevice_construct(self, i2c, args[ARG_device_address].u_int, args[ARG_probe].u_bool);
    return (mp_obj_t)self;
}

//|     def __enter__(self) -> None:
//|         """Automatically initializes the hardware on context exit. See FIX
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
STATIC mp_obj_t busdevice_i2cdevice_obj___enter__(mp_obj_t self_in) {
    common_hal_busdevice_i2cdevice_lock(self_in);
    return self_in;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(busdevice_i2cdevice___enter___obj, busdevice_i2cdevice_obj___enter__);

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware on context exit. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
STATIC mp_obj_t busdevice_i2cdevice_obj___exit__(size_t n_args, const mp_obj_t *args) {
    common_hal_busdevice_i2cdevice_unlock(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(busdevice_i2cdevice___exit___obj, 4, 4, busdevice_i2cdevice_obj___exit__);

STATIC void readinto(busdevice_i2cdevice_obj_t *self, mp_obj_t buffer, int32_t start, mp_int_t end) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buffer, &bufinfo, MP_BUFFER_WRITE);

    size_t length = bufinfo.len;
    normalize_buffer_bounds(&start, end, &length);
    if (length == 0) {
        mp_raise_ValueError(translate("Buffer must be at least length 1"));
    }

    uint8_t status = common_hal_busdevice_i2cdevice_readinto(self, ((uint8_t*)bufinfo.buf) + start, length);
    if (status != 0) {
        mp_raise_OSError(status);
    }
}

STATIC mp_obj_t busdevice_i2cdevice_readinto(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer, ARG_start, ARG_end };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_start,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_end,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
    };
    
    busdevice_i2cdevice_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    readinto(self, args[ARG_buffer].u_obj, args[ARG_start].u_int, args[ARG_end].u_int);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(busdevice_i2cdevice_readinto_obj, 2, busdevice_i2cdevice_readinto);


STATIC void write(busdevice_i2cdevice_obj_t *self, mp_obj_t buffer, int32_t start, mp_int_t end) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buffer, &bufinfo, MP_BUFFER_READ);

    size_t length = bufinfo.len;
    normalize_buffer_bounds(&start, end, &length);
    if (length == 0) {
        mp_raise_ValueError(translate("Buffer must be at least length 1"));
    }

    uint8_t status = common_hal_busdevice_i2cdevice_write(self, ((uint8_t*)bufinfo.buf) + start, length);
    if (status != 0) {
        mp_raise_OSError(status);
    }
}

STATIC mp_obj_t busdevice_i2cdevice_write(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer, ARG_start, ARG_end };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_start,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_end,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
    };
    busdevice_i2cdevice_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    write(self, args[ARG_buffer].u_obj, args[ARG_start].u_int, args[ARG_end].u_int);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(busdevice_i2cdevice_write_obj, 2, busdevice_i2cdevice_write);


/*STATIC void write_then_readinto(busdevice_i2cdevice_obj_t *self, mp_obj_t out_buffer, mp_obj_t in_buffer, 
                                int32_t out_start, mp_int_t out_end, int32_t in_start, mp_int_t in_end) {
    mp_buffer_info_t out_bufinfo;
    mp_get_buffer_raise(out_buffer, &out_bufinfo, MP_BUFFER_READ);

    size_t out_length = out_bufinfo.len;
    normalize_buffer_bounds(&out_start, out_end, &out_length);
    if (out_length == 0) {
        mp_raise_ValueError(translate("Buffer must be at least length 1"));
    }

    mp_buffer_info_t in_bufinfo;
    mp_get_buffer_raise(in_buffer, &in_bufinfo, MP_BUFFER_WRITE);

    size_t in_length = in_bufinfo.len;
    normalize_buffer_bounds(&in_start, in_end, &in_length);
    if (in_length == 0) {
        mp_raise_ValueError(translate("Buffer must be at least length 1"));
    }

    uint8_t status = common_hal_busdevice_i2cdevice_write_then_readinto(self, out_buffer, in_buffer, out_length, in_length);
    if (status != 0) {
        mp_raise_OSError(status);
    }
}*/

STATIC mp_obj_t busdevice_i2cdevice_write_then_readinto(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_out_buffer, ARG_in_buffer, ARG_out_start, ARG_out_end, ARG_in_start, ARG_in_end };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_out_buffer,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_in_buffer,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_out_start,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_out_end,       MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
        { MP_QSTR_in_start,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_in_end,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
    };
    busdevice_i2cdevice_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    write(self, args[ARG_out_buffer].u_obj, args[ARG_out_start].u_int, args[ARG_out_end].u_int);
    
    readinto(self, args[ARG_in_buffer].u_obj, args[ARG_in_start].u_int, args[ARG_in_end].u_int);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(busdevice_i2cdevice_write_then_readinto_obj, 3, busdevice_i2cdevice_write_then_readinto);


STATIC mp_obj_t busdevice_i2cdevice___probe_for_device(mp_obj_t self_in) {
    //busdevice_i2cdevice_obj_t *self = self_in;

    //common_hal_busdevice_i2cdevice_lock(self_in);

/*
    uint8_t buffer;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(&buffer, &bufinfo, MP_BUFFER_WRITE);

    uint8_t status = common_hal_busdevice_i2cdevice_readinto(self_in, (uint8_t*)bufinfo.buf, 1);
    if (status != 0) {
        common_hal_busdevice_i2cdevice_unlock(self_in);
        mp_raise_ValueError_varg(translate("No I2C device at address: %x"), self->device_address);
    }
*/
    //common_hal_busdevice_i2cdevice_unlock(self_in);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(busdevice_i2cdevice___probe_for_device_obj, busdevice_i2cdevice___probe_for_device);


STATIC const mp_rom_map_elem_t busdevice_i2cdevice_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&busdevice_i2cdevice___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&busdevice_i2cdevice___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&busdevice_i2cdevice_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&busdevice_i2cdevice_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_then_readinto), MP_ROM_PTR(&busdevice_i2cdevice_write_then_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR___probe_for_device), MP_ROM_PTR(&busdevice_i2cdevice___probe_for_device_obj) },
};


STATIC MP_DEFINE_CONST_DICT(busdevice_i2cdevice_locals_dict, busdevice_i2cdevice_locals_dict_table);

const mp_obj_type_t busdevice_i2cdevice_type = {
   { &mp_type_type },
   .name = MP_QSTR_I2CDevice,
   .make_new = busdevice_i2cdevice_make_new,
   .locals_dict = (mp_obj_dict_t*)&busdevice_i2cdevice_locals_dict,
};