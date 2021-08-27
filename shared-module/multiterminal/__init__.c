/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Paul Sokolovsky
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include "py/mpstate.h"

#include "shared-bindings/multiterminal/__init__.h"

mp_obj_t shared_module_multiterminal_get_secondary_terminal() {
    if (MP_STATE_VM(dupterm_objs[0]) == MP_OBJ_NULL) {
        return mp_const_none;
    } else {
        return MP_STATE_VM(dupterm_objs[0]);
    }
}

void shared_module_multiterminal_set_secondary_terminal(mp_obj_t secondary_terminal) {
    MP_STATE_VM(dupterm_objs[0]) = secondary_terminal;
    if (MP_STATE_PORT(dupterm_arr_obj) == MP_OBJ_NULL) {
        MP_STATE_PORT(dupterm_arr_obj) = mp_obj_new_bytearray(1, "");
    }
}

void shared_module_multiterminal_clear_secondary_terminal() {
    MP_STATE_VM(dupterm_objs[0]) = MP_OBJ_NULL;
    MP_STATE_PORT(dupterm_arr_obj) = MP_OBJ_NULL;
}
