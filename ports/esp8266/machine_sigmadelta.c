/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Josef Gajdusek
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
#include <string.h>

#include "py/runtime.h"
#include "user_interface.h"

#include "sigmadelta.h"

const mp_obj_type_t pyb_sigmadelta_type;

typedef struct _pyb_sigmadelta_obj_t {
    mp_obj_base_t base;
} pyb_sigmadelta_obj_t;

STATIC pyb_sigmadelta_obj_t pyb_sigmadelta = {{&pyb_sigmadelta_type}};

STATIC mp_obj_t pyb_sigmadelta_make_new(
    const mp_obj_type_t *type_in,
    size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    //pyb_sigmadelta_obj_t *self = m_new_obj(pyb_sigmadelta_obj_t);
    //return MP_OBJ_FROM_PTR(self);
    return &pyb_sigmadelta;
}

/**********************************************************************/

STATIC mp_obj_t pyb_sigmadelta_start(mp_obj_t self_in) {
    //pyb_sigmadelta_obj_t *sd = self_in;
    sigmadelta_start();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_sigmadelta_start_obj, pyb_sigmadelta_start);

STATIC mp_obj_t pyb_sigmadelta_stop(mp_obj_t self_in) {
    //pyb_sigmadelta_obj_t *sd = self_in;
    sigmadelta_stop();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_sigmadelta_stop_obj, pyb_sigmadelta_stop);

STATIC mp_obj_t pyb_sigmadelta_setup(
    mp_obj_t self_in, mp_obj_t prescaler, mp_obj_t target) {
    //pyb_sigmadelta_obj_t *sd = self_in;
    sigmadelta_setup(
        (uint8_t)mp_obj_get_int(prescaler),
        (uint8_t)mp_obj_get_int(target));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_sigmadelta_setup_obj, pyb_sigmadelta_setup);

STATIC mp_obj_t pyb_sigmadelta_attach(mp_obj_t self_in, mp_obj_t pin) {
    //pyb_sigmadelta_obj_t *sd = self_in;
    sigmadelta_attach((uint8_t)mp_obj_get_int(pin));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_sigmadelta_attach_obj, pyb_sigmadelta_attach);

STATIC mp_obj_t pyb_sigmadelta_detach(mp_obj_t self_in, mp_obj_t pin) {
    //pyb_sigmadelta_obj_t *sd = self_in;
    sigmadelta_detach((uint8_t)mp_obj_get_int(pin));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_sigmadelta_detach_obj, pyb_sigmadelta_detach);

/**********************************************************************/

STATIC const mp_rom_map_elem_t pyb_sigmadelta_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&pyb_sigmadelta_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&pyb_sigmadelta_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_setup), MP_ROM_PTR(&pyb_sigmadelta_setup_obj) },
    { MP_ROM_QSTR(MP_QSTR_attach), MP_ROM_PTR(&pyb_sigmadelta_attach_obj) },
    { MP_ROM_QSTR(MP_QSTR_detach), MP_ROM_PTR(&pyb_sigmadelta_detach_obj) },
};
STATIC MP_DEFINE_CONST_DICT(pyb_sigmadelta_locals_dict, pyb_sigmadelta_locals_dict_table);

const mp_obj_type_t pyb_sigmadelta_type = {
    { &mp_type_type },
    .name = MP_QSTR_SigmaDelta,
    .make_new = pyb_sigmadelta_make_new,
    .locals_dict = (mp_obj_dict_t*)&pyb_sigmadelta_locals_dict,
};
