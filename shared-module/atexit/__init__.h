// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "shared/runtime/pyexec.h"

typedef struct _atexit_callback_t {
    size_t n_pos, n_kw;
    mp_obj_t func, *args;
} atexit_callback_t;

extern void atexit_reset(void);
extern void atexit_gc_collect(void);

extern void shared_module_atexit_register(mp_obj_t *func,
    size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern void shared_module_atexit_unregister(const mp_obj_t *func);
extern void shared_module_atexit_execute(pyexec_result_t *result);
