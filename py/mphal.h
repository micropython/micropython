/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Damien P. George
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
#ifndef __MICROPY_INCLUDED_PY_MPHAL_H__
#define __MICROPY_INCLUDED_PY_MPHAL_H__

#include "py/mpconfig.h"

#ifdef MICROPY_MPHALPORT_H
#include MICROPY_MPHALPORT_H
#else
#include <mphalport.h>
#endif

#ifndef mp_hal_stdin_rx_chr
int mp_hal_stdin_rx_chr(void);
#endif

#ifndef mp_hal_stdout_tx_str
void mp_hal_stdout_tx_str(const char *str);
#endif

#ifndef mp_hal_stdout_tx_strn
void mp_hal_stdout_tx_strn(const char *str, size_t len);
#endif

#ifndef mp_hal_stdout_tx_strn_cooked
void mp_hal_stdout_tx_strn_cooked(const char *str, size_t len);
#endif

#ifndef mp_hal_delay_ms
void mp_hal_delay_ms(mp_uint_t ms);
#endif

#ifndef mp_hal_delay_us
void mp_hal_delay_us(mp_uint_t us);
#endif

#ifndef mp_hal_ticks_ms
mp_uint_t mp_hal_ticks_ms(void);
#endif

#ifndef mp_hal_ticks_us
mp_uint_t mp_hal_ticks_us(void);
#endif

#ifndef mp_hal_ticks_cpu
mp_uint_t mp_hal_ticks_cpu(void);
#endif

// If port HAL didn't define its own pin API, use generic
// "virtual pin" API from the core.
#ifndef mp_hal_pin_obj_t
#define mp_hal_pin_obj_t mp_obj_t
#define mp_hal_get_pin_obj(pin) (pin)
#define mp_hal_pin_read(pin) mp_virtual_pin_read(pin)
#define mp_hal_pin_write(pin, v) mp_virtual_pin_write(pin, v)
#include "extmod/virtpin.h"
#endif

#endif // __MICROPY_INCLUDED_PY_MPHAL_H__
