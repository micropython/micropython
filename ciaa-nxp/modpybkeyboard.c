/*
 * This file is part of the Micro Python project, http://micropython.org/
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
#include "ciaanxp_mphal.h"

typedef struct _pyb_keyboard_obj_t {
    mp_obj_base_t base;
    int rows;
    int columns;
} pyb_keyboard_obj_t;

STATIC pyb_keyboard_obj_t pyb_keyboard_obj[] = {
    {{&pyb_keyboard_type}}
};

#define KEYBOARD_ID(obj) ((obj) - &pyb_keyboard_obj[0])

void pyb_keyboard_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_keyboard_obj_t *self = self_in;
    mp_printf(print, "KEYBOARD (%u) Rows:%u Columns:%u", KEYBOARD_ID(self),self->rows,self->columns);
}

STATIC mp_obj_t pyb_keyboard_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 2, 2, false);
    mp_int_t rows = mp_obj_get_int(args[0]);
    mp_int_t columns = mp_obj_get_int(args[1]);

    pyb_keyboard_obj[0].rows = rows;
    pyb_keyboard_obj[0].columns = columns;

    mp_hal_initKeyboard((uint8_t)rows,(uint8_t)columns);

    return (mp_obj_t)&pyb_keyboard_obj[0];
}


uint8_t pyb_keyboard_getKey(pyb_keyboard_obj_t *self)
{
    uint8_t key = 0xFF;
    uint8_t r,c;
        for(r=0; r < self->rows; r++)
        {
                for(c=0; c < self->columns ;c++)
                {
                        int val = mp_hal_readMatrixKeyboard(r,c);
                        if(val==0)
                        {
                            key = r<<4 | c;
                            break;
                        }
                }
        }
    return key;
}


// get_matrix method : return current keyboard matrix state
mp_obj_t pyb_keyboard_get_matrix(mp_obj_t self_in) {
    pyb_keyboard_obj_t *self = self_in;

    uint8_t key = pyb_keyboard_getKey(self);
    return MP_OBJ_NEW_SMALL_INT(key);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_keyboard_get_matrix_obj, pyb_keyboard_get_matrix);


// get_char method : waits until a key is pressed and returns it
mp_obj_t pyb_keyboard_get_char(mp_obj_t self_in) {
    pyb_keyboard_obj_t *self = self_in;

    // wait key press
    uint8_t key=0xFF;
    while(key==0xFF)
	key = pyb_keyboard_getKey(self);

    // wait key relese
    uint8_t key2=key;
    while(key2!=0xFF)
	key2 = pyb_keyboard_getKey(self);

    mp_hal_milli_delay(1);

    return MP_OBJ_NEW_SMALL_INT(key);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_keyboard_get_char_obj, pyb_keyboard_get_char);



STATIC const mp_map_elem_t pyb_keyboard_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_matrix), (mp_obj_t)&pyb_keyboard_get_matrix_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_char), (mp_obj_t)&pyb_keyboard_get_char_obj },
};

STATIC MP_DEFINE_CONST_DICT(pyb_keyboard_locals_dict, pyb_keyboard_locals_dict_table);

const mp_obj_type_t pyb_keyboard_type = {
    { &mp_type_type },
    .name = MP_QSTR_Keyboard,
    .print = pyb_keyboard_print,
    .make_new = pyb_keyboard_make_new,
    .locals_dict = (mp_obj_t)&pyb_keyboard_locals_dict,
};
