/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Jeff Epler for Adafruit Industries
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

#include "shared-bindings/floppyio/__init__.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "common-hal/floppyio/__init__.h"

#include <stdint.h>

#include "py/binary.h"
#include "py/obj.h"
#include "py/runtime.h"

//| def flux_readinto(
//|     buffer: WriteableBuffer, data: digitalio.DigitalInOut, index: digitalio.DigitalInOut
//| ) -> int:
//|     """Read flux transition information into the buffer.
//|
//|     The function returns when the buffer has filled, or when the index input
//|     indicates that one full revolution of data has been recorded.  Due to
//|     technical limitations, this process may not be interruptible by
//|     KeyboardInterrupt.
//|
//|     :param buffer: Read data into this buffer.  Each element represents the time between successive zero-to-one transitions.
//|     :param data: Pin on which the flux data appears
//|     :param index: Pin on which the index pulse appears
//|     :return: The actual number of bytes of read
//|     """
//|     ...
//|
STATIC mp_obj_t floppyio_flux_readinto(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer, ARG_data, ARG_index };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_data, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_index, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_WRITE);
    digitalio_digitalinout_obj_t *data = assert_digitalinout(args[ARG_data].u_obj);
    digitalio_digitalinout_obj_t *index = assert_digitalinout(args[ARG_index].u_obj);

    return MP_OBJ_NEW_SMALL_INT(common_hal_floppyio_flux_readinto(bufinfo.buf, bufinfo.len, data, index));
}
MP_DEFINE_CONST_FUN_OBJ_KW(floppyio_flux_readinto_obj, 0, floppyio_flux_readinto);

//| def mfm_readinto(
//|     buffer: WriteableBuffer, data: digitalio.DigitalInOut, index: digitalio.DigitalInOut
//| ) -> int:
//|     """Read mfm blocks into the buffer.
//|
//|     The track is assumed to consist of 512-byte sectors.
//|
//|     The function returns when all sectors have been successfully read, or
//|     a number of index pulses have occurred.  Due to technical limitations, this
//|     process may not be interruptible by KeyboardInterrupt.
//|
//|     :param buffer: Read data into this buffer.  Must be a multiple of 512.
//|     :param data: Pin on which the mfm data appears
//|     :param index: Pin on which the index pulse appears
//|     :return: The actual number of sectors read
//|     """
//|     ...
//|
STATIC mp_obj_t floppyio_mfm_readinto(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer, ARG_data, ARG_index };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_data, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_index, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_WRITE);
    digitalio_digitalinout_obj_t *data = assert_digitalinout(args[ARG_data].u_obj);
    digitalio_digitalinout_obj_t *index = assert_digitalinout(args[ARG_index].u_obj);

    if (bufinfo.len % 512 != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("Buffer must be a multiple of 512 bytes"));
    }
    size_t n_sectors = bufinfo.len / 512;
    return MP_OBJ_NEW_SMALL_INT(common_hal_floppyio_mfm_readinto(bufinfo.buf, n_sectors, data, index));
}
MP_DEFINE_CONST_FUN_OBJ_KW(floppyio_mfm_readinto_obj, 0, floppyio_mfm_readinto);

//| samplerate: int
//| """The approximate sample rate in Hz used by flux_readinto."""

STATIC const mp_rom_map_elem_t floppyio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_floppyio) },
    { MP_ROM_QSTR(MP_QSTR_flux_readinto), MP_ROM_PTR(&floppyio_flux_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_mfm_readinto), MP_ROM_PTR(&floppyio_mfm_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_samplerate), MP_ROM_INT(FLOPPYIO_SAMPLERATE) },
};
STATIC MP_DEFINE_CONST_DICT(floppyio_module_globals, floppyio_module_globals_table);

const mp_obj_module_t floppyio_module = {
    .base = {&mp_type_module },
    .globals = (mp_obj_dict_t *)&floppyio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_floppyio, floppyio_module);
