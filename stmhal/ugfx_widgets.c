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
#include "gfx.h"

#include "genhdr/pins.h"
#include "bufhelper.h"

/// \moduleref ugfx
/// UPDATE ME



/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//////////////////      COMMON    ///////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

static GHandle get_ugfx_handle(mp_obj_t in);

/// \method text()
///
/// Gets or sets widget text
STATIC mp_obj_t ugfx_widget_text(mp_uint_t n_args, const mp_obj_t *args) {
	
	GHandle gh = get_ugfx_handle(args[0]);
	
	if (n_args == 1){
		const char * s = gwinGetText(gh);
		return mp_obj_new_str(s, strlen(s), TRUE);
	}
	else
	{
		const char *s = mp_obj_str_get_str(args[1]);
		gwinSetText(gh, s, TRUE);
		return mp_const_none;
	}
	   
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_widget_text_obj, 1, 2, ugfx_widget_text);



/// \method attach_input(input, function)
///
STATIC mp_obj_t ugfx_widget_attach_input(mp_obj_t self_in, mp_obj_t input, mp_obj_t function) {
    int fun = mp_obj_get_int(function);
	int i = mp_obj_get_int(input);

	gwinAttachToggle(get_ugfx_handle(self_in),fun,i);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(ugfx_widget_attach_input_obj, ugfx_widget_attach_input);


/// \method detach_input(function)
///
STATIC mp_obj_t ugfx_widget_detach_input(mp_obj_t self_in, mp_obj_t function) {
    int fun = mp_obj_get_int(function);

	gwinDetachToggle(get_ugfx_handle(self_in),fun);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ugfx_widget_detach_input_obj, ugfx_widget_detach_input);



/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//////////////////      BUTTON    ///////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

typedef struct _ugfx_button_t {
    mp_obj_base_t base;

	GHandle ghButton;

} ugfx_button_obj_t;

/// \classmethod \constructor(x, y, a, b, text, {parent})
///
/// Construct an Button object.
/// Will take the style from the parent, if the parents style is set. Otherwise uses default style
STATIC mp_obj_t ugfx_button_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 5, 6, false);


    const char *text = mp_obj_str_get_str(args[4]);
	int x = mp_obj_get_int(args[0]);
	int y = mp_obj_get_int(args[1]);
	int a = mp_obj_get_int(args[2]);
	int b = mp_obj_get_int(args[3]);

	GHandle parent = NULL;


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
	wi.text = text;

	if (n_args > 5){
		ugfx_container_obj_t *container = args[5];

		if (MP_OBJ_IS_TYPE(args[5], &ugfx_container_type)) {
			parent = container->ghContainer;
			wi.customStyle = container->style;
		}
	}
	wi.g.parent = parent;

	// Create the actual button
	btn->ghButton = gwinButtonCreate(NULL, &wi);


	return btn;
}

/// \method destroy()
///
/// frees up all resources
STATIC mp_obj_t ugfx_button_destroy(mp_obj_t self_in) {
    ugfx_button_obj_t *self = self_in;

	gwinDestroy(self->ghButton);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_button_destroy_obj, ugfx_button_destroy);



STATIC const mp_map_elem_t ugfx_button_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_destroy), (mp_obj_t)&ugfx_button_destroy_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR___del__), (mp_obj_t)&ugfx_button_destroy_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_attach_input), (mp_obj_t)&ugfx_widget_attach_input_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_detach_input), (mp_obj_t)&ugfx_widget_detach_input_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_text), (mp_obj_t)&ugfx_widget_text_obj },

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
//////////////////     TEXTBOX    ///////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

typedef struct _ugfx_textbox_t {
    mp_obj_base_t base;

	GHandle ghTextbox;

} ugfx_textbox_obj_t;

/// \classmethod \constructor(x, y, a, b, text, {parent})
///
/// Construct an Textbox object.
/// Will take the style from the parent, if the parents style is set. Otherwise uses default style
STATIC mp_obj_t ugfx_textbox_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 5, 6, false);


 //   const char *text = mp_obj_str_get_str(args[4]);
	int x = mp_obj_get_int(args[0]);
	int y = mp_obj_get_int(args[1]);
	int a = mp_obj_get_int(args[2]);
	int b = mp_obj_get_int(args[3]);
	int maxlen = mp_obj_get_int(args[4]);

	GHandle parent = NULL;


    // create textbox object
    ugfx_textbox_obj_t *btn = m_new_obj(ugfx_textbox_obj_t);
    btn->base.type = &ugfx_textbox_type;


	//setup textbox options
	GWidgetInit	wi;

	// Apply some default values for GWIN
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;

	// Apply the textbox parameters
	wi.g.width = a;
	wi.g.height = b;
	wi.g.y = y;
	wi.g.x = x;
//	wi.text = text;

	if (n_args > 5){
		ugfx_container_obj_t *container = args[5];

		if (MP_OBJ_IS_TYPE(args[5], &ugfx_container_type)) {
			parent = container->ghContainer;
			wi.customStyle = container->style;
		}
	}
	wi.g.parent = parent;

	// Create the actual textbox
	btn->ghTextbox = gwinTexteditCreate(NULL, &wi, maxlen);

	return btn;
}


/// \method destroy()
///
/// frees up all resources
STATIC mp_obj_t ugfx_textbox_destroy(mp_obj_t self_in) {
    ugfx_textbox_obj_t *self = self_in;

	gwinDestroy(self->ghTextbox);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_textbox_destroy_obj, ugfx_textbox_destroy);


STATIC const mp_map_elem_t ugfx_textbox_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_destroy), (mp_obj_t)&ugfx_textbox_destroy_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR___del__), (mp_obj_t)&ugfx_textbox_destroy_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_attach_input), (mp_obj_t)&ugfx_widget_attach_input_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_detach_input), (mp_obj_t)&ugfx_widget_detach_input_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_text), (mp_obj_t)&ugfx_widget_text_obj },

	//class constants
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_RED),        MP_OBJ_NEW_SMALL_INT(Red) },


};

STATIC MP_DEFINE_CONST_DICT(ugfx_textbox_locals_dict, ugfx_textbox_locals_dict_table);

const mp_obj_type_t ugfx_textbox_type = {
    { &mp_type_type },
    .name = MP_QSTR_Textbox,
    .make_new = ugfx_textbox_make_new,
    .locals_dict = (mp_obj_t)&ugfx_textbox_locals_dict,
};






/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//////////////////       LIST     ///////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

typedef struct _ugfx_list_t {
    mp_obj_base_t base;

	GHandle ghList;

} ugfx_list_obj_t;

/// \classmethod \constructor(x, y, a, b, *, parent=None, up=ugfx.JOY_UP, down=ugfx.JOY_DOWN)
///
/// Construct an List object.
/// Will take the style from the parent, if the parents style is set. Otherwise uses default style

STATIC const mp_arg_t ugfx_list_make_new_args[] = {
    { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_a, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_b, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_parent, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_up, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = GINPUT_TOGGLE_UP} },
    { MP_QSTR_down, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = GINPUT_TOGGLE_DOWN} },
};
#define UGFX_LIST_MAKE_NEW_NUM_ARGS MP_ARRAY_SIZE(ugfx_list_make_new_args)

STATIC mp_obj_t ugfx_list_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_val_t vals[UGFX_LIST_MAKE_NEW_NUM_ARGS];
    mp_arg_parse_all_kw_array(n_args, n_kw, args, UGFX_LIST_MAKE_NEW_NUM_ARGS, ugfx_list_make_new_args, vals);

    // create list object
    ugfx_list_obj_t *lst = m_new_obj(ugfx_list_obj_t);
    lst->base.type = &ugfx_list_type;

	//setup list options
	GWidgetInit	wi;

	// Apply some default values for GWIN
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;

	// Apply the list parameters
	wi.g.width = vals[2].u_int;
	wi.g.height = vals[3].u_int;
	wi.g.y = vals[1].u_int;
	wi.g.x = vals[0].u_int;
	wi.text = 0;

    // Apply parent
    wi.g.parent = NULL;
	if (MP_OBJ_IS_TYPE(vals[4].u_obj, &ugfx_container_type)) {
        ugfx_container_obj_t *container = vals[4].u_obj;
		wi.g.parent = container->ghContainer;
		wi.customStyle = container->style;
	}

	// Create the actual list
	lst->ghList = gwinListCreate(NULL, &wi, FALSE);   //FALSE - no multiselect

	//gwinListSetScroll(lst->ghList,scrollAlways);

    // attach default inputs
    gwinAttachToggle(lst->ghList, 1, vals[5].u_int);
    gwinAttachToggle(lst->ghList, 0, vals[6].u_int); //mp_obj_get_int(args[6])

	return lst;
}

/// \method destroy()
///
/// frees up all resources
STATIC mp_obj_t ugfx_list_destroy(mp_obj_t self_in) {
    ugfx_list_obj_t *self = self_in;

	gwinDestroy(self->ghList);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_list_destroy_obj, ugfx_list_destroy);


/// \method add_item(input)
///
STATIC mp_obj_t ugfx_list_add_item(mp_obj_t self_in, mp_obj_t str) {
    ugfx_list_obj_t *self = self_in;

	const char *s = mp_obj_str_get_str(str);

	gwinListAddItem(self->ghList,s,TRUE);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ugfx_list_add_item_obj, ugfx_list_add_item);



/// \method assign_image(index, img_obj)
///
STATIC mp_obj_t ugfx_list_assign_image(mp_obj_t self_in, mp_obj_t index_in, mp_obj_t image_in) {
    ugfx_list_obj_t *self = self_in;

	int i = mp_obj_get_int(index_in);
	if (image_in == mp_const_none){
		gwinListItemSetImage(self->ghList,i,0);
	}
	else if (!MP_OBJ_IS_TYPE(image_in, &ugfx_image_type)) {
		nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "img argument needs to be be a Image type or NULL"));
		return mp_const_none;
	}
	else
	{
		ugfx_image_obj_t *image = image_in;
		gwinListItemSetImage(self->ghList,i,&(image->thisImage));
	}
	
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(ugfx_list_assign_image_obj, ugfx_list_assign_image);

/// \method remove_item(index)
///
STATIC mp_obj_t ugfx_list_remove_item(mp_obj_t self_in, mp_obj_t index) {
    ugfx_list_obj_t *self = self_in;
	gwinListItemDelete(self->ghList, mp_obj_get_int(index));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ugfx_list_remove_item_obj, ugfx_list_remove_item);


/// \method get_selected_text()
///
STATIC mp_obj_t ugfx_list_get_selected_text(mp_obj_t self_in) {
    ugfx_list_obj_t *self = self_in;
	const char* s = gwinListGetSelectedText(self->ghList);
    return mp_obj_new_str(s, strlen(s), TRUE);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_list_get_selected_obj, ugfx_list_get_selected_text);


/// \method get_selected_index()
///
STATIC mp_obj_t ugfx_list_get_selected_index(mp_obj_t self_in) {
    ugfx_list_obj_t *self = self_in;
    return mp_obj_new_int(gwinListGetSelected(self->ghList));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_list_get_selected_index_obj, ugfx_list_get_selected_index);


/// \method set_selected_index()
///
STATIC mp_obj_t ugfx_list_set_selected_index(mp_obj_t self_in, mp_obj_t index) {
    ugfx_list_obj_t *self = self_in;
	gwinListSetSelected(self->ghList, mp_obj_get_int(index), TRUE);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ugfx_list_set_selected_index_obj, ugfx_list_set_selected_index);





/// \method count()
///
STATIC mp_obj_t ugfx_list_count(mp_obj_t self_in) {
    ugfx_list_obj_t *self = self_in;
    return mp_obj_new_int(gwinListItemCount(self->ghList));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_list_count_obj, ugfx_list_count);


STATIC const mp_map_elem_t ugfx_list_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_destroy), (mp_obj_t)&ugfx_list_destroy_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR___del__), (mp_obj_t)&ugfx_list_destroy_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_attach_input), (mp_obj_t)&ugfx_widget_attach_input_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_detach_input), (mp_obj_t)&ugfx_widget_detach_input_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_add_item), (mp_obj_t)&ugfx_list_add_item_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_assign_image), (mp_obj_t)&ugfx_list_assign_image_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_remove_item), (mp_obj_t)&ugfx_list_remove_item_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_selected_text), (mp_obj_t)&ugfx_list_get_selected_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_selected_index), (mp_obj_t)&ugfx_list_get_selected_index_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_set_selected_index), (mp_obj_t)&ugfx_list_set_selected_index_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_count), (mp_obj_t)&ugfx_list_count_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_text), (mp_obj_t)&ugfx_widget_text_obj },

	//class constants
    { MP_OBJ_NEW_QSTR(MP_QSTR_ROLES),        MP_OBJ_NEW_SMALL_INT(2) },


};

STATIC MP_DEFINE_CONST_DICT(ugfx_list_locals_dict, ugfx_list_locals_dict_table);

const mp_obj_type_t ugfx_list_type = {
    { &mp_type_type },
    .name = MP_QSTR_List,
    .make_new = ugfx_list_make_new,
    .locals_dict = (mp_obj_t)&ugfx_list_locals_dict,
};








/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////      KEYBOARD    //////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

typedef struct _ugfx_keyboard_t {
    mp_obj_base_t base;
	GHandle ghKeyboard;
	GListener	gl;
	mp_obj_t kb_callback;

} ugfx_keyboard_obj_t;

/// \classmethod \constructor(x, y, a, b, text, {parent})
///
/// Construct an Keyboard object.
/// Will take the style from the parent, if the parents style is set. Otherwise uses default style
STATIC mp_obj_t ugfx_keyboard_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 4, 5, false);


    //const char *text = mp_obj_str_get_str(args[4]);
	int x = mp_obj_get_int(args[0]);
	int y = mp_obj_get_int(args[1]);
	int a = mp_obj_get_int(args[2]);
	int b = mp_obj_get_int(args[3]);

	GHandle parent = NULL;

    // create keyboard object
    ugfx_keyboard_obj_t *kbd = m_new_obj(ugfx_keyboard_obj_t);
    kbd->base.type = &ugfx_keyboard_type;


	//setup keyboard options
	GWidgetInit	wi;

	// Apply some default values for GWIN
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;

	// Apply the keyboard parameters
	wi.g.width = a;
	wi.g.height = b;
	wi.g.y = y;
	wi.g.x = x;
	wi.text = 0; //text;

	if (n_args > 4){
		ugfx_container_obj_t *container = args[4];
		if (MP_OBJ_IS_TYPE(args[4], &ugfx_container_type)) {
			parent = container->ghContainer;
			wi.customStyle = container->style;
		}
	}
	wi.g.parent = parent;

	// Create the actual keyboard
	kbd->ghKeyboard = gwinKeyboardCreate(NULL, &wi);

	kbd->kb_callback = mp_const_none;


	return kbd;
}





// Process an event
static void ugfx_keyboard_event(void *param, GEvent *pe) {
	#define pke		((GEventKeyboard *)pe)
/*
	(void)				param;

	// Process various events
	switch (pe->type) {
	case GEVENT_KEYBOARD:
		// If Tab key pressed then set focus to next widget
		if (pke->bytecount == 1 && pke->c[0] == GKEY_TAB) {
			if (!(pke->keystate & GKEYSTATE_KEYUP))
				_gwinMoveFocus();
			break;
		}

		// Otherwise, send keyboard events only to widget in focus
		if (_widgetInFocus)
			((gwidgetVMT*)_widgetInFocus->vmt)->KeyboardEvent((GWidgetObject*)_widgetInFocus, pke);
		break;
	default:
		break;
	}

*/
	#undef pke

}

/// \method set_keyboard_callback(callback)
///
/// Sets the callback for any active keyboard widgets
///
STATIC mp_obj_t ugfx_set_keyboard_callback(mp_obj_t self_in, mp_obj_t callback) {
	ugfx_keyboard_obj_t *self = self_in;

	if (callback == mp_const_none) {
        self->kb_callback = mp_const_none;
		geventDetachSource(&(self->gl), gwinKeyboardGetEventSource(self->ghKeyboard));
    } else if (mp_obj_is_callable(callback)) {
        self->kb_callback = callback;
		geventListenerInit(&(self->gl));
		geventAttachSource(&(self->gl), gwinKeyboardGetEventSource(self->ghKeyboard), GLISTEN_KEYUP);
		geventRegisterCallback(&(self->gl), ugfx_keyboard_event, 0);
    } else {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "callback must be None or a callable object"));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ugfx_set_keyboard_callback_obj, ugfx_set_keyboard_callback);


/// \method clear_keyboard_callback(callback)
///
/// Sets the callback for any active keyboard widgets
///
STATIC mp_obj_t ugfx_clear_keyboard_callback(mp_obj_t self_in, mp_obj_t callback) {
	ugfx_keyboard_obj_t *self = self_in;

    self->kb_callback = mp_const_none;
	geventDetachSource(&(self->gl), gwinKeyboardGetEventSource(self->ghKeyboard));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ugfx_clear_keyboard_callback_obj, ugfx_clear_keyboard_callback);


/// \method selected_key()
///
STATIC mp_obj_t ugfx_keyboard_selected_key(mp_obj_t self_in) {
    ugfx_keyboard_obj_t *self = self_in;
	const char utf8_str[9];	
	uint8_t len = gwinKeyboardGetSelected(self->ghKeyboard, (uint8_t*)utf8_str);
//	utf8_str[len] = 0;	
    return mp_obj_new_str(utf8_str, len, TRUE);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_keyboard_selected_key_obj, ugfx_keyboard_selected_key);



/// \method destroy()
///
/// frees up all resources
STATIC mp_obj_t ugfx_keyboard_destroy(mp_obj_t self_in) {
    ugfx_keyboard_obj_t *self = self_in;

	gwinDestroy(self->ghKeyboard);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_keyboard_destroy_obj, ugfx_keyboard_destroy);

/*
/// \method callback(fun)
/// Set the function to be called when a key is pressed.
/// `fun` is passed 1 argument, the timer object.
/// If `fun` is `None` then the callback will be disabled.
STATIC mp_obj_t ugfx_keyboard_callback(mp_obj_t self_in, mp_obj_t callback) {
    pyb_keyboard_obj_t *self = self_in;
    if (callback == mp_const_none) {
        // stop interrupt (but not timer)
        __HAL_TIM_DISABLE_IT(&self->tim, TIM_IT_UPDATE);
        self->callback = mp_const_none;
    } else if (mp_obj_is_callable(callback)) {
        __HAL_TIM_DISABLE_IT(&self->tim, TIM_IT_UPDATE);
        self->callback = callback;
        // start timer, so that it interrupts on overflow, but clear any
        // pending interrupts which may have been set by initializing it.
        __HAL_TIM_CLEAR_FLAG(&self->tim, TIM_IT_UPDATE);
        HAL_TIM_Base_Start_IT(&self->tim); // This will re-enable the IRQ
        HAL_NVIC_EnableIRQ(self->irqn);
    } else {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "callback must be None or a callable object"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ugfx_keyboard_callback_obj, ugfx_keyboard_callback);
*/



STATIC const mp_map_elem_t ugfx_keyboard_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_destroy), (mp_obj_t)&ugfx_keyboard_destroy_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR___del__), (mp_obj_t)&ugfx_keyboard_destroy_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_attach_input), (mp_obj_t)&ugfx_widget_attach_input_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_detach_input), (mp_obj_t)&ugfx_widget_detach_input_obj },
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_callback), (mp_obj_t)&ugfx_keyboard_callback_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_text), (mp_obj_t)&ugfx_widget_text_obj },

	{ MP_OBJ_NEW_QSTR(MP_QSTR_set_keyboard_callback), (mp_obj_t)&ugfx_set_keyboard_callback_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_clear_keyboard_callback), (mp_obj_t)&ugfx_clear_keyboard_callback_obj },


	//class constants
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_RED),        MP_OBJ_NEW_SMALL_INT(Red) },


};

STATIC MP_DEFINE_CONST_DICT(ugfx_keyboard_locals_dict, ugfx_keyboard_locals_dict_table);

const mp_obj_type_t ugfx_keyboard_type = {
    { &mp_type_type },
    .name = MP_QSTR_Keyboard,
    .make_new = ugfx_keyboard_make_new,
    .locals_dict = (mp_obj_t)&ugfx_keyboard_locals_dict,
};






/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
////////////////      IMAGEBOX     //////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////


typedef struct _ugfx_imagebox_t {
    mp_obj_base_t base;

	GHandle ghImagebox;

} ugfx_imagebox_obj_t;

/// \classmethod \constructor(x, y, a, b, text, cache {parent})
///
/// Construct an imagebox object.
/// Will take the style from the parent, if the parents style is set. Otherwise uses default style
STATIC mp_obj_t ugfx_imagebox_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 6, 7, false);


    const char *file = mp_obj_str_get_str(args[4]);
	int x = mp_obj_get_int(args[0]);
	int y = mp_obj_get_int(args[1]);
	int a = mp_obj_get_int(args[2]);
	int b = mp_obj_get_int(args[3]);
	int cache = mp_obj_get_int(args[5]);

	GHandle parent = NULL;

    // create imagebox object
    ugfx_imagebox_obj_t *img = m_new_obj(ugfx_imagebox_obj_t);
    img->base.type = &ugfx_imagebox_type;


	//setup imagebox options
	GWidgetInit	wi;

	// Apply some default values for GWIN
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;

	// Apply the imagebox parameters
	wi.g.width = a;
	wi.g.height = b;
	wi.g.y = y;
	wi.g.x = x;
	
	
	if (n_args > 6){
		ugfx_container_obj_t *container = args[6];
		if (MP_OBJ_IS_TYPE(args[6], &ugfx_container_type)) {
			parent = container->ghContainer;
			wi.customStyle = container->style;
		}
	}
	wi.g.parent = parent;

	// Create the actual imagebox
	img->ghImagebox = gwinImageCreate(NULL, &wi.g);
	
	//we'll open the file initially to fill the gdispImage struct
	//when the draw function is used, will need to check the image handle is open
	int suc = gwinImageOpenFile(img->ghImagebox, file);
	
	if (suc){
		if (cache){
			int err = gwinImageCache(img->ghImagebox);
			print_image_error(err);
		}
	}
	else{
		nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Error opening file"));
		return mp_const_none;
	}

	
	return img;
}


/// \method destroy()
///
/// frees up all resources
STATIC mp_obj_t ugfx_imagebox_destroy(mp_obj_t self_in) {
    ugfx_imagebox_obj_t *self = self_in;

	gwinDestroy(self->ghImagebox);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_imagebox_destroy_obj, ugfx_imagebox_destroy);


STATIC const mp_map_elem_t ugfx_imagebox_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_destroy), (mp_obj_t)&ugfx_imagebox_destroy_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR___del__), (mp_obj_t)&ugfx_imagebox_destroy_obj},

	//class constants
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_RED),        MP_OBJ_NEW_SMALL_INT(Red) },

};

STATIC MP_DEFINE_CONST_DICT(ugfx_imagebox_locals_dict, ugfx_imagebox_locals_dict_table);

const mp_obj_type_t ugfx_imagebox_type = {
    { &mp_type_type },
    .name = MP_QSTR_Imagebox,
    .make_new = ugfx_imagebox_make_new,
    .locals_dict = (mp_obj_t)&ugfx_imagebox_locals_dict,
};

void print_image_error(gdispImageError err){
	switch(err){
		case GDISP_IMAGE_ERR_UNRECOVERABLE:
			nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Failed with error: ERR_UNRECOVERABLE"));
			break;
		case GDISP_IMAGE_ERR_BADFORMAT:
			nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Failed with error: ERR_BADFORMAT"));
			break;
		case GDISP_IMAGE_ERR_BADDATA:
			nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Failed with error: ERR_BADDATA"));
			break;
		case GDISP_IMAGE_ERR_UNSUPPORTED:
			nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Failed with error: ERR_UNSUPPORTED"));
			break;
		case GDISP_IMAGE_ERR_UNSUPPORTED_OK:
			nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Failed with error: ERR_UNSUPPORTED_OK"));
			break;
		case GDISP_IMAGE_ERR_NOMEMORY:
			nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Failed with error: ERR_NOMEMORY"));
			break;
		case GDISP_IMAGE_ERR_NOSUCHFILE:
			nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Failed with error: ERR_NOSUCHFILE"));
			break;
		default: break;				
	}	
}


/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////       LABEL      //////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////


typedef struct _ugfx_label_t {
    mp_obj_base_t base;

	GHandle ghLabel;

} ugfx_label_obj_t;

/// \classmethod \constructor(x, y, a, b, text, {parent})
///
/// Construct an label object.
/// Will take the style from the parent, if the parents style is set. Otherwise uses default style
STATIC mp_obj_t ugfx_label_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 5, 6, false);


    const char *text = mp_obj_str_get_str(args[4]);
	int x = mp_obj_get_int(args[0]);
	int y = mp_obj_get_int(args[1]);
	int a = mp_obj_get_int(args[2]);
	int b = mp_obj_get_int(args[3]);

	GHandle parent = NULL;

    // create label object
    ugfx_label_obj_t *btn = m_new_obj(ugfx_label_obj_t);
    btn->base.type = &ugfx_label_type;


	//setup label options
	GWidgetInit	wi;

	// Apply some default values for GWIN
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;

	// Apply the label parameters
	wi.g.width = a;
	wi.g.height = b;
	wi.g.y = y;
	wi.g.x = x;
	wi.text = text;

	if (n_args > 5){
		ugfx_container_obj_t *container = args[5];
		if (MP_OBJ_IS_TYPE(args[5], &ugfx_container_type)) {
			parent = container->ghContainer;
			wi.customStyle = container->style;
		}
	}
	wi.g.parent = parent;

	// Create the actual label
	btn->ghLabel = gwinLabelCreate(NULL, &wi);


	return btn;
}


/// \method destroy()
///
/// frees up all resources
STATIC mp_obj_t ugfx_label_destroy(mp_obj_t self_in) {
    ugfx_label_obj_t *self = self_in;

	gwinDestroy(self->ghLabel);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_label_destroy_obj, ugfx_label_destroy);


STATIC const mp_map_elem_t ugfx_label_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_destroy), (mp_obj_t)&ugfx_label_destroy_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR___del__), (mp_obj_t)&ugfx_label_destroy_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_text), (mp_obj_t)&ugfx_widget_text_obj },

	//class constants
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_RED),        MP_OBJ_NEW_SMALL_INT(Red) },


};

STATIC MP_DEFINE_CONST_DICT(ugfx_label_locals_dict, ugfx_label_locals_dict_table);

const mp_obj_type_t ugfx_label_type = {
    { &mp_type_type },
    .name = MP_QSTR_Label,
    .make_new = ugfx_label_make_new,
    .locals_dict = (mp_obj_t)&ugfx_label_locals_dict,
};









/// \class image - provides a wrapper for uGFX images
///
/// This class is used to hold a uGFX image handle, and can also be
/// used to cache images if used multiple times
///
///     lcd = pyb.image(filepath, cache)
///        where cache = TRUE/FALSE
///



/// \classmethod \constructor(file_path, cache)
///
/// Construct an uGFX image object.
STATIC mp_obj_t ugfx_image_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, 2, false);

	int cache;
	if (n_args == 1)
		cache = false;
	else
		cache = mp_obj_get_int(args[1]);
	
	const char *img_str = mp_obj_str_get_str(args[0]);


    // create object
    ugfx_image_obj_t *image = m_new_obj(ugfx_image_obj_t);
    image->base.type = &ugfx_image_type;
	
	
	//we'll open the file initially to fill the gdispImage struct
	//when the draw function is used, will need to check the image handle is open
	gdispImageError er = gdispImageOpenFile(&(image->thisImage), img_str);
	
	if (er == 0){
		if (cache){
			int err = gdispImageCache(&(image->thisImage));
			print_image_error(err);
		}
		//gdispImageClose(&(image->thisImage));  //TODO: delete this, currently for debugging reasons
		//TODO: error handling and reporting
		return image;
	}
	else{
		nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Error opening file"));
		return mp_const_none;
	}
		
	
}


/// \method close()
///
/// Frees up memory if cache was used, and closes the file
STATIC mp_obj_t ugfx_image_close(mp_obj_t self_in) {
    ugfx_image_obj_t *self = self_in;
    gdispImageClose(&(self->thisImage));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_image_close_obj, ugfx_image_close);


STATIC const mp_map_elem_t ugfx_image_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_close), (mp_obj_t)&ugfx_image_close_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR___del__), (mp_obj_t)&ugfx_image_close_obj },
	
	//class constants
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_RED),        MP_OBJ_NEW_SMALL_INT(Red) },


};

STATIC MP_DEFINE_CONST_DICT(ugfx_image_locals_dict, ugfx_image_locals_dict_table);

const mp_obj_type_t ugfx_image_type = {
    { &mp_type_type },
    .name = MP_QSTR_Image,
    .make_new = ugfx_image_make_new,
    .locals_dict = (mp_obj_t)&ugfx_image_locals_dict,
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

/// \classmethod \constructor(x, y, a, b, text, {parent})
///
/// Construct an Checkbox object.
/// Will take the style from the parent, if the parents style is set. Otherwise uses default style
STATIC mp_obj_t ugfx_checkbox_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 5, 6, false);


    const char *text = mp_obj_str_get_str(args[4]);
	int x = mp_obj_get_int(args[0]);
	int y = mp_obj_get_int(args[1]);
	int a = mp_obj_get_int(args[2]);
	int b = mp_obj_get_int(args[3]);

	GHandle parent = NULL;


    // create checkbox object
    ugfx_checkbox_obj_t *btn = m_new_obj(ugfx_checkbox_obj_t);
    btn->base.type = &ugfx_checkbox_type;


	//setup checkbox options
	GWidgetInit	wi;

	// Apply some default values for GWIN
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;

	// Apply the checkbox parameters
	wi.g.width = a;
	wi.g.height = b;
	wi.g.y = y;
	wi.g.x = x;
	wi.text = text;

	if (n_args > 5){
		ugfx_container_obj_t *container = args[5];

		if (MP_OBJ_IS_TYPE(args[5], &ugfx_container_type)) {
			parent = container->ghContainer;
			wi.customStyle = container->style;
		}
	}
	wi.g.parent = parent;

	// Create the actual checkbox
	btn->ghCheckbox = gwinCheckboxCreate(NULL, &wi);


	return btn;
}

/// \method destroy()
///
/// frees up all resources
STATIC mp_obj_t ugfx_checkbox_destroy(mp_obj_t self_in) {
    ugfx_checkbox_obj_t *self = self_in;

	gwinDestroy(self->ghCheckbox);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_checkbox_destroy_obj, ugfx_checkbox_destroy);

/// \method checked()
///
/// Gets or sets the checked state
STATIC mp_obj_t ugfx_checkbox_checked(mp_uint_t n_args, const mp_obj_t *args) {
	
	GHandle gh = get_ugfx_handle(args[0]);
	
	if (n_args == 1)
		return mp_obj_new_int(gwinCheckboxIsChecked(gh));
	else
		gwinCheckboxCheck(gh, mp_obj_get_int(args[1]));
	return mp_const_none;
	   
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ugfx_checkbox_checked_obj, 1, 2, ugfx_checkbox_checked);



STATIC const mp_map_elem_t ugfx_checkbox_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_destroy), (mp_obj_t)&ugfx_checkbox_destroy_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR___del__), (mp_obj_t)&ugfx_checkbox_destroy_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_attach_input), (mp_obj_t)&ugfx_widget_attach_input_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_detach_input), (mp_obj_t)&ugfx_widget_detach_input_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_text), (mp_obj_t)&ugfx_widget_text_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_checked), (mp_obj_t)&ugfx_checkbox_checked_obj },

	//class constants
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_RED),        MP_OBJ_NEW_SMALL_INT(Red) },


};

STATIC MP_DEFINE_CONST_DICT(ugfx_checkbox_locals_dict, ugfx_checkbox_locals_dict_table);

const mp_obj_type_t ugfx_checkbox_type = {
    { &mp_type_type },
    .name = MP_QSTR_Checkbox,
    .make_new = ugfx_checkbox_make_new,
    .locals_dict = (mp_obj_t)&ugfx_checkbox_locals_dict,
};




static GHandle get_ugfx_handle(mp_obj_t in){
	if (MP_OBJ_IS_TYPE(in,&ugfx_button_type))
		return ((ugfx_button_obj_t *)in)->ghButton;
	else if (MP_OBJ_IS_TYPE(in,&ugfx_checkbox_type))
		return ((ugfx_checkbox_obj_t *)in)->ghCheckbox;
	else if (MP_OBJ_IS_TYPE(in,&ugfx_list_type))
		return ((ugfx_list_obj_t *)in)->ghList;
	else if (MP_OBJ_IS_TYPE(in,&ugfx_textbox_type))
		return ((ugfx_textbox_obj_t *)in)->ghTextbox;
	else if (MP_OBJ_IS_TYPE(in,&ugfx_keyboard_type))
		return ((ugfx_keyboard_obj_t *)in)->ghKeyboard;
	else if (MP_OBJ_IS_TYPE(in,&ugfx_label_type))
		return ((ugfx_label_obj_t *)in)->ghLabel;
	//else if (MP_OBJ_IS_TYPE(in,&ugfx_label_type))
	//	return ((ugfx_container_obj_t *)in)->ghContainer;
	return 0;
}


#endif // MICROPY_HW_HAS_UGFX
