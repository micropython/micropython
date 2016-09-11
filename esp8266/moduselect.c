/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
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

#include <stdio.h>

#include "py/obj.h"


enum {
    POLL_IN   = 1 << 0,
    POLL_OUT  = 1 << 1,
    POLL_ERR    = 1 << 2,
    POLL_HUP    = 1 << 3,
};


STATIC const mp_map_elem_t uselect_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_uselect) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_select), MP_OBJ_NEW_QSTR(MP_QSTR_select) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_POLLIN), MP_OBJ_NEW_SMALL_INT(POLL_IN) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_POLLOUT), MP_OBJ_NEW_SMALL_INT(POLL_OUT) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_POLLERR), MP_OBJ_NEW_SMALL_INT(POLL_ERR) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_POLLHUP), MP_OBJ_NEW_SMALL_INT(POLL_HUP) },
};

STATIC MP_DEFINE_CONST_DICT(uselect_globals, uselect_globals_table);


const mp_obj_module_t uselect_module = {
    .base = { &mp_type_module },
    .name = MP_QSTR_uselect,
    .globals = (mp_obj_dict_t*) &uselect_globals,
};
