/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Paul Sokolovsky
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

#include <stdlib.h>

#include "py/runtime.h"

// This is universal iterator type which calls "iternext" method stored in
// particular object instance. (So, each instance of this time can have its
// own iteration behavior.) Having this type saves to define type objects
// for various internal iterator objects.

// Any instance should have these 2 fields at the beginning
typedef struct _mp_obj_polymorph_iter_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
} mp_obj_polymorph_iter_t;

static mp_obj_t polymorph_it_iternext(mp_obj_t self_in) {
    mp_obj_polymorph_iter_t *self = MP_OBJ_TO_PTR(self_in);
    // Redirect call to object instance's iternext method
    return self->iternext(self_in);
}

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_polymorph_iter,
    MP_QSTR_iterator,
    MP_TYPE_FLAG_ITER_IS_ITERNEXT,
    iter, polymorph_it_iternext
    );

#if MICROPY_ENABLE_FINALISER
// mp_type_polymorph_iter_with_finaliser is a variant of the universal iterator
// above which has a finaliser function attached. This function will be run when
// the GC collects the iter object and can be used to close file handles etc.

// Any instance should have these 3 fields at the beginning
typedef struct _mp_obj_polymorph_iter_with_finaliser_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    mp_fun_1_t finaliser;
} mp_obj_polymorph_with_finaliser_iter_t;

static mp_obj_t mp_obj_polymorph_iter_del(mp_obj_t self_in) {
    mp_obj_polymorph_with_finaliser_iter_t *self = MP_OBJ_TO_PTR(self_in);
    // Redirect call to object instance's finaliser method
    return self->finaliser(self_in);
}
static MP_DEFINE_CONST_FUN_OBJ_1(mp_obj_polymorph_iter_del_obj, mp_obj_polymorph_iter_del);

static const mp_rom_map_elem_t mp_obj_polymorph_iter_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mp_obj_polymorph_iter_del_obj) },
};
static MP_DEFINE_CONST_DICT(mp_obj_polymorph_iter_locals_dict, mp_obj_polymorph_iter_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_polymorph_iter_with_finaliser,
    MP_QSTR_iterator,
    MP_TYPE_FLAG_ITER_IS_ITERNEXT,
    iter, polymorph_it_iternext,
    locals_dict, &mp_obj_polymorph_iter_locals_dict
    );
#endif
