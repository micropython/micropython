/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Arduino SA
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
 *
 * MicroPython libmetal config.
 */
#ifndef MICROPY_INCLUDED_METAL_MICROPYTHON_H
#define MICROPY_INCLUDED_METAL_MICROPYTHON_H

#include <stdlib.h>

// Port-specific config.
#include "mpmetalport.h"

// Library major version number.
#define METAL_VER_MAJOR     1

// Library minor version number.
#define METAL_VER_MINOR     5

// Library patch level.
#define METAL_VER_PATCH     0

// Library version string.
#define METAL_VER           "1.5.0"

#if METAL_HAVE_STDATOMIC_H
#define HAVE_STDATOMIC_H
#endif

#if METAL_HAVE_FUTEX_H
#define HAVE_FUTEX_H
#endif

#ifndef METAL_MAX_DEVICE_REGIONS
#define METAL_MAX_DEVICE_REGIONS 1
#endif

static inline void *__metal_allocate_memory(unsigned int size) {
    return m_tracked_calloc(1, size);
}

static inline void __metal_free_memory(void *ptr) {
    m_tracked_free(ptr);
}

// The following functions must be provided by the port (in mpmetalport.h / mpmetalport.c).
int __metal_sleep_usec(unsigned int usec);
void sys_irq_enable(unsigned int vector);
void sys_irq_disable(unsigned int vector);
void sys_irq_restore_enable(unsigned int flags);
unsigned int sys_irq_save_disable(void);
#endif // MICROPY_INCLUDED_METAL_MICROPYTHON_H
