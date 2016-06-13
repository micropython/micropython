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
#include "ugfx.h"
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

typedef struct _pyb_ugfx_button_t {
    mp_obj_base_t base;
	
	GHandle ghButton;

} pyb_ugfx_button_obj_t;

/// \classmethod \constructor(parent, x, y, a, b, text, style)
///
/// Construct an Button object.
STATIC mp_obj_t pyb_ugfx_button_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 7, 7, false);

	
    const char *text = mp_obj_str_get_str(args[5]);
	int x = mp_obj_get_int(args[1]);
	int y = mp_obj_get_int(args[2]);
	int a = mp_obj_get_int(args[3]);
	int b = mp_obj_get_int(args[4]);
	
	GHandle parent = NULL;

	pyb_ugfx_container_obj_t *container = args[0];

	if (MP_OBJ_IS_TYPE(args[0], &pyb_ugfx_type)) {
		parent = NULL; //(default anyway)
	}
	else if (MP_OBJ_IS_TYPE(args[0], &pyb_ugfx_container_type)) {
		parent = container->ghContainer;
	}

    // create button object
    pyb_ugfx_button_obj_t *btn = m_new_obj(pyb_ugfx_button_obj_t);
    btn->base.type = &pyb_ugfx_button_type;
	
	
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
	wi.g.parent = parent;
	wi.text = text;

	// Create the actual button
	btn->ghButton = gwinButtonCreate(NULL, &wi);


	return btn;
}

/// \method destroy()
///
/// frees up all resources
STATIC mp_obj_t pyb_ugfx_button_destroy(mp_obj_t self_in) {
    pyb_ugfx_button_obj_t *self = self_in;
    
	gwinDestroy(self->ghButton);
	
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_ugfx_button_destroy_obj, pyb_ugfx_button_destroy);


/// \method attach_input(input)
///
STATIC mp_obj_t pyb_ugfx_button_attach_input(mp_obj_t self_in, mp_obj_t input) {
    pyb_ugfx_button_obj_t *self = self_in;
	
	int i = mp_obj_get_int(input);
	
	gwinAttachToggle(self->ghButton,0,i);
	
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_ugfx_button_attach_input_obj, pyb_ugfx_button_attach_input);



STATIC const mp_map_elem_t pyb_ugfx_button_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_destroy), (mp_obj_t)&pyb_ugfx_button_destroy_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_attach_input), (mp_obj_t)&pyb_ugfx_button_attach_input_obj },

	//class constants
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_RED),        MP_OBJ_NEW_SMALL_INT(Red) },


};

STATIC MP_DEFINE_CONST_DICT(pyb_ugfx_button_locals_dict, pyb_ugfx_button_locals_dict_table);

const mp_obj_type_t pyb_ugfx_button_type = {
    { &mp_type_type },
    .name = MP_QSTR_Button,
    .make_new = pyb_ugfx_button_make_new,
    .locals_dict = (mp_obj_t)&pyb_ugfx_button_locals_dict,
};







/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//////////////////       LIST     ///////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

typedef struct _pyb_ugfx_list_t {
    mp_obj_base_t base;
	
	GHandle ghList;

} pyb_ugfx_list_obj_t;

/// \classmethod \constructor(parent, x, y, a, b, text, style)
///
/// Construct an List object.
STATIC mp_obj_t pyb_ugfx_list_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 7, 7, false);

	
    const char *text = mp_obj_str_get_str(args[5]);
	int x = mp_obj_get_int(args[1]);
	int y = mp_obj_get_int(args[2]);
	int a = mp_obj_get_int(args[3]);
	int b = mp_obj_get_int(args[4]);
	
	GHandle parent = NULL;

	pyb_ugfx_container_obj_t *container = args[0];

	if (MP_OBJ_IS_TYPE(args[0], &pyb_ugfx_type)) {
		parent = NULL; //(default anyway)
	}
	else if (MP_OBJ_IS_TYPE(args[0], &pyb_ugfx_container_type)) {
		parent = container->ghContainer;
	}

    // create list object
    pyb_ugfx_list_obj_t *lst = m_new_obj(pyb_ugfx_list_obj_t);
    lst->base.type = &pyb_ugfx_list_type;
	
	
	//setup list options
	GWidgetInit	wi;

	// Apply some default values for GWIN
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;

	// Apply the list parameters	
	wi.g.width = a;
	wi.g.height = b;
	wi.g.y = y;
	wi.g.x = x;
	wi.g.parent = parent;
	wi.text = text;

	// Create the actual list
	lst->ghList = gwinListCreate(NULL, &wi, FALSE);   //FALSE - no multiselect
	
	//gwinListSetScroll(lst->ghList,scrollAlways);

	return lst;
}

/// \method destroy()
///
/// frees up all resources
STATIC mp_obj_t pyb_ugfx_list_destroy(mp_obj_t self_in) {
    pyb_ugfx_list_obj_t *self = self_in;
    
	gwinDestroy(self->ghList);
	
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_ugfx_list_destroy_obj, pyb_ugfx_list_destroy);


/// \method attach_input(input, function)
///
STATIC mp_obj_t pyb_ugfx_list_attach_input(mp_obj_t self_in, mp_obj_t input, mp_obj_t function) {
    pyb_ugfx_list_obj_t *self = self_in;
    int fun = mp_obj_get_int(function);	
	int i = mp_obj_get_int(input);
	
	gwinAttachToggle(self->ghList,fun,i);
	
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_ugfx_list_attach_input_obj, pyb_ugfx_list_attach_input);


/// \method add_item(input)
///
STATIC mp_obj_t pyb_ugfx_list_add_item(mp_obj_t self_in, mp_obj_t str) {
    pyb_ugfx_list_obj_t *self = self_in;
	
	const char *s = mp_obj_str_get_str(str);
	
	gwinListAddItem(self->ghList,s,TRUE);
	
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_ugfx_list_add_item_obj, pyb_ugfx_list_add_item);



STATIC const mp_map_elem_t pyb_ugfx_list_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_destroy), (mp_obj_t)&pyb_ugfx_list_destroy_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_attach_input), (mp_obj_t)&pyb_ugfx_list_attach_input_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_add_item), (mp_obj_t)&pyb_ugfx_list_add_item_obj },

	//class constants
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_RED),        MP_OBJ_NEW_SMALL_INT(Red) },


};

STATIC MP_DEFINE_CONST_DICT(pyb_ugfx_list_locals_dict, pyb_ugfx_list_locals_dict_table);

const mp_obj_type_t pyb_ugfx_list_type = {
    { &mp_type_type },
    .name = MP_QSTR_List,
    .make_new = pyb_ugfx_list_make_new,
    .locals_dict = (mp_obj_t)&pyb_ugfx_list_locals_dict,
};







/*

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////      CHECKBOX    //////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

typedef struct _pyb_ugfx_checkbox_t {
    mp_obj_base_t base;
	
	GHandle ghCheckbox;

} pyb_ugfx_checkbox_obj_t;

/// \classmethod \constructor(parent, x, y, a, b, text, style)
///
/// Construct an Button object.
STATIC mp_obj_t pyb_ugfx_checkbox_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 7, 7, false);

	
    const char *text = mp_obj_str_get_str(args[5]);
	int x = mp_obj_get_int(args[1]);
	int y = mp_obj_get_int(args[2]);
	int a = mp_obj_get_int(args[3]);
	int b = mp_obj_get_int(args[4]);
	
    // create button object
    pyb_ugfx_checkbox_obj_t *chk = m_new_obj(pyb_ugfx_checkbox_obj_t);
    chk->base.type = &pyb_ugfx_checkbox_type;
	
	
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
STATIC mp_obj_t pyb_ugfx_check_destroy(mp_obj_t self_in) {
    pyb_ugfx_checkbox_obj_t *self = self_in;
    
	gwinDestroy(self->ghCheckbox);
	
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_ugfx_check_destroy_obj, pyb_ugfx_check_destroy);


/// \method attach_input(pin)
///
/// attach a button to make button be pressable
STATIC mp_obj_t pyb_ugfx_check_attach_input(mp_obj_t self_in, mp_obj_t input) {
    pyb_ugfx_checkbox_obj_t *self = self_in;
	
	int i = mp_obj_get_int(input);
    
	gwinAttachToggle(self->ghCheckbox,0,i);
	
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_ugfx_check_attach_input_obj, pyb_ugfx_check_attach_input);



STATIC const mp_map_elem_t pyb_ugfx_checkbox_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_chk_destroy), (mp_obj_t)&pyb_ugfx_check_destroy_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_chk_attach_input), (mp_obj_t)&pyb_ugfx_check_attach_input_obj },

	//class constants
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_RED),        MP_OBJ_NEW_SMALL_INT(Red) },


};

STATIC MP_DEFINE_CONST_DICT(pyb_ugfx_checkbox_locals_dict, pyb_ugfx_checkbox_locals_dict_table);

const mp_obj_type_t pyb_ugfx_checkbox_type = {
    { &mp_type_type },
    .name = MP_QSTR_Checkbox,
    .make_new = pyb_ugfx_checkbox_make_new,
    .locals_dict = (mp_obj_t)&pyb_ugfx_checkbox_locals_dict,
};

*/


#endif // MICROPY_HW_HAS_UGFX
