// SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
// SPDX-FileCopyrightText: Copyright (c) 2015 Damien P. George
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_EXTMOD_MACHINE_MEM_H
#define MICROPY_INCLUDED_EXTMOD_MACHINE_MEM_H

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

#endif // MICROPY_INCLUDED_EXTMOD_MACHINE_MEM_H
