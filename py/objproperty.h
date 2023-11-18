/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
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
#ifndef MICROPY_INCLUDED_PY_OBJPROPERTY_H
#define MICROPY_INCLUDED_PY_OBJPROPERTY_H

#include "py/obj.h"

// CIRCUITPY-CHANGE: MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS marks classes with
// properties, descriptors, __delattr__ or __setattr___.
// When defining native classes that use properties, you *must* set the
// MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS flag. Otherwise, the property will be
// ignored.

#if MICROPY_PY_BUILTINS_PROPERTY

typedef struct _mp_obj_property_t {
    mp_obj_base_t base;
    mp_obj_t proxy[3]; // getter, setter, deleter
} mp_obj_property_t;

#if MICROPY_PY_OPTIMIZE_PROPERTY_FLASH_SIZE
typedef struct _mp_obj_property_getter_t {
    mp_obj_base_t base;
    mp_obj_t proxy[1]; // getter
} mp_obj_property_getter_t;

typedef struct _mp_obj_property_getset_t {
    mp_obj_base_t base;
    mp_obj_t proxy[2]; // getter, setter
} mp_obj_property_getset_t;

#define MP_PROPERTY_GETTER(P, G) const mp_obj_property_getter_t P __attribute((section(".property_getter"))) = {.base.type = &mp_type_property, .proxy = {G}}
#define MP_PROPERTY_GETSET(P, G, S) const mp_obj_property_getset_t P __attribute((section(".property_getset"))) = {.base.type = &mp_type_property, .proxy = {G, S}}

#else
typedef struct _mp_obj_property_t mp_obj_property_getter_t;
typedef struct _mp_obj_property_t mp_obj_property_getset_t;

#define MP_PROPERTY_GETTER(P, G) const mp_obj_property_t P = {.base.type = &mp_type_property, .proxy = {G, MP_ROM_NONE, MP_ROM_NONE}}
#define MP_PROPERTY_GETSET(P, G, S) const mp_obj_property_t P = {.base.type = &mp_type_property, .proxy = {G, S, MP_ROM_NONE}}
#endif

#endif  // MICROPY_PY_BUILTINS_PROPERTY

#endif  // MICROPY_INCLUDED_PY_OBJPROPERTY_H
