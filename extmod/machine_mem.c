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

#include "py/runtime.h"
#include "extmod/machine_mem.h"

#if MICROPY_PY_MACHINE

// If you wish to override the functions for mapping the machine_mem read/write
// address, then add a #define for MICROPY_MACHINE_MEM_GET_READ_ADDR and/or
// MICROPY_MACHINE_MEM_GET_WRITE_ADDR in your mpconfigport.h. Since the
// prototypes are identical, it is allowable for both of the macros to evaluate
// the to same function.
//
// It is expected that the modmachine.c file for a given port will provide the
// implementations, if the default implementation isn't used.

#if !defined(MICROPY_MACHINE_MEM_GET_READ_ADDR) || !defined(MICROPY_MACHINE_MEM_GET_WRITE_ADDR)
STATIC uintptr_t machine_mem_get_addr(machine_mem_obj_t *self, uintptr_t addr) {
    uint align = self->elem_size;
    if ((addr & (align - 1)) != 0) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("address %08x is not aligned to %d bytes"), addr, align);
    }
    return addr;
}
#if !defined(MICROPY_MACHINE_MEM_GET_READ_ADDR)
#define MICROPY_MACHINE_MEM_GET_READ_ADDR machine_mem_get_addr
#endif
#if !defined(MICROPY_MACHINE_MEM_GET_WRITE_ADDR)
#define MICROPY_MACHINE_MEM_GET_WRITE_ADDR machine_mem_get_addr
#endif
#endif

STATIC mp_obj_t machine_mem_new(unsigned elem_size, uintptr_t begin, uintptr_t end) {
    machine_mem_obj_t *self = m_new_obj(machine_mem_obj_t);
    self->base.type = &machine_mem_type;
    self->elem_size = elem_size;
    self->begin = begin;
    self->end = end;
    return MP_OBJ_FROM_PTR(self);
}

STATIC void machine_mem_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    machine_mem_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<%u-bit memory at %08x address>", 8 * self->elem_size, self->begin);
}

STATIC mp_obj_t machine_mem_subscr(mp_obj_t self_in, mp_obj_t index_in, mp_obj_t value) {
    // TODO support slice index to read/write multiple values at once
    machine_mem_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (value == MP_OBJ_NULL) {
        // delete
        return MP_OBJ_NULL; // op not supported
    }
    #if MICROPY_PY_BUILTINS_SLICE
    if (mp_obj_is_type(index_in, &mp_type_slice)) {
        mp_bound_slice_t slice;
        if (!mp_seq_get_fast_slice_indexes(INT_MAX, index_in, &slice)) {
            mp_raise_NotImplementedError(MP_ERROR_TEXT("only slices with step=1 (aka None) are supported"));
        }
        if (value == MP_OBJ_SENTINEL) {
            return machine_mem_new(self->elem_size, slice.start, slice.stop);
        }
    }
    #endif
    mp_int_t index = mp_obj_get_int_truncated(index_in);
    if (index < 0 || (self->end - self->begin) <= (uintptr_t)index) {
        mp_raise_msg_varg(&mp_type_IndexError, MP_ERROR_TEXT("address is out of range"));
    }
    if (value == MP_OBJ_SENTINEL) {
        // load
        uintptr_t addr = MICROPY_MACHINE_MEM_GET_READ_ADDR(self, self->begin + index);
        uint32_t val;
        switch (self->elem_size) {
            case 1:
                val = (*(uint8_t *)addr);
                break;
            case 2:
                val = (*(uint16_t *)addr);
                break;
            default:
                val = (*(uint32_t *)addr);
                break;
        }
        return mp_obj_new_int(val);
    } else {
        // store
        uintptr_t addr = MICROPY_MACHINE_MEM_GET_WRITE_ADDR(self, self->begin + index);
        uint32_t val = mp_obj_get_int_truncated(value);
        switch (self->elem_size) {
            case 1:
                (*(uint8_t *)addr) = val;
                break;
            case 2:
                (*(uint16_t *)addr) = val;
                break;
            default:
                (*(uint32_t *)addr) = val;
                break;
        }
        return mp_const_none;
    }
}

MP_DEFINE_CONST_OBJ_TYPE(
    machine_mem_type,
    MP_QSTR_mem,
    MP_TYPE_FLAG_NONE,
    print, machine_mem_print,
    subscr, machine_mem_subscr
    );

const machine_mem_obj_t machine_mem8_obj = {{&machine_mem_type}, 1, 0, UINT_MAX};
const machine_mem_obj_t machine_mem16_obj = {{&machine_mem_type}, 2, 0, UINT_MAX};
const machine_mem_obj_t machine_mem32_obj = {{&machine_mem_type}, 4, 0, UINT_MAX};

#endif // MICROPY_PY_MACHINE
