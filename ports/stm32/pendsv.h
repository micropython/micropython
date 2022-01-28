/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#ifndef MICROPY_INCLUDED_STM32_PENDSV_H
#define MICROPY_INCLUDED_STM32_PENDSV_H

#include "boardctrl.h"

enum {
    PENDSV_DISPATCH_SOFT_TIMER,
    #if MICROPY_PY_NETWORK && MICROPY_PY_LWIP
    PENDSV_DISPATCH_LWIP,
    #if MICROPY_PY_NETWORK_CYW43
    PENDSV_DISPATCH_CYW43,
    #endif
    #endif
    #if MICROPY_PY_BLUETOOTH && !MICROPY_PY_BLUETOOTH_USE_SYNC_EVENTS
    PENDSV_DISPATCH_BLUETOOTH_HCI,
    #endif
    MICROPY_BOARD_PENDSV_ENTRIES
    PENDSV_DISPATCH_MAX
};

#define PENDSV_DISPATCH_NUM_SLOTS PENDSV_DISPATCH_MAX

typedef void (*pendsv_dispatch_t)(void);

void pendsv_init(void);
void pendsv_kbd_intr(void);
void pendsv_schedule_dispatch(size_t slot, pendsv_dispatch_t f);

#endif // MICROPY_INCLUDED_STM32_PENDSV_H
