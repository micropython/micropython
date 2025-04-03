/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 OpenMV LLC.
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
#ifndef MICROPY_INCLUDED_ALIF_SYSTEM_TICK_H
#define MICROPY_INCLUDED_ALIF_SYSTEM_TICK_H

#include "py/mpconfig.h"

#if MICROPY_HW_SYSTEM_TICK_USE_LPTIMER
extern uint64_t system_tick_source_hz;
#elif MICROPY_HW_SYSTEM_TICK_USE_UTIMER
extern uint64_t system_core_clock_mhz;
#endif

void system_tick_init(void);
uint32_t system_tick_get_u32(void);
uint64_t system_tick_get_u64(void);
void system_tick_wfe_with_timeout_us(uint32_t timeout_us);
void system_tick_schedule_after_us(uint32_t ticks_us);
void system_tick_schedule_callback(void);

#endif // MICROPY_INCLUDED_ALIF_SYSTEM_TICK_H
