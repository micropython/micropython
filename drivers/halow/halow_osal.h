/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 OpenMV LLC.
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
 * MMOSAL implementation for MicroPython.
 */
#ifndef MICROPY_INCLUDED_DRIVERS_HALOW_HALOW_OSAL_H
#define MICROPY_INCLUDED_DRIVERS_HALOW_HALOW_OSAL_H

#include <stdbool.h>
#include <stddef.h>

// Take the driver's memory pool from the MicroPython heap.  Must be called from
// a context where the GC may run, i.e. not from the scheduler.  Returns false if
// the pool could not be allocated.
bool halow_osal_init(void);

// Allocate from the driver's private heap.  morselib runs from PendSV, where the
// MicroPython GC heap must not be touched.
void *halow_osal_malloc(size_t size);
void halow_osal_free(void *ptr);

// Run any morselib timers that have expired.  Called from halow_poll().
void halow_osal_timer_poll(void);

// Release the memory pool back to the MicroPython heap.  Only safe once
// morselib has been shut down and the scheduler torn down.
void halow_osal_deinit(void);

// True while morselib holds a critical section, i.e. while interrupts are off
// on its behalf.  Nothing may yield in that window.
bool halow_osal_in_critical(void);

#endif // MICROPY_INCLUDED_DRIVERS_HALOW_HALOW_OSAL_H

// Fatal-failure record for post-mortem reads over a debug probe: the console
// often dies with the fault, so the last failure is parked in RAM too.
#define HALOW_FATAL_MAGIC (0x48464154)   // "HFAT"
struct halow_fatal_record {
    uint32_t magic;
    uint32_t pc;
    uint32_t lr;
    uint32_t fileid;
    uint32_t line;
    uint32_t ticks_ms;
};
extern struct halow_fatal_record halow_fatal_record;
