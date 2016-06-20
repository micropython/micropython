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
#include "modugfx.h"

#include "genhdr/pins.h"
#include "bufhelper.h"

/// \moduleref ugfx
/// UPDATE ME


/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
////////////////       Font      ////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////



/// \classmethod \constructor("font name")
///
/// Construct an Font object.
STATIC mp_obj_t ugfx_font_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);


    const char *font_text = mp_obj_str_get_str(args[0]);

    // create font object
    ugfx_font_obj_t *fnt = m_new_obj(ugfx_font_obj_t);
    fnt->base.type = &ugfx_font_type;

	fnt->font = gdispOpenFont(font_text);
	if (fnt->font == NULL)
		nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Font '(%s)' does not exist", font_text));


	return fnt;
}

/// \method destroy()
///
/// frees up all resources
STATIC mp_obj_t ugfx_font_destroy(mp_obj_t self_in) {
    ugfx_font_obj_t *self = self_in;
    
	gdispCloseFont(self->font);
	
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_font_destroy_obj, ugfx_font_destroy);


STATIC const mp_map_elem_t ugfx_font_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_destroy), (mp_obj_t)&ugfx_font_destroy_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR___del__), (mp_obj_t)&ugfx_font_destroy_obj},
	//class constants
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_RED),        MP_OBJ_NEW_SMALL_INT(Red) },


};

STATIC MP_DEFINE_CONST_DICT(ugfx_font_locals_dict, ugfx_font_locals_dict_table);

const mp_obj_type_t ugfx_font_type = {
    { &mp_type_type },
    .name = MP_QSTR_Font,
    .make_new = ugfx_font_make_new,
    .locals_dict = (mp_obj_t)&ugfx_font_locals_dict,
};


/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
////////////////       Style      ///////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////



/// \classmethod \constructor({style_to_copy})
///
/// Construct an Style object. Can copy an inputted style, overwise 
///   initialises with a copy of the default style
STATIC mp_obj_t ugfx_style_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 1, false);


    // create style object
    ugfx_style_obj_t *sty = m_new_obj(ugfx_style_obj_t);
    sty->base.type = &ugfx_style_type;

	sty->style = *gwinGetDefaultStyle();
	if (n_args == 1){
		if (MP_OBJ_IS_TYPE(args[0], &ugfx_font_type)) {
			ugfx_style_obj_t *in_sty = args[0];
			sty->style =  in_sty->style;
		}
	}
	
	return sty;
}



/// \method set_disabled([text_colour, edge_colour, fill_colour, progress_colour])
///
/// set the colours to use when the widget is disabled 
STATIC mp_obj_t ugfx_style_set_disabled(mp_obj_t self_in, mp_obj_t colours) {
    ugfx_style_obj_t *self = self_in;
    
	mp_obj_t *items;
	mp_uint_t len;
	
	mp_obj_get_array(colours, &len, &items);
	
	if (len == 4){
		GColorSet cs = {
			mp_obj_get_int(items[0]),
			mp_obj_get_int(items[1]),
			mp_obj_get_int(items[2]),
			mp_obj_get_int(items[3])			
		};
		self->style.disabled = cs;
	}
	else
		nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Requires array of 4 colours"));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ugfx_style_set_disabled_obj, ugfx_style_set_disabled);


/// \method set_pressed([text_colour, edge_colour, fill_colour, progress_colour])
///
/// set the colours to use when the widget is pressed 
STATIC mp_obj_t ugfx_style_set_pressed(mp_obj_t self_in, mp_obj_t colours) {
    ugfx_style_obj_t *self = self_in;
    
	mp_obj_t *items;
	mp_uint_t len;
	
	mp_obj_get_array(colours, &len, &items);
	
	if (len == 4){
		GColorSet cs = {
			mp_obj_get_int(items[0]),
			mp_obj_get_int(items[1]),
			mp_obj_get_int(items[2]),
			mp_obj_get_int(items[3])			
		};
		self->style.pressed = cs;
	}
	else
		nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Requires array of 4 colours"));

	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ugfx_style_set_pressed_obj, ugfx_style_set_pressed);




/// \method set_enabled([text_colour, edge_colour, fill_colour, progress_colour])
///
/// set the colours to use when the widget is enabled 
STATIC mp_obj_t ugfx_style_set_enabled(mp_obj_t self_in, mp_obj_t colours) {
    ugfx_style_obj_t *self = self_in;
    
	mp_obj_t *items;
	mp_uint_t len;
	
	mp_obj_get_array(colours, &len, &items);
	
	if (len == 4){
		GColorSet cs = {
			mp_obj_get_int(items[0]),
			mp_obj_get_int(items[1]),
			mp_obj_get_int(items[2]),
			mp_obj_get_int(items[3])			
		};
		self->style.enabled = cs;
	}
	else
		nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Requires array of 4 colours"));

	
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ugfx_style_set_enabled_obj, ugfx_style_set_enabled);


/// \method set_focus(focus_colour)
///
/// set the focus colour
STATIC mp_obj_t ugfx_style_set_focus(mp_obj_t self_in, mp_obj_t colour) {
    ugfx_style_obj_t *self = self_in;

	self->style.focus = mp_obj_get_int(colour);
		
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ugfx_style_set_focus_obj, ugfx_style_set_focus);

/// \method set_background(background_colour)
///
/// set the background colour
STATIC mp_obj_t ugfx_style_set_background(mp_obj_t self_in, mp_obj_t colour) {
    ugfx_style_obj_t *self = self_in;

	self->style.background = mp_obj_get_int(colour);
		
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ugfx_style_set_background_obj, ugfx_style_set_background);



STATIC const mp_map_elem_t ugfx_style_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_set_enabled), (mp_obj_t)&ugfx_style_set_enabled_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_set_disabled), (mp_obj_t)&ugfx_style_set_disabled_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_set_pressed), (mp_obj_t)&ugfx_style_set_pressed_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_set_focus), (mp_obj_t)&ugfx_style_set_focus_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_set_background), (mp_obj_t)&ugfx_style_set_background_obj},
	//class constants
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_RED),        MP_OBJ_NEW_SMALL_INT(Red) },


};

STATIC MP_DEFINE_CONST_DICT(ugfx_style_locals_dict, ugfx_style_locals_dict_table);

const mp_obj_type_t ugfx_style_type = {
    { &mp_type_type },
    .name = MP_QSTR_Style,
    .make_new = ugfx_style_make_new,
    .locals_dict = (mp_obj_t)&ugfx_style_locals_dict,
};



#endif // MICROPY_HW_HAS_UGFX
