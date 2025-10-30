/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Koudai Aono
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
#ifndef MICROPY_INCLUDED_PY_OBJTSTRING_H
#define MICROPY_INCLUDED_PY_OBJTSTRING_H

#include "py/obj.h"

#if MICROPY_PY_TSTRINGS

// Interpolation object type - represents {expr} in t-strings
typedef struct _mp_obj_interpolation_t {
    mp_obj_base_t base;
    mp_obj_t value;         // Evaluated value of the expression
    mp_obj_t expression;    // String representation of the expression
    mp_obj_t conversion;    // None or single char string ('r', 's', 'a')
    mp_obj_t format_spec;   // Format specification string
} mp_obj_interpolation_t;

// Template object type - represents the result of a t-string literal
typedef struct _mp_obj_template_t {
    mp_obj_base_t base;
    mp_obj_t strings;        // Tuple of string segments
    mp_obj_t interpolations; // Tuple of Interpolation objects
} mp_obj_template_t;

// Exported type objects
extern const mp_obj_type_t mp_type_template;
extern const mp_obj_type_t mp_type_interpolation;

#endif // MICROPY_PY_TSTRINGS

#endif // MICROPY_INCLUDED_PY_OBJTSTRING_H
