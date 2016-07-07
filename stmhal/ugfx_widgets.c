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


/// \method attach_input(input)
///
STATIC mp_obj_t ugfx_button_attach_input(mp_obj_t self_in, mp_obj_t input) {
    ugfx_button_obj_t *self = self_in;

	int i = mp_obj_get_int(input);

	gwinAttachToggle(self->ghButton,0,i);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ugfx_button_attach_input_obj, ugfx_button_attach_input);



STATIC const mp_map_elem_t ugfx_button_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_destroy), (mp_obj_t)&ugfx_button_destroy_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR___del__), (mp_obj_t)&ugfx_button_destroy_obj},
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


/// \method attach_input(input, function)
///
STATIC mp_obj_t ugfx_list_attach_input(mp_obj_t self_in, mp_obj_t input, mp_obj_t function) {
    ugfx_list_obj_t *self = self_in;
    int fun = mp_obj_get_int(function);
	int i = mp_obj_get_int(input);

	gwinAttachToggle(self->ghList,fun,i);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(ugfx_list_attach_input_obj, ugfx_list_attach_input);


/// \method detach_input(function)
///
STATIC mp_obj_t ugfx_list_detach_input(mp_obj_t self_in, mp_obj_t function) {
    ugfx_list_obj_t *self = self_in;
    int fun = mp_obj_get_int(function);

	gwinDetachToggle(self->ghList,fun);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ugfx_list_detach_input_obj, ugfx_list_detach_input);



/// \method add_item(input)
///
STATIC mp_obj_t ugfx_list_add_item(mp_obj_t self_in, mp_obj_t str) {
    ugfx_list_obj_t *self = self_in;

	const char *s = mp_obj_str_get_str(str);

	gwinListAddItem(self->ghList,s,TRUE);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ugfx_list_add_item_obj, ugfx_list_add_item);


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
    { MP_OBJ_NEW_QSTR(MP_QSTR_attach_input), (mp_obj_t)&ugfx_list_attach_input_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_detach_input), (mp_obj_t)&ugfx_list_detach_input_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_add_item), (mp_obj_t)&ugfx_list_add_item_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_remove_item), (mp_obj_t)&ugfx_list_remove_item_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_selected_text), (mp_obj_t)&ugfx_list_get_selected_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_selected_index), (mp_obj_t)&ugfx_list_get_selected_index_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_set_selected_index), (mp_obj_t)&ugfx_list_set_selected_index_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_count), (mp_obj_t)&ugfx_list_count_obj },

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


/// \method attach_input(input, function)
///
STATIC mp_obj_t ugfx_keyboard_attach_input(mp_obj_t self_in, mp_obj_t input, mp_obj_t function) {
    ugfx_keyboard_obj_t *self = self_in;
    int fun = mp_obj_get_int(function);
	int i = mp_obj_get_int(input);

	gwinAttachToggle(self->ghKeyboard,fun,i);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(ugfx_keyboard_attach_input_obj, ugfx_keyboard_attach_input);



STATIC const mp_map_elem_t ugfx_keyboard_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_destroy), (mp_obj_t)&ugfx_keyboard_destroy_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR___del__), (mp_obj_t)&ugfx_keyboard_destroy_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_attach_input), (mp_obj_t)&ugfx_keyboard_attach_input_obj },
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_callback), (mp_obj_t)&ugfx_keyboard_callback_obj },

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


/// \method set_text(text)
///
STATIC mp_obj_t ugfx_label_set_text(mp_obj_t self_in, mp_obj_t str) {
    ugfx_label_obj_t *self = self_in;
    const char *s = mp_obj_str_get_str(str);

	gwinSetText(self->ghLabel, s, TRUE);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ugfx_label_set_text_obj, ugfx_label_set_text);


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
    { MP_OBJ_NEW_QSTR(MP_QSTR_set_text), (mp_obj_t)&ugfx_label_set_text_obj },

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









/*

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
    ugfx_checkbox_obj_t *self = self_in;

	gwinDestroy(self->ghCheckbox);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ugfx_check_destroy_obj, ugfx_check_destroy);


/// \method attach_input(pin)
///
/// attach a button to make button be pressable
STATIC mp_obj_t ugfx_check_attach_input(mp_obj_t self_in, mp_obj_t input) {
    ugfx_checkbox_obj_t *self = self_in;

	int i = mp_obj_get_int(input);

	gwinAttachToggle(self->ghCheckbox,0,i);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ugfx_check_attach_input_obj, ugfx_check_attach_input);



STATIC const mp_map_elem_t ugfx_checkbox_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_chk_destroy), (mp_obj_t)&ugfx_check_destroy_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_chk_attach_input), (mp_obj_t)&ugfx_check_attach_input_obj },

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

*/


#endif // MICROPY_HW_HAS_UGFX
