/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Mark Komus
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
#include "shared-bindings/adafruit_bus_device/i2c_device/I2CDevice.h"
#include "shared-bindings/util.h"
#include "shared-module/adafruit_bus_device/i2c_device/I2CDevice.h"

#include "shared/runtime/buffer_helper.h"
#include "shared/runtime/context_manager_helpers.h"
#include "py/runtime.h"
#include "py/smallint.h"


//| class I2CDevice:
//|     """I2C Device Manager"""
//|
//|     def __init__(self, i2c: busio.I2C, device_address: int, probe: bool = True) -> None:
//|         """Represents a single I2C device and manages locking the bus and the device
//|         address.
//|
//|         :param ~busio.I2C i2c: The I2C bus the device is on
//|         :param int device_address: The 7 bit device address
//|         :param bool probe: Probe for the device upon object creation, default is true
//|
//|         Example::
//|
//|             import busio
//|             from board import *
//|             from adafruit_bus_device.i2c_device import I2CDevice
//|             with busio.I2C(SCL, SDA) as i2c:
//|                 device = I2CDevice(i2c, 0x70)
//|                 bytes_read = bytearray(4)
//|                 with device:
//|                     device.readinto(bytes_read)
//|                 # A second transaction
//|                 with device:
//|                     device.write(bytes_read)
//|         """
//|     ...
STATIC mp_obj_t adafruit_bus_device_i2cdevice_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    adafruit_bus_device_i2cdevice_obj_t *self =
        mp_obj_malloc(adafruit_bus_device_i2cdevice_obj_t, &adafruit_bus_device_i2cdevice_type);
    enum { ARG_i2c, ARG_device_address, ARG_probe };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_device_address, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_probe, MP_ARG_BOOL, {.u_bool = true} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t *i2c = args[ARG_i2c].u_obj;

    common_hal_adafruit_bus_device_i2cdevice_construct(MP_OBJ_TO_PTR(self), i2c, args[ARG_device_address].u_int);
    if (args[ARG_probe].u_bool == true) {
        common_hal_adafruit_bus_device_i2cdevice_probe_for_device(self);
    }

    return (mp_obj_t)self;
}

//|     def __enter__(self) -> I2CDevice:
//|         """Context manager entry to lock bus."""
//|         ...
STATIC mp_obj_t adafruit_bus_device_i2cdevice_obj___enter__(mp_obj_t self_in) {
    adafruit_bus_device_i2cdevice_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_adafruit_bus_device_i2cdevice_lock(self);
    return self;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(adafruit_bus_device_i2cdevice___enter___obj, adafruit_bus_device_i2cdevice_obj___enter__);

//|     def __exit__(self) -> None:
//|         """Automatically unlocks the bus on exit."""
//|         ...
STATIC mp_obj_t adafruit_bus_device_i2cdevice_obj___exit__(size_t n_args, const mp_obj_t *args) {
    common_hal_adafruit_bus_device_i2cdevice_unlock(MP_OBJ_TO_PTR(args[0]));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(adafruit_bus_device_i2cdevice___exit___obj, 4, 4, adafruit_bus_device_i2cdevice_obj___exit__);

//|     import sys
//|     def readinto(
//|         self, buffer: WriteableBuffer, *, start: int = 0, end: int = sys.maxsize
//|     ) -> None:
//|         """Read into ``buffer`` from the device.
//|
//|         If ``start`` or ``end`` is provided, then the buffer will be sliced
//|         as if ``buffer[start:end]`` were passed.
//|         The number of bytes read will be the length of ``buffer[start:end]``.
//|
//|         :param WriteableBuffer buffer: read bytes into this buffer
//|         :param int start: beginning of buffer slice
//|         :param int end: end of buffer slice; if not specified, use ``len(buffer)``
//|         """
//|         ...
STATIC mp_obj_t adafruit_bus_device_i2cdevice_readinto(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer, ARG_start, ARG_end };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer,     MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_start,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_end,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
    };

    adafruit_bus_device_i2cdevice_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t dest[8];
    uint8_t num_kws = 1;

    mp_load_method(self->i2c, MP_QSTR_readfrom_into, dest);
    dest[2] = MP_OBJ_NEW_SMALL_INT(self->device_address);
    dest[3] = args[ARG_buffer].u_obj;
    // dest[4] = mp_obj_new_str("start", 5);
    dest[4] = MP_OBJ_NEW_QSTR(MP_QSTR_start);
    dest[5] = MP_OBJ_NEW_SMALL_INT(args[ARG_start].u_int);
    if (args[ARG_end].u_int != INT_MAX) {
        dest[6] = MP_OBJ_NEW_QSTR(MP_QSTR_end);
        dest[7] = MP_OBJ_NEW_SMALL_INT(args[ARG_end].u_int);
        num_kws++;
    }
    mp_call_method_n_kw(2, num_kws, dest);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(adafruit_bus_device_i2cdevice_readinto_obj, 1, adafruit_bus_device_i2cdevice_readinto);

//|     import sys
//|     def write(self, buffer: ReadableBuffer, *, start: int = 0, end: int = sys.maxsize) -> None:
//|         """Write the bytes from ``buffer`` to the device, then transmit a stop bit.
//|
//|         If ``start`` or ``end`` is provided, then the buffer will be sliced
//|         as if ``buffer[start:end]`` were passed, but without copying the data.
//|         The number of bytes written will be the length of ``buffer[start:end]``.
//|
//|         :param ReadableBuffer buffer: write out bytes from this buffer
//|         :param int start: beginning of buffer slice
//|         :param int end: end of buffer slice; if not specified, use ``len(buffer)``
//|         """
//|         ...
STATIC mp_obj_t adafruit_bus_device_i2cdevice_write(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer, ARG_start, ARG_end };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer,     MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_start,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_end,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
    };
    adafruit_bus_device_i2cdevice_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t dest[8];
    uint8_t num_kws = 1;

    mp_load_method(self->i2c, MP_QSTR_writeto, dest);
    dest[2] = MP_OBJ_NEW_SMALL_INT(self->device_address);
    dest[3] = args[ARG_buffer].u_obj;
    dest[4] = MP_OBJ_NEW_QSTR(MP_QSTR_start);
    dest[5] = MP_OBJ_NEW_SMALL_INT(args[ARG_start].u_int);
    if (args[ARG_end].u_int != INT_MAX) {
        dest[6] = MP_OBJ_NEW_QSTR(MP_QSTR_end);
        dest[7] = MP_OBJ_NEW_SMALL_INT(args[ARG_end].u_int);
        num_kws++;
    }

    mp_call_method_n_kw(2, num_kws, dest);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(adafruit_bus_device_i2cdevice_write_obj, 1, adafruit_bus_device_i2cdevice_write);


//|     import sys
//|     def write_then_readinto(
//|         self,
//|         out_buffer: ReadableBuffer,
//|         in_buffer: WriteableBuffer,
//|         *,
//|         out_start: int = 0,
//|         out_end: int = sys.maxsize,
//|         in_start: int = 0,
//|         in_end: int = sys.maxsize
//|     ) -> None:
//|         """Write the bytes from ``out_buffer`` to the device, then immediately
//|         reads into ``in_buffer`` from the device.
//|
//|         If ``out_start`` or ``out_end`` is provided, then the buffer will be sliced
//|         as if ``out_buffer[out_start:out_end]`` were passed, but without copying the data.
//|         The number of bytes written will be the length of ``out_buffer[out_start:out_end]``.
//|
//|         If ``in_start`` or ``in_end`` is provided, then the input buffer will be sliced
//|         as if ``in_buffer[in_start:in_end]`` were passed,
//|         The number of bytes read will be the length of ``out_buffer[in_start:in_end]``.
//|
//|         :param ReadableBuffer out_buffer: write out bytes from this buffer
//|         :param WriteableBuffer in_buffer: read bytes into this buffer
//|         :param int out_start: beginning of ``out_buffer`` slice
//|         :param int out_end: end of ``out_buffer`` slice; if not specified, use ``len(out_buffer)``
//|         :param int in_start: beginning of ``in_buffer`` slice
//|         :param int in_end: end of ``in_buffer slice``; if not specified, use ``len(in_buffer)``
//|         """
//|         ...
//|
STATIC mp_obj_t adafruit_bus_device_i2cdevice_write_then_readinto(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_out_buffer, ARG_in_buffer, ARG_out_start, ARG_out_end, ARG_in_start, ARG_in_end };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_out_buffer,    MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_in_buffer,     MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_out_start,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_out_end,       MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
        { MP_QSTR_in_start,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_in_end,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
    };
    adafruit_bus_device_i2cdevice_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t dest[13];
    uint8_t num_kws = 2;
    uint8_t index = 2;

    mp_load_method(self->i2c, MP_QSTR_writeto_then_readfrom, dest);
    dest[index++] = MP_OBJ_NEW_SMALL_INT(self->device_address);
    dest[index++] = args[ARG_out_buffer].u_obj;
    dest[index++] = args[ARG_in_buffer].u_obj;
    dest[index++] = MP_OBJ_NEW_QSTR(MP_QSTR_out_start);
    dest[index++] = MP_OBJ_NEW_SMALL_INT(args[ARG_out_start].u_int);
    if (args[ARG_out_end].u_int != INT_MAX) {
        dest[index++] = MP_OBJ_NEW_QSTR(MP_QSTR_out_end);
        dest[index++] = MP_OBJ_NEW_SMALL_INT(args[ARG_out_end].u_int);
        num_kws++;
    }
    dest[index++] = MP_OBJ_NEW_QSTR(MP_QSTR_in_start);
    dest[index++] = MP_OBJ_NEW_SMALL_INT(args[ARG_in_start].u_int);
    if (args[ARG_in_end].u_int != INT_MAX) {
        dest[index++] = MP_OBJ_NEW_QSTR(MP_QSTR_in_end);
        dest[index++] = MP_OBJ_NEW_SMALL_INT(args[ARG_in_end].u_int);
        num_kws++;
    }

    mp_call_method_n_kw(3, num_kws, dest);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(adafruit_bus_device_i2cdevice_write_then_readinto_obj, 1, adafruit_bus_device_i2cdevice_write_then_readinto);

STATIC const mp_rom_map_elem_t adafruit_bus_device_i2cdevice_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&adafruit_bus_device_i2cdevice___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&adafruit_bus_device_i2cdevice___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&adafruit_bus_device_i2cdevice_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&adafruit_bus_device_i2cdevice_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_then_readinto), MP_ROM_PTR(&adafruit_bus_device_i2cdevice_write_then_readinto_obj) },
};

STATIC MP_DEFINE_CONST_DICT(adafruit_bus_device_i2cdevice_locals_dict, adafruit_bus_device_i2cdevice_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    adafruit_bus_device_i2cdevice_type,
    MP_QSTR_I2CDevice,
    MP_TYPE_FLAG_NONE,
    make_new, adafruit_bus_device_i2cdevice_make_new,
    locals_dict, &adafruit_bus_device_i2cdevice_locals_dict
    );
