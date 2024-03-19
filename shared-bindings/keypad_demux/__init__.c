/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 CDarius
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

#include "shared-bindings/keypad_demux/DemuxKeyMatrix.h"
#include "shared-bindings/util.h"

//| """Support for scanning key matrices that use a demultiplexer
//|
//| The `keypad_demux` module provides native support to scan sets of keys or buttons,
//| connected in a row-and-column matrix.
//|
//| .. jinja
//| """

STATIC mp_rom_map_elem_t keypad_demux_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),          MP_OBJ_NEW_QSTR(MP_QSTR_keypad_demux) },
    { MP_ROM_QSTR(MP_QSTR_DemuxKeyMatrix),    MP_OBJ_FROM_PTR(&keypad_demux_demuxkeymatrix_type) },
};

STATIC MP_DEFINE_CONST_DICT(keypad_demux_module_globals, keypad_demux_module_globals_table);

const mp_obj_module_t keypad_demux_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&keypad_demux_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_keypad_demux, keypad_demux_module);
