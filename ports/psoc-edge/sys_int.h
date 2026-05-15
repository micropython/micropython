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

#ifndef MICROPY_INCLUDED_PSOC_EDGE_SYS_INT_H
#define MICROPY_INCLUDED_PSOC_EDGE_SYS_INT_H

#include "cy_sysint.h"
#include "cy_device_headers.h"

/**
 * Define the lowest interrupt priority.
 * The cy_device_headers_ns.h file might
 * define the interrupt priority levels.
 */
#if defined(__NVIC_PRIO_BITS)
    #define SYS_INT_IRQ_LOWEST_PRIORITY ((1 << __NVIC_PRIO_BITS) - 1)
#else
    #define SYS_INT_IRQ_LOWEST_PRIORITY 7
#endif

typedef struct _sys_int_cfg_t {
    IRQn_Type irq_num;
    uint32_t priority;
    cy_israddress handler;
} sys_int_cfg_t;

void sys_int_init(sys_int_cfg_t *int_cfg);
void sys_int_deinit(sys_int_cfg_t *int_cfg);
void sys_int_enable(sys_int_cfg_t *int_cfg);
void sys_int_disable(sys_int_cfg_t *int_cfg);
void sys_int_clear(sys_int_cfg_t *int_cfg);

#endif // MICROPY_INCLUDED_PSOC_EDGE_SYS_INT_H
