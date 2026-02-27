/*
 * This file is part of the MicroPython project, http://micropython.org/
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
#ifndef MICROPY_INCLUDED_PY_OBJLIST_H
#define MICROPY_INCLUDED_PY_OBJLIST_H

#include "py/obj.h"
#include "py/misc.h"

typedef struct _mp_obj_list_t {
    mp_obj_base_t base;
    size_t alloc;
    size_t len;
    mp_obj_t *items MP_ATTR_COUNTED_BY(alloc);
} mp_obj_list_t;

void mp_obj_list_init(mp_obj_list_t *o, size_t n);
mp_obj_t mp_obj_list_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args);
mp_obj_t mp_obj_list_append(mp_obj_t self_in, mp_obj_t arg);
mp_obj_t mp_obj_list_sort(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs);
mp_obj_t mp_obj_list_remove(mp_obj_t self_in, mp_obj_t value);

static inline void mp_obj_list_get(mp_obj_t self_in, size_t *len, mp_obj_t **items) {
    mp_obj_list_t *self = (mp_obj_list_t *)MP_OBJ_TO_PTR(self_in);
    *len = self->len;
    *items = self->items;
}

static inline void mp_obj_list_set_len(mp_obj_t self_in, size_t len) {
    // trust that the caller knows what it's doing
    // TODO realloc if len got much smaller than alloc
    mp_obj_list_t *self = (mp_obj_list_t *)MP_OBJ_TO_PTR(self_in);
    self->len = len;
}

static inline void mp_obj_list_store(mp_obj_t self_in, mp_obj_t index, mp_obj_t value) {
    mp_obj_list_t *self = (mp_obj_list_t *)MP_OBJ_TO_PTR(self_in);
    size_t i = mp_get_index(self->base.type, self->len, index, false);
    self->items[i] = value;
}

#endif // MICROPY_INCLUDED_PY_OBJLIST_H
