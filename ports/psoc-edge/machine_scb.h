/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Infineon Technologies AG
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

#ifndef MICROPY_INCLUDED_PSOC_EDGE_MACHINE_SCB_H
#define MICROPY_INCLUDED_PSOC_EDGE_MACHINE_SCB_H

#include "sys_int.h"
#include "py/obj.h"

typedef void (*machine_scb_parent_irq_handler_t)(mp_obj_t scb_obj);

typedef struct _machine_scb_obj_t {
    CySCB_Type *scb;
    sys_int_cfg_t irq;
    en_clk_dst_t clk;
    mp_obj_t parent;
    machine_scb_parent_irq_handler_t parent_handler;
} machine_scb_obj_t;

machine_scb_obj_t *machine_scb_obj_alloc(uint8_t scb, mp_obj_t parent, machine_scb_parent_irq_handler_t handler);
void machine_scb_obj_free(machine_scb_obj_t *scb);

#endif // MICROPY_INCLUDED_PSOC_EDGE_MACHINE_SCB_H
