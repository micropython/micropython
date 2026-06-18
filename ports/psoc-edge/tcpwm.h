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

#ifndef MICROPY_INCLUDED_PSOC_EDGE_MACHINE_TCPWM_H
#define MICROPY_INCLUDED_PSOC_EDGE_MACHINE_TCPWM_H

#include "py/obj.h"
#include "cy_sysclk.h"
#include <stdbool.h>
#include "genhdr/pins_af.h"

void machine_tcpwm_counter_alloc(uint32_t counter_num, mp_obj_t owner);
bool machine_tcpwm_counter_try_alloc(uint32_t counter_num, mp_obj_t owner);
void machine_tcpwm_counter_free(uint32_t counter_num, mp_obj_t owner);
en_clk_dst_t machine_tcpwm_counter_pclk(uint32_t counter_num);

#endif // MICROPY_INCLUDED_PSOC_EDGE_MACHINE_TCPWM_H
