/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
 * Copyright (c) 2022-2024 Infineon Technologies AG
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
#ifndef MICROPY_INCLUDED_PSOCEDGE_MODPSOCEDGE_H
#define MICROPY_INCLUDED_PSOCEDGE_MODPSOCEDGE_H

// micropython includes
#include "py/obj.h"

// Return values for psoc_edge.system_reset_cause().
typedef enum {
    SYSTEM_RESET_NONE,             // no identifiable cause (power-on)
    SYSTEM_RESET_WDT,              // WDT reset (CY_SYSLIB_RESET_HWWDT)
    SYSTEM_RESET_DEEPSLEEP_FAULT,  // deep-sleep fault
    SYSTEM_RESET_SOFT,             // NVIC_SystemReset() or physical reset pin
} system_reset_reason_t;

extern const mp_obj_type_t psoc_edge_qspi_flash_type;

mp_obj_t psocedge_system_reset_cause(void);

#endif // MICROPY_INCLUDED_PSOCEDGE_MODPSOCEDGE_H
