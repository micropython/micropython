// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"


void memorymonitor_exception_print(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind);

#define MP_DEFINE_MEMORYMONITOR_EXCEPTION(exc_name, base_name) \
    const mp_obj_type_t mp_type_memorymonitor_##exc_name = { \
        { &mp_type_type }, \
        .name = MP_QSTR_##exc_name, \
        .print = memorymonitor_exception_print, \
        .make_new = mp_obj_exception_make_new, \
        .attr = mp_obj_exception_attr, \
        .parent = &mp_type_##base_name, \
    };

extern const mp_obj_type_t mp_type_memorymonitor_AllocationError;

NORETURN void mp_raise_memorymonitor_AllocationError(mp_rom_error_text_t msg, ...);
