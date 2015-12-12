/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include "extmod/machine_mem.h"

#if MICROPY_PY_MACHINE

STATIC void machine_mem_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    machine_mem_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<%u-bit memory>", 8 * self->elem_size);
}

STATIC mp_obj_t machine_mem_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value) {
    // TODO support slice index to read/write multiple values at once
    machine_mem_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (value == MP_OBJ_NULL) {
        // delete
        return MP_OBJ_NULL; // op not supported
    } else if (value == MP_OBJ_SENTINEL) {
        // load
        uintptr_t addr = machine_mem_get_read_addr(index, self->elem_size);
        uint32_t val;
        switch (self->elem_size) {
            case 1: val = (*(uint8_t*)addr); break;
            case 2: val = (*(uint16_t*)addr); break;
            default: val = (*(uint32_t*)addr); break;
        }
        return mp_obj_new_int(val);
    } else {
        // store
        uintptr_t addr = machine_mem_get_write_addr(index, self->elem_size);
        uint32_t val = mp_obj_get_int(value);
        switch (self->elem_size) {
            case 1: (*(uint8_t*)addr) = val; break;
            case 2: (*(uint16_t*)addr) = val; break;
            default: (*(uint32_t*)addr) = val; break;
        }
        return mp_const_none;
    }
}

const mp_obj_type_t machine_mem_type = {
    { &mp_type_type },
    .name = MP_QSTR_mem,
    .print = machine_mem_print,
    .subscr = machine_mem_subscr,
};

const machine_mem_obj_t machine_mem8_obj = {{&machine_mem_type}, 1};
const machine_mem_obj_t machine_mem16_obj = {{&machine_mem_type}, 2};
const machine_mem_obj_t machine_mem32_obj = {{&machine_mem_type}, 4};

#endif // MICROPY_PY_MACHINE
