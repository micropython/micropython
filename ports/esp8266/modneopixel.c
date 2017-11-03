/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Hubert Denkmair
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

#include "py/mpconfig.h"
#if MICROPY_ESP8266_NEOPIXEL

#include "py/runtime.h"
#include "esp_mphal.h"
#include "espneopixel.h"

typedef struct {
    mp_obj_base_t base;
    pyb_pin_obj_t* pin;
    unsigned n;
    uint8_t bpp;
    size_t buf_size;
    uint8_t *buf;
} NeoPixel_obj_t;

STATIC const mp_obj_type_t NeoPixel_type;
STATIC mp_obj_t NeoPixel_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 2, 3, true);

    NeoPixel_obj_t *self = m_new_obj(NeoPixel_obj_t);
    self->base.type = &NeoPixel_type;
    self->pin = mp_obj_get_pin_obj(args[0]);
    self->n = mp_obj_get_int(args[1]);
    self->bpp = (n_args > 2) ? mp_obj_get_int(args[2]) : 3;
    self->buf_size = self->bpp * self->n;
    self->buf = (uint8_t*)m_new(uint8_t, self->buf_size);
    mp_hal_pin_output(self->pin->phys_port);

    return MP_OBJ_FROM_PTR(self);
}

STATIC void NeoPixel_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    NeoPixel_obj_t* self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<NeoPixel %p>", self->buf);
}

STATIC mp_obj_t NeoPixel_subscr(mp_obj_t self_in, mp_obj_t index_in, mp_obj_t val_in) {
    NeoPixel_obj_t* self = MP_OBJ_TO_PTR(self_in);
    size_t index = mp_obj_get_int(index_in);
    if (index >= self->n) {
        nlr_raise(mp_obj_new_exception(&mp_type_IndexError));
    }

    if (val_in == MP_OBJ_NULL) {
        // delete
        nlr_raise(mp_obj_new_exception(&mp_type_NotImplementedError));
    } else if (val_in == MP_OBJ_SENTINEL) {
        // read
        size_t offset = self->bpp * index;
        mp_obj_t tuple[4];
        tuple[1] = mp_obj_new_int(self->buf[offset++]); // g
        tuple[0] = mp_obj_new_int(self->buf[offset++]); // r
        tuple[2] = mp_obj_new_int(self->buf[offset++]); // b
        if (self->bpp > 3) {
            tuple[3] = mp_obj_new_int(self->buf[offset++]); // y
        }
        return mp_obj_new_tuple(self->bpp, tuple);
    } else {
        // write
        size_t val_len;
        mp_obj_t *val;
        mp_obj_get_array(val_in, &val_len, &val);

        if (val_len != self->bpp) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "__setitem__ expects a tuple the same length as bytes per pixel (%d)", val_len));
        }

        size_t offset = self->bpp * index;
        self->buf[offset++] = mp_obj_get_int(val[1]); // g
        self->buf[offset++] = mp_obj_get_int(val[0]); // r
        self->buf[offset++] = mp_obj_get_int(val[2]); // b
        if (self->bpp > 3) {
            self->buf[offset++] = mp_obj_get_int(val[3]); // y
        }
        return mp_const_none;
    }
}

STATIC mp_obj_t NeoPixel_fill(mp_obj_t self_in, mp_obj_t val_in) {
    NeoPixel_obj_t* self = MP_OBJ_TO_PTR(self_in);

    size_t val_len;
    mp_obj_t *val;
    mp_obj_get_array(val_in, &val_len, &val);
    if (val_len != self->bpp) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "fill expects a tuple the same length as bytes per pixel (%d)", val_len));
    }

    uint8_t r = mp_obj_get_int(val[0]); // r
    uint8_t g = mp_obj_get_int(val[1]); // g
    uint8_t b = mp_obj_get_int(val[2]); // b
    uint8_t y = (val_len>3) ? mp_obj_get_int(val[3]) : 0; // y

    size_t offset = 0;
    for (size_t i=0; i<self->n; i++) {
        self->buf[offset++] = g;
        self->buf[offset++] = r;
        self->buf[offset++] = b;
        if (self->bpp<4) { continue; }
        self->buf[offset++] = y;
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(NeoPixel_fill_obj, NeoPixel_fill);

STATIC mp_obj_t NeoPixel_write(mp_obj_t self_in) {
    NeoPixel_obj_t *self = MP_OBJ_TO_PTR(self_in);
    esp_neopixel_write(self->pin->phys_port, self->buf, self->n*self->bpp, true);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(NeoPixel_write_obj, NeoPixel_write);

STATIC mp_obj_t NeoPixel___del__(mp_obj_t self_in) {
    NeoPixel_obj_t *self = MP_OBJ_TO_PTR(self_in);
    m_del(uint8_t, self->buf, self->buf_size);
    self->buf = NULL;
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(NeoPixel___del__obj, NeoPixel___del__);

STATIC const mp_rom_map_elem_t NeoPixel_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_fill), MP_ROM_PTR(&NeoPixel_fill_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&NeoPixel_write_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&NeoPixel___del__obj) },
};
STATIC MP_DEFINE_CONST_DICT(NeoPixel_locals_dict, NeoPixel_locals_dict_table);

STATIC const mp_obj_type_t NeoPixel_type = {
    { &mp_type_type },
    .name = MP_QSTR_NeoPixel,
    .make_new = NeoPixel_make_new,
    .print = NeoPixel_print,
    .subscr = NeoPixel_subscr,
    .locals_dict = (mp_obj_dict_t*)&NeoPixel_locals_dict,
};

STATIC const mp_map_elem_t neopixel_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_neopixel) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_NeoPixel), (mp_obj_t)&NeoPixel_type },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_neopixel_globals, neopixel_globals_table);

const mp_obj_module_t mp_module_neopixel = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_neopixel_globals,
};

#endif // MICROPY_ESP8266_NEOPIXEL
