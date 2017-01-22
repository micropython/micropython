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
#include "modmachine.h"
#include "esponewire.h"

STATIC mp_obj_t onewire_timings(mp_obj_t timings_in) {
    mp_obj_t *items;
    mp_obj_get_array_fixed_n(timings_in, 9, &items);
    for (int i = 0; i < 9; ++i) {
        esp_onewire_timings[i] = mp_obj_get_int(items[i]);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(onewire_timings_obj, onewire_timings);

STATIC mp_obj_t onewire_reset(mp_obj_t pin_in) {
    return mp_obj_new_bool(esp_onewire_reset(mp_obj_get_pin(pin_in)));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(onewire_reset_obj, onewire_reset);

STATIC mp_obj_t onewire_readbit(mp_obj_t pin_in) {
    return MP_OBJ_NEW_SMALL_INT(esp_onewire_readbit(mp_obj_get_pin(pin_in)));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(onewire_readbit_obj, onewire_readbit);

STATIC mp_obj_t onewire_readbyte(mp_obj_t pin_in) {
    uint pin = mp_obj_get_pin(pin_in);
    uint8_t value = 0;
    for (int i = 0; i < 8; ++i) {
        value |= esp_onewire_readbit(pin) << i;
    }
    return MP_OBJ_NEW_SMALL_INT(value);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(onewire_readbyte_obj, onewire_readbyte);

STATIC mp_obj_t onewire_writebit(mp_obj_t pin_in, mp_obj_t value_in) {
    esp_onewire_writebit(mp_obj_get_pin(pin_in), mp_obj_get_int(value_in));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(onewire_writebit_obj, onewire_writebit);

STATIC mp_obj_t onewire_writebyte(mp_obj_t pin_in, mp_obj_t value_in) {
    uint pin = mp_obj_get_pin(pin_in);
    int value = mp_obj_get_int(value_in);
    for (int i = 0; i < 8; ++i) {
        esp_onewire_writebit(pin, value & 1);
        value >>= 1;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(onewire_writebyte_obj, onewire_writebyte);

STATIC mp_obj_t onewire_crc8(mp_obj_t data) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data, &bufinfo, MP_BUFFER_READ);
    uint8_t crc = 0;
    for (size_t i = 0; i < bufinfo.len; ++i) {
        uint8_t byte = ((uint8_t*)bufinfo.buf)[i];
        for (int b = 0; b < 8; ++b) {
            uint8_t fb_bit = (crc ^ byte) & 0x01;
            if (fb_bit == 0x01) {
                crc = crc ^ 0x18;
            }
            crc = (crc >> 1) & 0x7f;
            if (fb_bit == 0x01) {
                crc = crc | 0x80;
            }
            byte = byte >> 1;
        }
    }
    return MP_OBJ_NEW_SMALL_INT(crc);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(onewire_crc8_obj, onewire_crc8);

STATIC const mp_map_elem_t onewire_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_onewire) },

    { MP_ROM_QSTR(MP_QSTR_timings), MP_ROM_PTR((mp_obj_t)&onewire_timings_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR((mp_obj_t)&onewire_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_readbit), MP_ROM_PTR((mp_obj_t)&onewire_readbit_obj) },
    { MP_ROM_QSTR(MP_QSTR_readbyte), MP_ROM_PTR((mp_obj_t)&onewire_readbyte_obj) },
    { MP_ROM_QSTR(MP_QSTR_writebit), MP_ROM_PTR((mp_obj_t)&onewire_writebit_obj) },
    { MP_ROM_QSTR(MP_QSTR_writebyte), MP_ROM_PTR((mp_obj_t)&onewire_writebyte_obj) },
    { MP_ROM_QSTR(MP_QSTR_crc8), MP_ROM_PTR((mp_obj_t)&onewire_crc8_obj) },
};

STATIC MP_DEFINE_CONST_DICT(onewire_module_globals, onewire_module_globals_table);

const mp_obj_module_t onewire_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&onewire_module_globals,
};
