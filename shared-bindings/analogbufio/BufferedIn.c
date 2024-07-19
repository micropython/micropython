// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Lee Atkinson, MeanStride Technology, Inc.
//
// SPDX-License-Identifier: MIT

#include <string.h>
#include "shared/runtime/context_manager_helpers.h"
#include "py/binary.h"
#include "py/mphal.h"
#include "py/nlr.h"
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
static mp_obj_t analogbufio_bufferedin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
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
static mp_obj_t analogbufio_bufferedin_deinit(mp_obj_t self_in) {
    analogbufio_bufferedin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_analogbufio_bufferedin_deinit(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(analogbufio_bufferedin_deinit_obj, analogbufio_bufferedin_deinit);

static void check_for_deinit(analogbufio_bufferedin_obj_t *self) {
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
static mp_obj_t analogbufio_bufferedin___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_analogbufio_bufferedin_deinit(args[0]);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(analogbufio_bufferedin___exit___obj, 4, 4, analogbufio_bufferedin___exit__);

//|     def readinto(self, buffer: WriteableBuffer, loop: bool = False) -> int:
//|         """Fills the provided buffer with ADC voltage values.
//|
//|         ADC values will be read into the given buffer at the supplied sample_rate.
//|         Depending on the buffer typecode, 'B', 'H', samples are 8-bit byte-arrays or
//|         16-bit half-words and are always unsigned.
//|         (See https://docs.circuitpython.org/en/latest/docs/library/array.html)
//|         For 8-bit samples, the most significant bits of the 12-bit ADC values are kept.
//|         For 16-bit samples, if loop=False, the 12-bit ADC values are scaled up to fill the 16 bit range.
//|         If loop=True, ADC values are stored without scaling.
//|
//|         :param ~circuitpython_typing.WriteableBuffer buffer: buffer: A buffer for samples
//|         :param ~bool loop: loop: Set to true for continuous conversions, False to fill buffer once then stop
//|         """
//|         ...
//|
static mp_obj_t analogbufio_bufferedin_obj_readinto(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer, ARG_loop };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer,    MP_ARG_OBJ | MP_ARG_REQUIRED, {} },
        { MP_QSTR_loop,      MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
    };
    analogbufio_bufferedin_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    mp_obj_t buffer = args[ARG_buffer].u_obj;
    // Buffer defined and allocated by user
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buffer, &bufinfo, MP_BUFFER_READ);
    uint8_t bytes_per_sample = 1;
    if (bufinfo.typecode == 'H') {
        bytes_per_sample = 2;
    } else if (bufinfo.typecode != 'B' && bufinfo.typecode != BYTEARRAY_TYPECODE) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q must be a bytearray or array of type 'H' or 'B'"), MP_QSTR_buffer);
    }
    mp_uint_t captured = common_hal_analogbufio_bufferedin_readinto(self, bufinfo.buf, bufinfo.len, bytes_per_sample, args[ARG_loop].u_bool);
    return MP_OBJ_NEW_SMALL_INT(captured);
}
MP_DEFINE_CONST_FUN_OBJ_KW(analogbufio_bufferedin_readinto_obj, 1, analogbufio_bufferedin_obj_readinto);

static const mp_rom_map_elem_t analogbufio_bufferedin_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__),    MP_ROM_PTR(&analogbufio_bufferedin_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),     MP_ROM_PTR(&analogbufio_bufferedin_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__),  MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__),   MP_ROM_PTR(&analogbufio_bufferedin___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto),   MP_ROM_PTR(&analogbufio_bufferedin_readinto_obj)},
};

static MP_DEFINE_CONST_DICT(analogbufio_bufferedin_locals_dict, analogbufio_bufferedin_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    analogbufio_bufferedin_type,
    MP_QSTR_BufferedIn,
    MP_TYPE_FLAG_NONE,
    make_new, analogbufio_bufferedin_make_new,
    locals_dict, &analogbufio_bufferedin_locals_dict
    );
