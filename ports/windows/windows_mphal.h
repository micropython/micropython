/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#include "sleep.h"
#include "ports/unix/mphalport.h"

// Don't use the unix version of this macro.
#undef MICROPY_INTERNAL_WFE

#if MICROPY_ENABLE_SCHEDULER
// Use minimum 1mSec sleep to make sure there is effectively a wait period:
// something like usleep(500) truncates and ends up calling Sleep(0).
#define MICROPY_INTERNAL_WFE(TIMEOUT_MS) msec_sleep(MAX(1.0, (double)(TIMEOUT_MS)))
#else
#define MICROPY_INTERNAL_WFE(TIMEOUT_MS) /* No-op */
#endif

#define MICROPY_HAL_HAS_VT100 (0)

void mp_hal_move_cursor_back(unsigned int pos);
void mp_hal_erase_line_from_cursor(unsigned int n_chars_to_erase);

#undef mp_hal_ticks_cpu
mp_uint_t mp_hal_ticks_cpu(void);
