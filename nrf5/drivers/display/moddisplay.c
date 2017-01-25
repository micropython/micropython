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

#if MICROPY_PY_DISPLAY

#include "epaper_sld00200p_obj.h"
#include "lcd_ili9341_obj.h"
#include "lcd_ls0xxb7dxxx_obj.h"
#include "lcd_ssd1289_obj.h"
#include "oled_ssd1305_obj.h"
#include "oled_ssd1306_obj.h"

STATIC const mp_map_elem_t mp_module_display_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_display) },
#if MICROPY_PY_DISPLAY_EPAPER_SLD00200P
    { MP_OBJ_NEW_QSTR(MP_QSTR_SLD00200P), (mp_obj_t)&epaper_sld00200p_type },
#endif
#if MICROPY_PY_DISPLAY_LCD_ILI9341
    { MP_OBJ_NEW_QSTR(MP_QSTR_ILI9341), (mp_obj_t)&lcd_ili9341_type },
#endif
#if MICROPY_PY_DISPLAY_LCD_LS0XXB7DXXX
    { MP_OBJ_NEW_QSTR(MP_QSTR_LS0XXB7DXXX), (mp_obj_t)&lcd_ls0xxb7dxxx_type },
#endif
#if MICROPY_PY_DISPLAY_LCD_SSD1289
    { MP_OBJ_NEW_QSTR(MP_QSTR_SSD1289), (mp_obj_t)&lcd_ssd1289_type },
#endif
#if MICROPY_PY_DISPLAY_OLED_SSD1305
    { MP_OBJ_NEW_QSTR(MP_QSTR_SSD1305), (mp_obj_t)&oled_ssd1305_type },
#endif
#if MICROPY_PY_DISPLAY_OLED_SSD1306
    { MP_OBJ_NEW_QSTR(MP_QSTR_SSD1306), (mp_obj_t)&oled_ssd1306_type },
#endif
};


STATIC MP_DEFINE_CONST_DICT(mp_module_display_globals, mp_module_display_globals_table);

const mp_obj_module_t mp_module_display = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_display_globals,
};

#endif // MICROPY_PY_DISPLAY
