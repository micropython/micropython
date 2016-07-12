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

extern const mp_obj_type_t ugfx_button_type;
extern const mp_obj_type_t ugfx_checkbox_type;
extern const mp_obj_type_t ugfx_list_type;
extern const mp_obj_type_t ugfx_textbox_type;
extern const mp_obj_type_t ugfx_keyboard_type;
extern const mp_obj_type_t ugfx_label_type;
extern const mp_obj_type_t ugfx_image_type;
extern const mp_obj_type_t ugfx_imagebox_type;

void print_image_error(gdispImageError err);

typedef struct _ugfx_image_obj_t {
    mp_obj_base_t base;
	
	gdispImage thisImage; 

} ugfx_image_obj_t;
