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
STATIC mp_obj_t pyb_ugfx_font_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);


    const char *font_text = mp_obj_str_get_str(args[0]);

    // create font object
    pyb_ugfx_font_obj_t *fnt = m_new_obj(pyb_ugfx_font_obj_t);
    fnt->base.type = &pyb_ugfx_font_type;

	fnt->font = gdispOpenFont(font_text);
	if (fnt->font == NULL)
		nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Font '(%s)' does not exist", font_text));


	return fnt;
}

/// \method destroy()
///
/// frees up all resources
STATIC mp_obj_t pyb_ugfx_font_destroy(mp_obj_t self_in) {
    pyb_ugfx_font_obj_t *self = self_in;
    
	gdispCloseFont(self->font);
	
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_ugfx_font_destroy_obj, pyb_ugfx_font_destroy);


STATIC const mp_map_elem_t pyb_ugfx_font_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_destroy), (mp_obj_t)&pyb_ugfx_font_destroy_obj},
	//class constants
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_RED),        MP_OBJ_NEW_SMALL_INT(Red) },


};

STATIC MP_DEFINE_CONST_DICT(pyb_ugfx_font_locals_dict, pyb_ugfx_font_locals_dict_table);

const mp_obj_type_t pyb_ugfx_font_type = {
    { &mp_type_type },
    .name = MP_QSTR_Font,
    .make_new = pyb_ugfx_font_make_new,
    .locals_dict = (mp_obj_t)&pyb_ugfx_font_locals_dict,
};


#endif // MICROPY_HW_HAS_UGFX
