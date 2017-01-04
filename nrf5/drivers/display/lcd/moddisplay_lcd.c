/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Glenn Ruben Bakke
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

#if MICROPY_PY_DISPLAY_LCD

STATIC const mp_map_elem_t lcd_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_lcd) },
#if 0
    { MP_OBJ_NEW_QSTR(MP_QSTR_sld10261p), (mp_obj_t)&lcd_sld10261p_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ssd1289), (mp_obj_t)&lcd_ssd1289_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ls027b7dh01), (mp_obj_t)&lcd_ls027b7dh01_type },
#endif
};

STATIC MP_DEFINE_CONST_DICT(lcd_module_globals, lcd_module_globals_table);

const mp_obj_module_t display_lcd_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&lcd_module_globals,
};

#endif // MICROPY_PY_DISPLAY_LCD
