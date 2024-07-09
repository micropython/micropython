// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Noralf Trønnes
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    qstr name;
    uint8_t type;
    uint8_t index;
} samd_clock_obj_t;

#define CLOCK(_name, _type, _index) \
    const samd_clock_obj_t clock_##_name = { \
        { &samd_clock_type }, \
        .name = MP_QSTR_##_name, \
        .type = _type, \
        .index = _index, \
    }

#define CLOCK_SOURCE(_name) \
    const samd_clock_obj_t clock_##_name = { \
        { &samd_clock_type }, \
        .name = MP_QSTR_##_name, \
        .type = 0, \
        .index = GCLK_SOURCE_##_name, \
    }

#define CLOCK_GCLK(_name) \
    const samd_clock_obj_t clock_##_name = { \
        { &samd_clock_type }, \
        .name = MP_QSTR_##_name, \
        .type = 1, \
        .index = _name##_GCLK_ID, \
    }

#define CLOCK_GCLK_(_name, _extra) \
    const samd_clock_obj_t clock_##_name##_##_extra = { \
        { &samd_clock_type }, \
        .name = MP_QSTR_##_name##_##_extra, \
        .type = 1, \
        .index = _name##_GCLK_ID_##_extra, \
    }

#define CLOCK_ENTRY(_name)  { MP_ROM_QSTR(MP_QSTR_##_name), MP_ROM_PTR(&clock_##_name) }
#define CLOCK_ENTRY_(_name, _extra)  { MP_ROM_QSTR(MP_QSTR_##_name##_##_extra), MP_ROM_PTR(&clock_##_name##_##_extra) }

extern const mp_obj_type_t samd_clock_type;
extern const mp_obj_dict_t samd_clock_globals;
