/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Noralf Trønnes
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_WDT_WDT_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_WDT_WDT_H

#include <stdint.h>
#include <stdbool.h>

extern void common_hal_wdt_init(uint32_t duration, bool pause_during_sleep);
extern void common_hal_wdt_feed(void);
extern void common_hal_wdt_disable(void);

extern const mp_obj_type_t wdt_wdt_type;

typedef struct _wdt_wdt_obj_t {
    mp_obj_base_t base;
    uint32_t timeout;
    bool sleep;
} wdt_wdt_obj_t;

extern const wdt_wdt_obj_t wdt_wdt_obj;

#endif  // MICROPY_INCLUDED_SHARED_BINDINGS_WDT_WDT_H
