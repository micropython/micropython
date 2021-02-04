/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2014 Paul Sokolovsky
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
#ifndef MICROPY_INCLUDED_PY_OBJARRAY_H
#define MICROPY_INCLUDED_PY_OBJARRAY_H

#include "py/obj.h"

// Used only for memoryview types, set in "typecode" to indicate a writable memoryview
#define MP_OBJ_ARRAY_TYPECODE_FLAG_RW (0x80)

// This structure is used for all of bytearray, array.array, memoryview
// objects.  Note that memoryview has different meaning for some fields,
// see comment at the beginning of objarray.c.
typedef struct _mp_obj_array_t {
    mp_obj_base_t base;
    size_t typecode : 8;
    // free is number of unused elements after len used elements
    // alloc size = len + free
    // But for memoryview, 'free' is reused as offset (in elements) into the
    // parent object. (Union is not used to not go into a complication of
    // union-of-bitfields with different toolchains). See comments in
    // objarray.c.
    size_t free : (8 * sizeof(size_t) - 8);
    size_t len; // in elements
    void *items;
} mp_obj_array_t;

#if MICROPY_PY_BUILTINS_MEMORYVIEW
static inline void mp_obj_memoryview_init(mp_obj_array_t *self, size_t typecode, size_t offset, size_t len, void *items) {
    self->base.type = &mp_type_memoryview;
    self->typecode = typecode;
    self->free = offset;
    self->len = len;
    self->items = items;
}
#endif

#endif // MICROPY_INCLUDED_PY_OBJARRAY_H
