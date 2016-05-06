/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Damien P. George
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

#include <stdio.h>
#include <stdint.h>

#include "py/obj.h"
#include "py/mphal.h"
#include "modpyb.h"
#include "esprcswitch.h"

STATIC mp_obj_t rcswitch_send(mp_obj_t pin_in, mp_obj_t value_in) {
    return mp_obj_new_bool(esp_rcswitch_send(mp_obj_get_pin(pin_in),mp_obj_get_int(value_in)));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(rcswitch_send_obj, rcswitch_send);

STATIC const mp_map_elem_t rcswitch_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_rcswitch) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR((mp_obj_t)&rcswitch_send_obj) }
};

STATIC MP_DEFINE_CONST_DICT(rcswitch_module_globals, rcswitch_module_globals_table);

const mp_obj_module_t rcswitch_module = {
    .base = { &mp_type_module },
    .name = MP_QSTR_rcswitch,
    .globals = (mp_obj_dict_t*)&rcswitch_module_globals,
};
