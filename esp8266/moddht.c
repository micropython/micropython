/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Seong-Woo Kim
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
#include "espdht.h"

STATIC mp_obj_t dht_read(mp_obj_t pin_in) {
    return MP_OBJ_NEW_SMALL_INT(esp_dht_read(mp_obj_get_pin(pin_in)));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(dht_read_obj, dht_read);

STATIC mp_obj_t dht_readhumi(void) {
    return mp_obj_new_float(esp_dht_readhumi());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(dht_readhumi_obj, dht_readhumi);

STATIC mp_obj_t dht_readtemp(void) {
    return mp_obj_new_float(esp_dht_readtemp());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(dht_readtemp_obj, dht_readtemp);

STATIC const mp_map_elem_t dht_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_dht) },

    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR((mp_obj_t)&dht_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readhumi), MP_ROM_PTR((mp_obj_t)&dht_readhumi_obj) },
    { MP_ROM_QSTR(MP_QSTR_readtemp), MP_ROM_PTR((mp_obj_t)&dht_readtemp_obj) },
};

STATIC MP_DEFINE_CONST_DICT(dht_module_globals, dht_module_globals_table);

const mp_obj_module_t dht_module = {
    .base = { &mp_type_module },
    .name = MP_QSTR_dht,
    .globals = (mp_obj_dict_t*)&dht_module_globals,
};
