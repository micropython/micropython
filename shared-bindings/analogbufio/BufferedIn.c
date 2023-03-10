/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2022 Lee Atkinson, MeanStride Technology, Inc.
 *
 * TODO: Based on analogio/AnalogIn.c from Scott Shaw
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

#include <string.h>
#include "shared/runtime/context_manager_helpers.h"
#include "py/binary.h"
#include "py/mphal.h"
#include "py/nlr.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/analogbufio/BufferedIn.h"
#include "shared-bindings/util.h"

//| class BufferedIn:
//|     """Capture multiple analog voltage levels to the supplied buffer
//|
//|     Usage::
//|
//|         import board
//|         import analogbufio
//|         import array
//|
//|         length = 1000
//|         mybuffer = array.array("H", [0x0000] * length)
//|         rate = 500000
//|         adcbuf = analogbufio.BufferedIn(board.GP26, sample_rate=rate)
//|         adcbuf.readinto(mybuffer)
//|         adcbuf.deinit()
//|         for i in range(length):
//|             print(i, mybuffer[i])
//|
//|         (TODO) The reference voltage varies by platform so use
//|         ``reference_voltage`` to read the configured setting.
//|         (TODO) Provide mechanism to read CPU Temperature."""
//|

//|     def __init__(self, pin: microcontroller.Pin, *, sample_rate: int) -> None:
//|         """Create a `BufferedIn` on the given pin and given sample rate.
//|
//|         :param ~microcontroller.Pin pin: the pin to read from
//|         :param ~int sample_rate: rate: sampling frequency, in samples per second"""
//|         ...
STATIC mp_obj_t analogbufio_bufferedin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_pin, ARG_sample_rate };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pin,    MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_sample_rate, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Validate Pin
    const mcu_pin_obj_t *pin = validate_obj_is_free_pin(args[ARG_pin].u_obj, MP_QSTR_pin);

    // Create local object
    analogbufio_bufferedin_obj_t *self = m_new_obj_with_finaliser(analogbufio_bufferedin_obj_t);
    self->base.type = &analogbufio_bufferedin_type;

    // Call local interface in ports/common-hal/analogbufio
    common_hal_analogbufio_bufferedin_construct(self, pin, args[ARG_sample_rate].u_int);

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Shut down the `BufferedIn` and release the pin for other use."""
//|         ...
STATIC mp_obj_t analogbufio_bufferedin_deinit(mp_obj_t self_in) {
    analogbufio_bufferedin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_analogbufio_bufferedin_deinit(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(analogbufio_bufferedin_deinit_obj, analogbufio_bufferedin_deinit);

STATIC void check_for_deinit(analogbufio_bufferedin_obj_t *self) {
    if (common_hal_analogbufio_bufferedin_deinited(self)) {
        raise_deinited_error();
    }
}
//|     def __enter__(self) -> BufferedIn:
//|         """No-op used by Context Managers."""
//|         ...
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
STATIC mp_obj_t analogbufio_bufferedin___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_analogbufio_bufferedin_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(analogbufio_bufferedin___exit___obj, 4, 4, analogbufio_bufferedin___exit__);

//|     def readinto(self, buffer: WriteableBuffer) -> int:
//|         """Fills the provided buffer with ADC voltage values.
//|
//|         ADC values will be read into the given buffer at the supplied sample_rate.
//|         Depending on the buffer typecode, 'B', 'H', samples are 8-bit byte-arrays or
//|         16-bit half-words and are always unsigned.
//|         The ADC most significant bits of the ADC are kept. (See
//|         https://docs.circuitpython.org/en/latest/docs/library/array.html)
//|
//|         :param ~circuitpython_typing.WriteableBuffer buffer: buffer: A buffer for samples"""
//|         ...
//|
STATIC mp_obj_t analogbufio_bufferedin_obj_readinto(mp_obj_t self_in, mp_obj_t buffer_obj) {
    analogbufio_bufferedin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    // Buffer defined and allocated by user
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buffer_obj, &bufinfo, MP_BUFFER_READ);

    uint8_t bytes_per_sample = 1;

    // Bytes Per Sample
    if (bufinfo.typecode == 'H') {
        bytes_per_sample = 2;
    } else if (bufinfo.typecode != 'B' && bufinfo.typecode != BYTEARRAY_TYPECODE) {
        mp_raise_ValueError_varg(translate("%q must be a bytearray or array of type 'H' or 'B'"), MP_QSTR_buffer);
    }

    mp_uint_t captured = common_hal_analogbufio_bufferedin_readinto(self, bufinfo.buf, bufinfo.len, bytes_per_sample);
    return MP_OBJ_NEW_SMALL_INT(captured);
}
MP_DEFINE_CONST_FUN_OBJ_2(analogbufio_bufferedin_readinto_obj, analogbufio_bufferedin_obj_readinto);

STATIC const mp_rom_map_elem_t analogbufio_bufferedin_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__),    MP_ROM_PTR(&analogbufio_bufferedin_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),     MP_ROM_PTR(&analogbufio_bufferedin_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__),  MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__),   MP_ROM_PTR(&analogbufio_bufferedin___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto),       MP_ROM_PTR(&analogbufio_bufferedin_readinto_obj)},

};

STATIC MP_DEFINE_CONST_DICT(analogbufio_bufferedin_locals_dict, analogbufio_bufferedin_locals_dict_table);

const mp_obj_type_t analogbufio_bufferedin_type = {
    { &mp_type_type },
    .name = MP_QSTR_BufferedIn,
    .make_new = analogbufio_bufferedin_make_new,
    .locals_dict = (mp_obj_t)&analogbufio_bufferedin_locals_dict,
};
