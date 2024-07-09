// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    int16_t value;
    int16_t name;
} cp_enum_obj_t;

#define MAKE_ENUM_VALUE(type, prefix, name, value) \
    const cp_enum_obj_t prefix##_##name##_obj = {  \
        { &type }, value, MP_QSTR_##name, \
    }

#define MAKE_ENUM_MAP(name) \
    const mp_rom_map_elem_t name##_locals_table[] =

#define MAKE_ENUM_MAP_ENTRY(prefix, name) \
    { MP_ROM_QSTR(MP_QSTR_##name), MP_ROM_PTR(&prefix##_##name##_obj) }

#define MAKE_PRINTER(module, typename) \
    static void typename##_##print(const mp_print_t * print, mp_obj_t self_in, mp_print_kind_t kind) { \
        cp_enum_obj_print_helper(MP_QSTR_##module, print, self_in, kind); \
    }

#define MAKE_ENUM_TYPE(module, type, typename, ...) \
    MP_DEFINE_CONST_OBJ_TYPE(typename##_type, \
    MP_QSTR_##type, \
    0, \
    print, typename##_print, \
    locals_dict, &typename##_locals_dict, \
    ##__VA_ARGS__ \
    )

mp_obj_t cp_enum_find(const mp_obj_type_t *type, int value);
int cp_enum_value(const mp_obj_type_t *type, mp_obj_t obj, qstr arg_name);
void cp_enum_obj_print_helper(uint16_t module, const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
