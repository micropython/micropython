/*
 * This file is part of the Circuit Python project, https://github.com/adafruit/circuitpython
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Roy Hooper
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
#include "py/runtime.h"

#include "shared-bindings/_bit_transpose/__init__.h"

//| """A fast bit transposition function for parallel NeoPixel strips
//|
//| When driving multiple NeoPixel strips from a shift register, the bits
//| must be re-ordered in a specific way.  This module offers a low-level
//| routine for performing the transformation."""
//|

//| def bit_transpose(input: _typing.ReadableBuffer, *, output: Optional[_typing.WritableBuffer]=None):
//|     """Convert a sequence of 8*N pixel values into a single stream of bytes suitable for sending via a parallel conversion method (PioPixl8)
//|
//|     Returns the output buffer if specified (which must be big enough to hold the result), otherwise a freshly allocated buffer."""
//|     ...
//|
STATIC mp_obj_t bit_transpose(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_input, ARG_output };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_input, MP_ARG_OBJ | MP_ARG_REQUIRED, {} },
        { MP_QSTR_output, MP_ARG_OBJ | MP_ARG_KW_ONLY, { .u_obj = mp_const_none } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t input_bufinfo;
    mp_buffer_info_t output_bufinfo;

    mp_get_buffer_raise(args[ARG_input].u_obj, &input_bufinfo, MP_BUFFER_READ);
    int n = input_bufinfo.len;
    if (n % 8 != 0) {
        mp_raise_ValueError(translate("Input buffer must be a multiple of 8 bytes"));
    }
    mp_obj_t output = args[ARG_output].u_obj;

    if (!output || output == mp_const_none) {
        output = mp_obj_new_bytearray_of_zeros(n);
    }
    mp_get_buffer_raise(output, &output_bufinfo, MP_BUFFER_WRITE);
    int m = output_bufinfo.len;
    if (m < n) {
        mp_raise_ValueError(translate("Output buffer must be at least as big as input buffer"));
    }
    common_hal_bit_transpose_bit_transpose(output_bufinfo.buf, input_bufinfo.buf, input_bufinfo.len);
    return output;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(bit_transpose_bit_transpose_obj, 1, bit_transpose);

STATIC const mp_rom_map_elem_t bit_transpose_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__bit_transpose) },
    { MP_ROM_QSTR(MP_QSTR_bit_transpose), MP_ROM_PTR(&bit_transpose_bit_transpose_obj) },
};

STATIC MP_DEFINE_CONST_DICT(bit_transpose_module_globals, bit_transpose_module_globals_table);

const mp_obj_module_t bit_transpose_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&bit_transpose_module_globals,
};
