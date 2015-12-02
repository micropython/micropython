/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014, 2015 Damien P. George
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

#include "machine_mem.h"

#include <stdio.h>
#include <stdint.h>

#include "py/nlr.h"

#if MICROPY_PLAT_DEV_MEM
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#define MICROPY_PAGE_SIZE 4096
#define MICROPY_PAGE_MASK (MICROPY_PAGE_SIZE - 1)
#endif

#if MICROPY_PY_MACHINE

STATIC uintptr_t get_addr(mp_obj_t addr_o, uint align) {
    uintptr_t addr = mp_obj_int_get_truncated(addr_o);
    if ((addr & (align - 1)) != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "address %08x is not aligned to %d bytes", addr, align));
    }
    #if MICROPY_PLAT_DEV_MEM
    {
        // Not thread-safe
        static int fd;
        static uintptr_t last_base = (uintptr_t)-1;
        static uintptr_t map_page;
        if (!fd) {
            fd = open("/dev/mem", O_RDWR | O_SYNC);
            if (fd == -1) {
                nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(errno)));
            }
        }

        uintptr_t cur_base = addr & ~MICROPY_PAGE_MASK;
        if (cur_base != last_base) {
            map_page = (uintptr_t)mmap(NULL, MICROPY_PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, cur_base);
            last_base = cur_base;
        }
        addr = map_page + (addr & MICROPY_PAGE_MASK);
    }
    #else
    if (MP_OBJ_IS_SMALL_INT(addr_o)) {
        // TODO: This should probably be based on MCIROPY_OBJ_REPR
        addr &= 0x7fffffff;
    }
    #endif

    return addr;
}

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
        uintptr_t addr = get_addr(index, self->elem_size);
        uint32_t val;
        switch (self->elem_size) {
            case 1: val = (*(uint8_t*)addr); break;
            case 2: val = (*(uint16_t*)addr); break;
            default: val = (*(uint32_t*)addr); break;
        }
        return mp_obj_new_int(val);
    } else {
        // store
        uintptr_t addr = get_addr(index, self->elem_size);
        #if defined(MICROPY_MACHINE_MEM_MIN_WRITABLE_ADDR)
        if (addr < MICROPY_MACHINE_MEM_MIN_WRITABLE_ADDR) {
            // Everything below MICROPY_MACHINE_MEM_MIN_WRITABLE_ADDR is either ROM or aliased to something higher, so we don't
            // lose anything by restricting writes to this area, and we gain some safety.
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "cannot write to address %08x", addr));
        }
        #endif

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
