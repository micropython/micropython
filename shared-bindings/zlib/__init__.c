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

#include "shared-bindings/zlib/__init__.h"

#include "supervisor/shared/translate/translate.h"

//| """zlib decompression functionality
//|
//| The `zlib` module allows limited functionality similar to the CPython zlib library.
//| This module allows to decompress binary data compressed with DEFLATE algorithm
//| (commonly used in zlib library and gzip archiver). Compression is not yet implemented."""

//| def zlib_decompress(
//|     data: bytes, wbits: Optional[int] = 0, bufsize: Optional[int] = 0
//| ) -> bytes:
//|     """Return decompressed *data* as bytes. *wbits* is DEFLATE dictionary window
//|     size used during compression (8-15, the dictionary size is power of 2 of
//|     that value). Additionally, if value is positive, *data* is assumed to be
//|     zlib stream (with zlib header). Otherwise, if it's negative, it's assumed
//|     to be raw DEFLATE stream.
//|
//|     The wbits parameter controls the size of the history buffer (or “window size”), and what header
//|     and trailer format is expected.
//|
//|     Common wbits values:
//|
//|     * To decompress deflate format, use wbits = -15
//|     * To decompress zlib format, use wbits = 15
//|     * To decompress gzip format, use wbits = 31
//|
//|     :param bytes data: data to be decompressed
//|     :param int wbits: DEFLATE dictionary window size used during compression. See above.
//|     :param int bufsize: ignored for compatibility with CPython only
//|     """
//|     ...
//|
STATIC mp_obj_t zlib_decompress(size_t n_args, const mp_obj_t *args) {
    bool is_zlib = true;
    if (n_args > 1 && MP_OBJ_SMALL_INT_VALUE(args[1]) < 0) {
        is_zlib = false;
    }

    return common_hal_zlib_decompress(args[0], is_zlib);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(zlib_decompress_obj, 1, 3, zlib_decompress);

STATIC const mp_rom_map_elem_t zlib_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_zlib) },
    { MP_ROM_QSTR(MP_QSTR_decompress), MP_ROM_PTR(&zlib_decompress_obj) },
};

STATIC MP_DEFINE_CONST_DICT(zlib_globals, zlib_globals_table);

const mp_obj_module_t zlib_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&zlib_globals,
};

MP_REGISTER_MODULE(MP_QSTR_zlib, zlib_module, CIRCUITPY_ZLIB);
