/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#include "py/builtin.h"

#if MICROPY_PY_IO

extern const mp_obj_type_t mp_type_fileio;
extern const mp_obj_type_t mp_type_textio;

STATIC const mp_rom_map_elem_t mp_module_io_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__io) },
    // Note: mp_builtin_open_obj should be defined by port, it's not
    // part of the core.
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&mp_builtin_open_obj) },
    #if MICROPY_PY_IO_FILEIO
    { MP_ROM_QSTR(MP_QSTR_FileIO), MP_ROM_PTR(&mp_type_fileio) },
    #if MICROPY_CPYTHON_COMPAT
    { MP_ROM_QSTR(MP_QSTR_TextIOWrapper), MP_ROM_PTR(&mp_type_textio) },
    #endif
    #endif
    { MP_ROM_QSTR(MP_QSTR_StringIO), MP_ROM_PTR(&mp_type_stringio) },
    #if MICROPY_PY_IO_BYTESIO
    { MP_ROM_QSTR(MP_QSTR_BytesIO), MP_ROM_PTR(&mp_type_bytesio) },
    #endif
};

STATIC MP_DEFINE_CONST_DICT(mp_module_io_globals, mp_module_io_globals_table);

const mp_obj_module_t mp_module_io = {
    .base = { &mp_type_module },
    .name = MP_QSTR__io,
    .globals = (mp_obj_dict_t*)&mp_module_io_globals,
};

#endif
