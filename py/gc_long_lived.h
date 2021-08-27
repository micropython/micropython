/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries LLC
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

// These helpers move MicroPython objects and their sub-objects to the long lived portion of the
// heap.

#ifndef MICROPY_INCLUDED_PY_GC_LONG_LIVED_H
#define MICROPY_INCLUDED_PY_GC_LONG_LIVED_H

#include "py/objfun.h"
#include "py/objproperty.h"
#include "py/objstr.h"

mp_obj_fun_bc_t *make_fun_bc_long_lived(mp_obj_fun_bc_t *fun_bc, uint8_t max_depth);
mp_obj_property_t *make_property_long_lived(mp_obj_property_t *prop, uint8_t max_depth);
mp_obj_dict_t *make_dict_long_lived(mp_obj_dict_t *dict, uint8_t max_depth);
mp_obj_str_t *make_str_long_lived(mp_obj_str_t *str);
mp_obj_t make_obj_long_lived(mp_obj_t obj, uint8_t max_depth);

#endif // MICROPY_INCLUDED_PY_GC_LONG_LIVED_H
