/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Ernesto Gigliotti <ernestogigliotti@gmail.com>
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
#include "py/stream.h"
#include "board.h"
#include "modpyb.h"
#include "ciaanxp_mphal.h"

typedef struct _pyb_lcd_obj_t {
    mp_obj_base_t base;
    int lines;
} pyb_lcd_obj_t;

STATIC pyb_lcd_obj_t pyb_lcd_obj[] = {
    {{&pyb_lcd_type}}
};

#define LCD_ID(obj) ((obj) - &pyb_lcd_obj[0])

void pyb_lcd_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_lcd_obj_t *self = self_in;
    mp_printf(print, "LCD (%u) %u lines", LCD_ID(self),self->lines);
}

STATIC mp_obj_t pyb_lcd_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 2, 2, false);
    mp_int_t lines = mp_obj_get_int(args[0]);
    mp_int_t dotFormat = mp_obj_get_int(args[1]);

    pyb_lcd_obj[0].lines = lines;    

    mp_hal_initLCD((uint8_t)lines,(uint8_t)dotFormat);

    return (mp_obj_t)&pyb_lcd_obj[0];
}



// clear method 
mp_obj_t pyb_lcd_clear(mp_obj_t self_in) {    
    mp_hal_clearLCD();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_lcd_clear_obj, pyb_lcd_clear);


// goto_xy method 
mp_obj_t pyb_lcd_goto_xy(mp_obj_t self_in,mp_obj_t x,mp_obj_t y) {        
    mp_hal_gotoXYLCD(mp_obj_get_int(x),mp_obj_get_int(y));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_lcd_goto_xy_obj, pyb_lcd_goto_xy);

// config_cursor method 
mp_obj_t pyb_lcd_config_cursor(mp_obj_t self_in,mp_obj_t cursorOnOffObj,mp_obj_t blinkOnOffObj) {  

	uint8_t cursorOnOff=0;
	if(mp_obj_is_true(cursorOnOffObj))      
		cursorOnOff=1;
		
	uint8_t blinkOnOff=0;
	if(mp_obj_is_true(blinkOnOffObj))      
		blinkOnOff=1;

    mp_hal_configCursorLCD(cursorOnOff,blinkOnOff);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_lcd_config_cursor_obj, pyb_lcd_config_cursor);



// used by stream
STATIC mp_uint_t pyb_lcd_print_m(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    
    const char *buf = buf_in;

    // write the data
    mp_hal_printStringLCD(buf);
    return size;
}


STATIC const mp_map_elem_t pyb_lcd_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_clear), (mp_obj_t)&pyb_lcd_clear_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_write), (mp_obj_t)&mp_stream_write_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_goto_xy), (mp_obj_t)&pyb_lcd_goto_xy_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_config_cursor), (mp_obj_t)&pyb_lcd_config_cursor_obj },
};

STATIC MP_DEFINE_CONST_DICT(pyb_lcd_locals_dict, pyb_lcd_locals_dict_table);

STATIC const mp_stream_p_t lcd_stream_p = {
    .write = pyb_lcd_print_m,
    .is_text = true,
};

const mp_obj_type_t pyb_lcd_type = {
    { &mp_type_type },
    .name = MP_QSTR_LCD,
    .print = pyb_lcd_print,
    .make_new = pyb_lcd_make_new,
    .stream_p = &lcd_stream_p,
    .locals_dict = (mp_obj_t)&pyb_lcd_locals_dict,
};
