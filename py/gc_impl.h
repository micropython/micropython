/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2026 Dryw Wade Clere at SparkFun Electronics
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
#ifndef MICROPY_INCLUDED_PY_GC_IMPL_H
#define MICROPY_INCLUDED_PY_GC_IMPL_H

// Private definitions shared by gc_common.c and the GC implementations.
// Not for use outside the memory manager.

#include "py/gc.h"
#include "py/runtime.h"

#if MICROPY_DEBUG_VALGRIND
#include <valgrind/memcheck.h>
#endif

#if MICROPY_PY_THREAD && !MICROPY_PY_THREAD_GIL
#define GC_MUTEX_INIT() mp_thread_recursive_mutex_init(&MP_STATE_MEM(gc_mutex))
#define GC_ENTER() mp_thread_recursive_mutex_lock(&MP_STATE_MEM(gc_mutex), 1)
#define GC_EXIT() mp_thread_recursive_mutex_unlock(&MP_STATE_MEM(gc_mutex))
#else
// Either no threading, or assume callers to gc_collect() hold the GIL
#define GC_MUTEX_INIT()
#define GC_ENTER()
#define GC_EXIT()
#endif

// Address sanitizer needs to know that the access to ptrs[i] must always be
// considered OK, even if it's a load from an address that would normally be
// prohibited (due to being undefined, in a red zone, etc).
#if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8))
__attribute__((no_sanitize_address))
#endif
static inline void *gc_get_ptr(void **ptrs, size_t i) {
    #if MICROPY_DEBUG_VALGRIND
    if (!VALGRIND_CHECK_MEM_IS_ADDRESSABLE(&ptrs[i], sizeof(*ptrs))) {
        return NULL;
    }
    #endif
    return ptrs[i];
}

// Implemented by each GC implementation: take the GC mutex, set GC_COLLECT_FLAG and
// prepare for a mark phase (or, for gc_sweep_all, a sweep with nothing marked).
void gc_impl_collect_start(bool will_mark);

#endif // MICROPY_INCLUDED_PY_GC_IMPL_H
