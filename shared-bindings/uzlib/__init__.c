/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Mark Komus
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
#include <assert.h>
#include <string.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "py/builtin.h"
#include "py/objtuple.h"
#include "py/binary.h"
#include "py/parsenum.h"

#include "shared-bindings/uzlib/DecompIO.h"

#include "supervisor/shared/translate.h"

#include "shared-bindings/uzlib/__init__.h"
#include "shared-bindings/uzlib/DecompIO.h"

STATIC mp_obj_t uzlib_decompress(size_t n_args, const mp_obj_t *args) {
    // TODO: Check number of args

    bool is_zlib = true;
    if (n_args > 1 && MP_OBJ_SMALL_INT_VALUE(args[1]) < 0) {
        is_zlib = false;
    }

    return common_hal_uzlib_decompress(args[0], is_zlib);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(uzlib_decompress_obj, 1, 3, uzlib_decompress);

STATIC const mp_rom_map_elem_t uzlib_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_uzlib) },
    { MP_ROM_QSTR(MP_QSTR_decompress), MP_ROM_PTR(&uzlib_decompress_obj) },
    { MP_ROM_QSTR(MP_QSTR_DecompIO), MP_ROM_PTR(&decompio_type) },
};

STATIC MP_DEFINE_CONST_DICT(uzlib_globals, uzlib_globals_table);

const mp_obj_module_t uzlib_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&uzlib_globals,
};

MP_REGISTER_MODULE(MP_QSTR_uzlib, uzlib_module, CIRCUITPY_UZLIB);
