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

#include "py/runtime.h"
#include "board.h"
#include "modpyb.h"

typedef struct _pyb_switch_obj_t {
    mp_obj_base_t base;
} pyb_switch_obj_t;

STATIC const pyb_switch_obj_t pyb_switch_obj[] = {
    {{&pyb_switch_type}},
    {{&pyb_switch_type}},
};

#define NUM_SWITCH MP_ARRAY_SIZE(pyb_switch_obj)
#define SWITCH_ID(obj) ((obj) - &pyb_switch_obj[0] + 1)

void pyb_switch_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_switch_obj_t *self = self_in;
    mp_printf(print, "Switch(%u)", SWITCH_ID(self));
}

STATIC mp_obj_t pyb_switch_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    mp_int_t sw_id = mp_obj_get_int(args[0]);
    if (!(1 <= sw_id && sw_id <= NUM_SWITCH)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Switch %d does not exist"), sw_id);
    }
    return (mp_obj_t)&pyb_switch_obj[sw_id - 1];
}

mp_obj_t pyb_switch_value(mp_obj_t self_in) {
    pyb_switch_obj_t *self = self_in;
    return switch_get(SWITCH_ID(self)) ? mp_const_true : mp_const_false;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_switch_value_obj, pyb_switch_value);

mp_obj_t pyb_switch_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    return pyb_switch_value(self_in);
}

STATIC const mp_rom_map_elem_t pyb_switch_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&pyb_switch_value_obj) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_switch_locals_dict, pyb_switch_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    pyb_switch_type,
    MP_QSTR_Switch,
    MP_TYPE_FLAG_NONE,
    make_new, pyb_switch_make_new,
    print, pyb_switch_print,
    call, pyb_switch_call,
    locals_dict, &pyb_switch_locals_dict
    );
