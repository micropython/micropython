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

#include <stdio.h>
#include <stdint.h>

#include STM32_HAL_H

#include "py/nlr.h"
#include "py/obj.h"
#include "portmodules.h"

// To use compile-time constants we are restricted to 31-bit numbers (a small int,
// so it fits in a Micro Python object pointer).  Thus, when extracting a constant
// from an object, we must clear the MSB.

STATIC uint32_t get_read_addr(mp_obj_t addr_o, uint align) {
    uint32_t addr = mp_obj_get_int(addr_o) & 0x7fffffff;
    /*
    if (addr < 0x10000000) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "cannot read from address %08x", addr));
    }
    */
    if ((addr & (align - 1)) != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "address %08x is not aligned to %d bytes", addr, align));
    }
    return addr;
}

STATIC uint32_t get_write_addr(mp_obj_t addr_o, uint align) {
    uint32_t addr = mp_obj_get_int(addr_o) & 0x7fffffff;
    if (addr < 0x10000000) {
        // Everything below 0x10000000 is either ROM or aliased to something higher, so we don't
        // lose anything by restricting writes to this area, and we gain some safety.
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "cannot write to address %08x", addr));
    }
    if ((addr & (align - 1)) != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "address %08x is not aligned to %d bytes", addr, align));
    }
    return addr;
}

typedef struct _stm_mem_obj_t {
    mp_obj_base_t base;
    uint32_t elem_size; // in bytes
} stm_mem_obj_t;

STATIC void stm_mem_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    stm_mem_obj_t *self = self_in;
    mp_printf(print, "<%u-bit memory>", 8 * self->elem_size);
}

STATIC mp_obj_t stm_mem_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value) {
    // TODO support slice index to read/write multiple values at once
    stm_mem_obj_t *self = self_in;
    if (value == MP_OBJ_NULL) {
        // delete
        return MP_OBJ_NULL; // op not supported
    } else if (value == MP_OBJ_SENTINEL) {
        // load
        uint32_t addr = get_read_addr(index, self->elem_size);
        uint32_t val;
        switch (self->elem_size) {
            case 1: val = (*(uint8_t*)addr); break;
            case 2: val = (*(uint16_t*)addr); break;
            default: val = (*(uint32_t*)addr); break;
        }
        return mp_obj_new_int(val);
    } else {
        // store
        uint32_t addr = get_write_addr(index, self->elem_size);
        uint32_t val = mp_obj_get_int(value);
        switch (self->elem_size) {
            case 1: (*(uint8_t*)addr) = val; break;
            case 2: (*(uint16_t*)addr) = val; break;
            default: (*(uint32_t*)addr) = val; break;
        }
        return mp_const_none;
    }
}

STATIC const mp_obj_type_t stm_mem_type = {
    { &mp_type_type },
    .name = MP_QSTR_mem,
    .print = stm_mem_print,
    .subscr = stm_mem_subscr,
};

STATIC const stm_mem_obj_t stm_mem8_obj = {{&stm_mem_type}, 1};
STATIC const stm_mem_obj_t stm_mem16_obj = {{&stm_mem_type}, 2};
STATIC const stm_mem_obj_t stm_mem32_obj = {{&stm_mem_type}, 4};

STATIC const mp_map_elem_t stm_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_stm) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_mem8), (mp_obj_t)&stm_mem8_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_mem16), (mp_obj_t)&stm_mem16_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_mem32), (mp_obj_t)&stm_mem32_obj },

#include "genhdr/modstm_const.h"
};

STATIC MP_DEFINE_CONST_DICT(stm_module_globals, stm_module_globals_table);

const mp_obj_module_t stm_module = {
    .base = { &mp_type_module },
    .name = MP_QSTR_stm,
    .globals = (mp_obj_dict_t*)&stm_module_globals,
};
