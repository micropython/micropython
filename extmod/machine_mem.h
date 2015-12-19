/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Damien P. George
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


#ifndef __MICROPY_INCLUDED_EXTMOD_MACHINE_MEM_H__
#define __MICROPY_INCLUDED_EXTMOD_MACHINE_MEM_H__

#include "py/obj.h"

typedef struct _machine_mem_obj_t {
    mp_obj_base_t base;
    unsigned elem_size; // in bytes
} machine_mem_obj_t;

extern const mp_obj_type_t machine_mem_type;

extern const machine_mem_obj_t machine_mem8_obj;
extern const machine_mem_obj_t machine_mem16_obj;
extern const machine_mem_obj_t machine_mem32_obj;

#if defined(MICROPY_MACHINE_MEM_GET_READ_ADDR)
uintptr_t MICROPY_MACHINE_MEM_GET_READ_ADDR(mp_obj_t addr_o, uint align);
#endif
#if defined(MICROPY_MACHINE_MEM_GET_WRITE_ADDR)
uintptr_t MICROPY_MACHINE_MEM_GET_WRITE_ADDR(mp_obj_t addr_o, uint align);
#endif

#endif // __MICROPY_INCLUDED_EXTMOD_MACHINE_MEM_H__
