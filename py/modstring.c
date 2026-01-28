/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Damien P. George
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

#if MICROPY_PY_TSTRINGS

static const mp_rom_map_elem_t mp_module_string_templatelib_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_string_dot_templatelib) },
    { MP_ROM_QSTR(MP_QSTR_Template), MP_ROM_PTR(&mp_type_template) },
    { MP_ROM_QSTR(MP_QSTR_Interpolation), MP_ROM_PTR(&mp_type_interpolation) },
};
static MP_DEFINE_CONST_DICT(mp_module_string_templatelib_globals, mp_module_string_templatelib_globals_table);

static const mp_obj_module_t mp_module_string_templatelib = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_string_templatelib_globals,
};

static const mp_rom_map_elem_t mp_module_string_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_string) },
    { MP_ROM_QSTR(MP_QSTR_templatelib), MP_ROM_PTR(&mp_module_string_templatelib) },
};
static MP_DEFINE_CONST_DICT(mp_module_string_globals, mp_module_string_globals_table);

const mp_obj_module_t mp_module_string = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_string_globals,
};

MP_REGISTER_MODULE(MP_QSTR_string, mp_module_string);

#endif // MICROPY_PY_TSTRINGS
