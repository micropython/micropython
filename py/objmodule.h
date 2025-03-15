/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2019 Damien P. George
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
#ifndef MICROPY_INCLUDED_PY_OBJMODULE_H
#define MICROPY_INCLUDED_PY_OBJMODULE_H

#include "py/obj.h"

#ifndef NO_QSTR
// Only include module definitions when not doing qstr extraction, because the
// qstr extraction stage also generates this module definition header file.
#include "genhdr/moduledefs.h"
#endif

extern const mp_map_t mp_builtin_module_map;
extern const mp_map_t mp_builtin_extensible_module_map;

mp_obj_t mp_module_get_builtin(qstr module_name, bool extensible);

void mp_module_generic_attr(qstr attr, mp_obj_t *dest, const uint16_t *keys, mp_obj_t *values);

#endif // MICROPY_INCLUDED_PY_OBJMODULE_H
