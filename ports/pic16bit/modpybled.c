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

typedef struct _pyb_led_obj_t {
    mp_obj_base_t base;
} pyb_led_obj_t;

STATIC const pyb_led_obj_t pyb_led_obj[] = {
    {{&pyb_led_type}},
    {{&pyb_led_type}},
    {{&pyb_led_type}},
};

#define NUM_LED MP_ARRAY_SIZE(pyb_led_obj)
#define LED_ID(obj) ((obj) - &pyb_led_obj[0] + 1)

void pyb_led_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_led_obj_t *self = self_in;
    mp_printf(print, "LED(%u)", LED_ID(self));
}

STATIC mp_obj_t pyb_led_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    mp_int_t led_id = mp_obj_get_int(args[0]);
    if (!(1 <= led_id && led_id <= NUM_LED)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("LED %d does not exist"), led_id);
    }
    return (mp_obj_t)&pyb_led_obj[led_id - 1];
}

mp_obj_t pyb_led_on(mp_obj_t self_in) {
    pyb_led_obj_t *self = self_in;
    led_state(LED_ID(self), 1);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_led_on_obj, pyb_led_on);

mp_obj_t pyb_led_off(mp_obj_t self_in) {
    pyb_led_obj_t *self = self_in;
    led_state(LED_ID(self), 0);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_led_off_obj, pyb_led_off);

mp_obj_t pyb_led_toggle(mp_obj_t self_in) {
    pyb_led_obj_t *self = self_in;
    led_toggle(LED_ID(self));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_led_toggle_obj, pyb_led_toggle);

STATIC const mp_rom_map_elem_t pyb_led_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&pyb_led_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&pyb_led_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_toggle), MP_ROM_PTR(&pyb_led_toggle_obj) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_led_locals_dict, pyb_led_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    pyb_led_type,
    MP_QSTR_LED,
    MP_TYPE_FLAG_NONE,
    make_new, pyb_led_make_new,
    print, pyb_led_print,
    locals_dict, &pyb_led_locals_dict
    );
