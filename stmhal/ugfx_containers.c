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
//#include "ugfx_containers.h"
#include "ugfx.h"

#include "genhdr/pins.h"
#include "bufhelper.h"

/// \moduleref ugfx
/// UPDATE ME


/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
////////////////     Container    ///////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

//typedef struct _pyb_ugfx_container_t {
//    mp_obj_base_t base;
//	
//	GHandle ghContainer;
//
//} pyb_ugfx_container_obj_t;

/// \classmethod \constructor(x, y, a, b, text, {style})
///
/// Construct an Container object. Need to call .show() after creation
STATIC mp_obj_t pyb_ugfx_container_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 5, 6, false);


    const char *text = mp_obj_str_get_str(args[4]);
	int x = mp_obj_get_int(args[0]);
	int y = mp_obj_get_int(args[1]);
	int a = mp_obj_get_int(args[2]);
	int b = mp_obj_get_int(args[3]);

    // create container object
    pyb_ugfx_container_obj_t *ctr = m_new_obj(pyb_ugfx_container_obj_t);
    ctr->base.type = &pyb_ugfx_container_type;
	
	
	//setup container options
	GWidgetInit	wi;
 
	// Apply some default values for GWIN
	gwinWidgetClearInit(&wi);

	wi.g.show = FALSE;
 
	// Apply the container parameters	
	wi.g.width = a;
	wi.g.height = b;
	wi.g.y = y;
	wi.g.x = x;
	//wi.g.parent = ;
	wi.text = text;
	
	if (n_args == 6){
		pyb_ugfx_style_obj_t *st = args[5]; 
		if (MP_OBJ_IS_TYPE(args[5], &pyb_ugfx_style_type))
			wi.customStyle = &(st->style);
		else
			nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Requires a 'Style' object as input"));
	}
 
	// Create the actual container
	ctr->ghContainer = gwinContainerCreate(NULL, &wi, GWIN_CONTAINER_BORDER);
	

	return ctr;
}

/// \method destroy()
///
/// frees up all resources
STATIC mp_obj_t pyb_ugfx_container_destroy(mp_obj_t self_in) {
    pyb_ugfx_container_obj_t *self = self_in;
    
	gwinDestroy(self->ghContainer);
	
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_ugfx_container_destroy_obj, pyb_ugfx_container_destroy);

/// \method show()
///
/// shows the container and all its children
STATIC mp_obj_t pyb_ugfx_container_show(mp_obj_t self_in) {
    pyb_ugfx_container_obj_t *self = self_in;
    
        gwinShow(self->ghContainer);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_ugfx_container_show_obj, pyb_ugfx_container_show);

/// \method hide()
///
/// shows the container and all its children
STATIC mp_obj_t pyb_ugfx_container_hide(mp_obj_t self_in) {
    pyb_ugfx_container_obj_t *self = self_in;
    
        gwinHide(self->ghContainer);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_ugfx_container_hide_obj, pyb_ugfx_container_hide);

/*
/// \method set_style()
///
/// Sets the style used by the widget.
///
STATIC mp_obj_t pyb_ugfx_container_set_style(mp_obj_t self_in, mp_obj_t style_obj) {
	pyb_ugfx_style_obj_t *st = style_obj; 
	if (MP_OBJ_IS_TYPE(style_obj, &pyb_ugfx_style_type))
		gwinSetDefaultStyle(&(st->style));
	else
		nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Requires a 'Style' object as input"));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_ugfx_container_set_style_obj, pyb_ugfx_container_set_style);
*/



STATIC const mp_map_elem_t pyb_ugfx_container_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_destroy), (mp_obj_t)&pyb_ugfx_container_destroy_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_show), (mp_obj_t)&pyb_ugfx_container_show_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_hide), (mp_obj_t)&pyb_ugfx_container_hide_obj},
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_set_style), (mp_obj_t)&pyb_ugfx_container_set_style_obj},
	//class constants
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_RED),        MP_OBJ_NEW_SMALL_INT(Red) },


};

STATIC MP_DEFINE_CONST_DICT(pyb_ugfx_container_locals_dict, pyb_ugfx_container_locals_dict_table);

const mp_obj_type_t pyb_ugfx_container_type = {
    { &mp_type_type },
    .name = MP_QSTR_Container,
    .make_new = pyb_ugfx_container_make_new,
    .locals_dict = (mp_obj_t)&pyb_ugfx_container_locals_dict,
};


#endif // MICROPY_HW_HAS_UGFX
