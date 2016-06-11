/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#include <math.h>

#include STM32_HAL_H

#include "py/nlr.h"
#include "py/runtime.h"

#if MICROPY_HW_HAS_UGFX

#include "pin.h"
#include "ugfx_widgets.h"
#include "gfx.h"

#include "genhdr/pins.h"
#include "bufhelper.h"

/// \moduleref ugfx
/// UPDATE ME


/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//////////////////      BUTTON    ///////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

typedef struct _ugfx_button_t {
    mp_obj_base_t base;
	
	GHandle ghButton;

} ugfx_button_obj_t;

/// \classmethod \constructor(parent, x, y, a, b, text, style)
///
/// Construct an Button object.
STATIC mp_obj_t ugfx_button_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 7, 7, false);

	
    const char *text = mp_obj_str_get_str(args[5]);
	int x = mp_obj_get_int(args[1]);
	int y = mp_obj_get_int(args[2]);
	int a = mp_obj_get_int(args[3]);
	int b = mp_obj_get_int(args[4]);
	
    // create button object
    ugfx_button_obj_t *btn = m_new_obj(ugfx_button_obj_t);
    btn->base.type = &ugfx_button_type;
	
	
	//setup button options
	GWidgetInit	wi;
 
	// Apply some default values for GWIN
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
 
	// Apply the button parameters	
	wi.g.width = a;
	wi.g.height = b;
	wi.g.y = y;
	wi.g.x = x;
	//wi.g.parent = ;
	wi.text = text;
 
	// Create the actual button
	btn->ghButton = gwinButtonCreate(NULL, &wi);
	

	return btn;
}

/// \method attach_input(pin)
///
/// attach a button to make button be pressable
STATIC mp_obj_t ugfx_button_destroy(mp_obj_t self_in,???) {
    pyb_lcd_obj_t *self = self_in;
    
	gwinDestroy(self->ghButton);
	
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_button_attach_input_obj, ugfx_button_attach_input);


/// \method destroy()
///
/// clears up all associated memory
STATIC mp_obj_t ugfx_button_attach_input(mp_obj_t self_in, mp_obj_t, input) {
    pyb_lcd_obj_t *self = self_in;
	
	int i = mp_obj_get_int(input);
	
	gwinAttachToggle(self->ghButton,0,i);
	
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_button_destroy_obj, ugfx_button_destroy);



STATIC const mp_map_elem_t ugfx_button_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_destroy), (mp_obj_t)&ugfx_button_destroy_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_attach_input), (mp_obj_t)&ugfx_button_attach_input_obj },

	//class constants
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_RED),        MP_OBJ_NEW_SMALL_INT(Red) },


};

STATIC MP_DEFINE_CONST_DICT(ugfx_button_locals_dict, ugfx_button_locals_dict_table);

const mp_obj_type_t ugfx_button_type = {
    { &mp_type_type },
    .name = MP_QSTR_Button,
    .make_new = ugfx_button_make_new,
    .locals_dict = (mp_obj_t)&ugfx_button_locals_dict,
};





/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////      CHECKBOX    //////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

typedef struct _ugfx_checkbox_t {
    mp_obj_base_t base;
	
	GHandle ghCheckbox;

} ugfx_checkbox_obj_t;

/// \classmethod \constructor(parent, x, y, a, b, text, style)
///
/// Construct an Button object.
STATIC mp_obj_t ugfx_checkbox_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 7, 7, false);

	
    const char *text = mp_obj_str_get_str(args[5]);
	int x = mp_obj_get_int(args[1]);
	int y = mp_obj_get_int(args[2]);
	int a = mp_obj_get_int(args[3]);
	int b = mp_obj_get_int(args[4]);
	
    // create button object
    ugfx_checkbox_obj_t *chk = m_new_obj(ugfx_checkbox_obj_t);
    chk->base.type = &ugfx_checkbox_type;
	
	
	//setup button options
	GWidgetInit	wi;
 
	// Apply some default values for GWIN
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
 
	// Apply the button parameters	
	wi.g.width = a;
	wi.g.height = b;
	wi.g.y = y;
	wi.g.x = x;
	//wi.g.parent = ;
	wi.text = text;
 
	// Create the actual button
	chk->ghCheckbox = gwinCheckboxCreate(NULL, &wi);
	

	return chk;
}

/// \method destroy()
///
/// clears up all associated memory
STATIC mp_obj_t ugfx_check_destroy(mp_obj_t self_in) {
    pyb_lcd_obj_t *self = self_in;
    
	gwinDestroy(self->ghCheckbox);
	
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_check_destroy_obj, ugfx_check_destroy);


/// \method attach_input(pin)
///
/// attach a button to make button be pressable
STATIC mp_obj_t ugfx_check_attach_input(mp_obj_t self_in, mp_obj_t, input) {
    pyb_lcd_obj_t *self = self_in;
	
	int i = mp_obj_get_int(input);
    
	gwinAttachToggle(self->ghCheckbox,0,i);
	
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_check_attach_input_obj, ugfx_check_attach_input);



STATIC const mp_map_elem_t ugfx_button_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_destroy), (mp_obj_t)&ugfx_check_destroy_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_attach_input), (mp_obj_t)&ugfx_check_attach_input_obj },

	//class constants
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_RED),        MP_OBJ_NEW_SMALL_INT(Red) },


};

STATIC MP_DEFINE_CONST_DICT(ugfx_checkbox_locals_dict, ugfx_checkbox_locals_dict_table);

const mp_obj_type_t ugfx_checkbox_type = {
    { &mp_type_type },
    .name = MP_QSTR_Checkbox,
    .make_new = ugfx_checkbox_make_new,
    .locals_dict = (mp_obj_t)&ugfx_checbox_locals_dict,
};




#endif // MICROPY_HW_HAS_UGFX
