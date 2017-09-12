/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
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
 #include "autoreload.h"
 #include "rgb_led_status.h"

//| :mod:`samd` --- SAMD implementation settings
//| =================================================
//|
//| .. module:: samd
//|   :synopsis: SAMD implementation settings
//|   :platform: SAMD21
//|

//| .. method:: enable_autoreload()
//|
//|   Enable autoreload based on USB file write activity.
//|
STATIC mp_obj_t samd_enable_autoreload(void) {
    autoreload_enable();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(samd_enable_autoreload_obj, samd_enable_autoreload);

//| .. method:: disable_autoreload()
//|
//|   Disable autoreload based on USB file write activity until
//|   `enable_autoreload` is called.
//|
STATIC mp_obj_t samd_disable_autoreload(void) {
    autoreload_disable();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(samd_disable_autoreload_obj, samd_disable_autoreload);

//| .. method:: set_rgb_status_brightness()
//|
//|   Set brightness of status neopixel from 0-255
//|   `set_rgb_status_brightness` is called.
//|
STATIC mp_obj_t samd_set_rgb_status_brightness(mp_obj_t lvl){
      // This must be int. If cast to uint8_t first, will never raise a ValueError.
      int brightness_int = mp_obj_get_int(lvl);
      if(brightness_int < 0 || brightness_int > 255){
            mp_raise_ValueError("Brightness must be between 0 and 255");
      }
      set_rgb_status_brightness((uint8_t)brightness_int);
      return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(samd_set_rgb_status_brightness_obj, samd_set_rgb_status_brightness);

STATIC const mp_rom_map_elem_t samd_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_samd) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_enable_autoreload),  MP_ROM_PTR(&samd_enable_autoreload_obj)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_disable_autoreload),  MP_ROM_PTR(&samd_disable_autoreload_obj)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_set_rgb_status_brightness),  MP_ROM_PTR(&samd_set_rgb_status_brightness_obj)},
};

STATIC MP_DEFINE_CONST_DICT(samd_module_globals, samd_module_globals_table);

const mp_obj_module_t samd_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&samd_module_globals,
};
