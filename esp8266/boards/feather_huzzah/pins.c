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

#include "common-hal/microcontroller/__init__.h"

STATIC const mp_map_elem_t board_global_dict_table[] = {
  { MP_OBJ_NEW_QSTR(MP_QSTR_ADC), (mp_obj_t)&pin_TOUT },
  { MP_OBJ_NEW_QSTR(MP_QSTR_GPIO16), (mp_obj_t)&pin_XPD_DCDC },
  { MP_OBJ_NEW_QSTR(MP_QSTR_GPIO14), (mp_obj_t)&pin_MTMS },
  { MP_OBJ_NEW_QSTR(MP_QSTR_SCK), (mp_obj_t)&pin_MTMS },
  { MP_OBJ_NEW_QSTR(MP_QSTR_GPIO12), (mp_obj_t)&pin_MTDI },
  { MP_OBJ_NEW_QSTR(MP_QSTR_MISO), (mp_obj_t)&pin_MTDI },
  { MP_OBJ_NEW_QSTR(MP_QSTR_GPIO13), (mp_obj_t)&pin_MTCK },
  { MP_OBJ_NEW_QSTR(MP_QSTR_MOSI), (mp_obj_t)&pin_MTCK },
  { MP_OBJ_NEW_QSTR(MP_QSTR_GPIO15), (mp_obj_t)&pin_MTDO },
  { MP_OBJ_NEW_QSTR(MP_QSTR_GPIO2), (mp_obj_t)&pin_GPIO2 },
  { MP_OBJ_NEW_QSTR(MP_QSTR_GPIO0), (mp_obj_t)&pin_GPIO0 },
  { MP_OBJ_NEW_QSTR(MP_QSTR_GPIO4), (mp_obj_t)&pin_GPIO4 },
  { MP_OBJ_NEW_QSTR(MP_QSTR_SDA), (mp_obj_t)&pin_GPIO4 },
  { MP_OBJ_NEW_QSTR(MP_QSTR_RX), (mp_obj_t)&pin_U0RXD },
  { MP_OBJ_NEW_QSTR(MP_QSTR_TX), (mp_obj_t)&pin_U0TXD },
  { MP_OBJ_NEW_QSTR(MP_QSTR_GPIO5), (mp_obj_t)&pin_DVDD },
  { MP_OBJ_NEW_QSTR(MP_QSTR_SCL), (mp_obj_t)&pin_DVDD },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_global_dict_table);
