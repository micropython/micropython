/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Daniel Campora
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
#ifndef MICROPY_INCLUDED_LIB_UTILS_MPIRQ_H
#define MICROPY_INCLUDED_LIB_UTILS_MPIRQ_H

#include "py/runtime.h"

/******************************************************************************
 DEFINE CONSTANTS
 ******************************************************************************/

enum {
    MP_IRQ_ARG_INIT_handler = 0,
    MP_IRQ_ARG_INIT_trigger,
    MP_IRQ_ARG_INIT_hard,
    MP_IRQ_ARG_INIT_NUM_ARGS,
};

/******************************************************************************
 DEFINE TYPES
 ******************************************************************************/

typedef mp_uint_t (*mp_irq_trigger_fun_t)(mp_obj_t self, mp_uint_t trigger);
typedef mp_uint_t (*mp_irq_info_fun_t)(mp_obj_t self, mp_uint_t info_type);

enum {
    MP_IRQ_INFO_FLAGS,
    MP_IRQ_INFO_TRIGGERS,
};

typedef struct _mp_irq_methods_t {
    mp_irq_trigger_fun_t trigger;
    mp_irq_info_fun_t info;
} mp_irq_methods_t;

typedef struct _mp_irq_obj_t {
    mp_obj_base_t base;
    mp_irq_methods_t *methods;
    mp_obj_t parent;
    mp_obj_t handler;
    bool ishard;
} mp_irq_obj_t;

/******************************************************************************
 DECLARE EXPORTED DATA
 ******************************************************************************/

extern const mp_arg_t mp_irq_init_args[];
extern const mp_obj_type_t mp_irq_type;

/******************************************************************************
 DECLARE PUBLIC FUNCTIONS
 ******************************************************************************/

mp_irq_obj_t *mp_irq_new(const mp_irq_methods_t *methods, mp_obj_t parent);
void mp_irq_init(mp_irq_obj_t *self, const mp_irq_methods_t *methods, mp_obj_t parent);
void mp_irq_handler(mp_irq_obj_t *self);

#endif // MICROPY_INCLUDED_LIB_UTILS_MPIRQ_H
