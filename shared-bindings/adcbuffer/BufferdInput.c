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
#include "shared-bindings/adcbuffer/BufferedInput.h"
#include "shared-bindings/util.h"

/// class BufferedInput:
///     """Input analog voltage level to supplied buffer using DMA Capture"""
///
///     def __init__(self, pin: microcontroller.Pin, buffer: WriteableBuffer, *, sample_rate: int = 500000) -> None:
///         """Use the BufferedInput on the given pin. Fill the given buffer from ADC read values at the supplied
///         sample_rate.
///
///         :param ~microcontroller.Pin pin: the pin to read from"""
///         :param ~circuitpython_typing.WriteableBuffer buffer: buffer: A buffer for samples
///         :param ~int sample_rate: rate: The desired playback sample rate
///
///     Usage::
///
///       import board
///       import adcbuffer
///       import array
///
///       length = 1000
///       mybuffer = array.array("H", [0] * length)
///       rate = 500000
///       adcbuf = adcbuffer.BufferedInput(board.GP26, mybuffer, rate)
///       adcbuf.readmultiple()
///       adcbuf.deinit()
///       for i in range(length):
///           print(i, mybuffer[i])
///
///       (TODO) The reference voltage varies by platform so use ``reference_voltage`` to read the configured setting.
///       (TODO) Provide mechanism to read CPU Temperature
///       """
///         ...
///
STATIC void validate_rate(mp_float_t rate) {
    if (rate < (mp_float_t)1.0f || rate > (mp_float_t)500000.0f) {
        mp_raise_ValueError(translate("sample rate must be 1.0-500000.0 per second"));
    }
}

STATIC mp_obj_t adcbuffer_bufferedinput_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_pin, ARG_buffer, ARG_sample_rate };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pin,    MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_buffer, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_sample_rate, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_int = 500000} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Validate Pin
    const mcu_pin_obj_t *pin = validate_obj_is_free_pin(args[ARG_pin].u_obj);

    // Buffer Pointer defined and allocated by user
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_READ);

    // signed or unsigned, byte per sample
    bool signed_samples = bufinfo.typecode == 'b' || bufinfo.typecode == 'h';
    uint8_t bytes_per_sample = 1;

    // Bytes Per Sample
    if (bufinfo.typecode == 'h' || bufinfo.typecode == 'H') {
        bytes_per_sample = 2;
    } else if (bufinfo.typecode != 'b' && bufinfo.typecode != 'B' && bufinfo.typecode != BYTEARRAY_TYPECODE) {
        mp_raise_ValueError(translate("sample_source buffer must be a bytearray or array of type 'h', 'H', 'b' or 'B'"));
    }

    // Validate sample rate here
    mp_float_t sample_rate = mp_obj_get_float(args[ARG_sample_rate].u_obj);
    validate_rate(sample_rate);

    // Create local object
    adcbuffer_bufferedinput_obj_t *self = m_new_obj(adcbuffer_bufferedinput_obj_t);
    self->base.type = &adcbuffer_bufferedinput_type;

    // Call local intereface in ports/common-hal/adcbuffer
    common_hal_adcbuffer_bufferedinput_construct(self,
        pin,
        ((uint8_t *)bufinfo.buf),
        bufinfo.len,
        bytes_per_sample,
        signed_samples,
        sample_rate
        );

    return MP_OBJ_FROM_PTR(self);
}

///     def deinit(self) -> None:
///         """Turn off the BufferedInput and release the pin for other use."""
///         ...
///
STATIC mp_obj_t adcbuffer_bufferedinput_deinit(mp_obj_t self_in) {
    adcbuffer_bufferedinput_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_adcbuffer_bufferedinput_deinit(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(adcbuffer_bufferedinput_deinit_obj, adcbuffer_bufferedinput_deinit);

STATIC void check_for_deinit(adcbuffer_bufferedinput_obj_t *self) {
    if (common_hal_adcbuffer_bufferedinput_deinited(self)) {
        raise_deinited_error();
    }
}
///     def __enter__(self) -> BufferedInput:
///         """No-op used by Context Managers."""
///         ...
///
///  Provided by context manager helper.
///
///     def __exit__(self) -> None:
///         """Automatically deinitializes the hardware when exiting a context. See
///         :ref:`lifetime-and-contextmanagers` for more info."""
///         ...
///
STATIC mp_obj_t adcbuffer_bufferedinput___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_adcbuffer_bufferedinput_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(adcbuffer_bufferedinput___exit___obj, 4, 4, adcbuffer_bufferedinput___exit__);

///     value: int
///     """The value on the analog pin between 0 and 65535 inclusive (16-bit). (read-only)
///
///     Even if the underlying analog to digital converter (ADC) is lower
///     resolution, the value is 16-bit."""
///
STATIC mp_obj_t adcbuffer_bufferedinput_obj_readmultiple(mp_obj_t self_in) {
    adcbuffer_bufferedinput_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_adcbuffer_bufferedinput_readmultiple(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(adcbuffer_bufferedinput_readmultiple_obj, adcbuffer_bufferedinput_obj_readmultiple);

/// MP_PROPERTY_GETTER(adcbuffer_bufferedinput_value_obj,
///    (mp_obj_t)&adcbuffer_bufferedinput_get_value_obj);
///
///     reference_voltage: float
///     """The maximum voltage measurable (also known as the reference voltage) as a
///     `float` in Volts.  Note the ADC value may not scale to the actual voltage linearly
///     at ends of the analog range."""
///

STATIC const mp_rom_map_elem_t adcbuffer_bufferedinput_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit),             MP_ROM_PTR(&adcbuffer_bufferedinput_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__),          MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__),           MP_ROM_PTR(&adcbuffer_bufferedinput___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_readmultiple),       MP_ROM_PTR(&adcbuffer_bufferedinput_readmultiple_obj)},

};

STATIC MP_DEFINE_CONST_DICT(adcbuffer_bufferedinput_locals_dict, adcbuffer_bufferedinput_locals_dict_table);

const mp_obj_type_t adcbuffer_bufferedinput_type = {
    { &mp_type_type },
    .name = MP_QSTR_BufferedInput,
    .make_new = adcbuffer_bufferedinput_make_new,
    .locals_dict = (mp_obj_t)&adcbuffer_bufferedinput_locals_dict,
};
