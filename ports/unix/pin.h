/*
 * This file is part of the MicroPython project, https://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Alessandro Gatti
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

#ifndef MICROPY_INCLUDED_UNIX_PIN_H
#define MICROPY_INCLUDED_UNIX_PIN_H

#include "py/mphal.h"

#if MICROPY_PY_GPIO

#ifndef __linux__
#error "Platform does not support GPIO access"
#endif

#include <stdint.h>

#if MICROPY_PY_GPIO_IRQ
#include <sys/epoll.h>
#endif

#include "py/obj.h"
#include "py/runtime.h"

extern const mp_obj_type_t machine_pin_type;

typedef struct {
    mp_obj_base_t base;
    mp_obj_t port;
    int fd;
    uint32_t number;
    #if MICROPY_PY_GPIO_IRQ
    struct epoll_event event;
    mp_obj_t callback;
    bool hard_callback;
    #endif
} machine_pin_obj_t;

void mp_pin_init(void);
void mp_pin_deinit(void);

#endif

#endif
