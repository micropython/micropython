/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Philipp Ebensberger
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
#include "py/mphal.h"
#include "pin.h"


STATIC void pin_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    // TODO: implement pin_obj_print
    return;
}

STATIC mp_obj_t pin_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
	// TODO: implement pin_obj_make_new
	return NULL;
}

STATIC mp_obj_t pin_obj_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
	// TODO: implement pin_obj_call
	return NULL;
}


// ================================================================================================================== //
// ================================================================================================================== //

STATIC const mp_rom_map_elem_t pin_locals_dict_table[] = {
	// TODO: implement pin object methods
};
STATIC MP_DEFINE_CONST_DICT(pin_locals_dict, pin_locals_dict_table);

// ================================================================================================================== //

const mp_obj_type_t machine_pin_type = {
    { &mp_type_type },
    .name = MP_QSTR_PIN,
    .print = pin_obj_print,
    .make_new = pin_obj_make_new,
    .call = pin_obj_call,
    .locals_dict = (mp_obj_dict_t *)&pin_locals_dict,
};

const mp_obj_type_t machine_pin_af_type = {
    { &mp_type_type },
    .name = MP_QSTR_PIN_AF
};