/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Glenn Ruben Bakke
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

#include "py/obj.h"
#include "py/runtime.h"

#if MICROPY_PY_UBLUEPY

#include "modubluepy.h"

STATIC const mp_map_elem_t ubluepy_constants_locals_dict_table[] = {
#if (BLUETOOTH_SD == 132)
    // GAP events
    { MP_OBJ_NEW_QSTR(MP_QSTR_EVT_GAP_CONNECTED),    MP_OBJ_NEW_SMALL_INT(16) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_EVT_GAP_DISCONNECTED), MP_OBJ_NEW_SMALL_INT(17) },
#endif
    { MP_OBJ_NEW_QSTR(MP_QSTR_UUID_CCCD),            MP_OBJ_NEW_SMALL_INT(0x2902) },
};

STATIC MP_DEFINE_CONST_DICT(ubluepy_constants_locals_dict, ubluepy_constants_locals_dict_table);

const mp_obj_type_t ubluepy_constants_type = {
    { &mp_type_type },
    .name = MP_QSTR_constants,
    .locals_dict = (mp_obj_t)&ubluepy_constants_locals_dict
};

#endif // MICROPY_PY_UBLUEPY
