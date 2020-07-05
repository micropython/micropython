/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright 2020 Sony Semiconductor Solutions Corporation
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_GNSS_POSITIONFIX_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_GNSS_POSITIONFIX_H

#include "py/obj.h"

typedef enum {
    POSITIONFIX_INVALID,
    POSITIONFIX_2D,
    POSITIONFIX_3D,
} gnss_positionfix_t;

const mp_obj_type_t gnss_positionfix_type;

gnss_positionfix_t gnss_positionfix_obj_to_type(mp_obj_t obj);
mp_obj_t gnss_positionfix_type_to_obj(gnss_positionfix_t mode);

typedef struct {
    mp_obj_base_t base;
} gnss_positionfix_obj_t;
extern const gnss_positionfix_obj_t gnss_positionfix_invalid_obj;
extern const gnss_positionfix_obj_t gnss_positionfix_fix2d_obj;
extern const gnss_positionfix_obj_t gnss_positionfix_fix3d_obj;

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_GNSS_POSITIONFIX_H
