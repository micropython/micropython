/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2017 Damien P. George
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
#ifndef MICROPY_INCLUDED_PY_PYSTACK_H
#define MICROPY_INCLUDED_PY_PYSTACK_H

#include "py/mpstate.h"

// Enable this debugging option to check that the amount of memory freed is
// consistent with amounts that were previously allocated.
#define MP_PYSTACK_DEBUG (0)

#if MICROPY_ENABLE_PYSTACK

void mp_pystack_init(void *start, void *end);
void *mp_pystack_alloc(size_t n_bytes);

// This function can free multiple continuous blocks at once: just pass the
// pointer to the block that was allocated first and it and all subsequently
// allocated blocks will be freed.
static inline void mp_pystack_free(void *ptr) {
    assert((uint8_t *)ptr >= MP_STATE_THREAD(pystack_start));
    assert((uint8_t *)ptr <= MP_STATE_THREAD(pystack_cur));
    #if MP_PYSTACK_DEBUG
    size_t n_bytes_to_free = MP_STATE_THREAD(pystack_cur) - (uint8_t *)ptr;
    size_t n_bytes = *(size_t *)(MP_STATE_THREAD(pystack_cur) - MICROPY_PYSTACK_ALIGN);
    while (n_bytes < n_bytes_to_free) {
        n_bytes += *(size_t *)(MP_STATE_THREAD(pystack_cur) - n_bytes - MICROPY_PYSTACK_ALIGN);
    }
    if (n_bytes != n_bytes_to_free) {
        mp_printf(&mp_plat_print, "mp_pystack_free() failed: %u != %u\n", (uint)n_bytes_to_free,
            (uint)*(size_t *)(MP_STATE_THREAD(pystack_cur) - MICROPY_PYSTACK_ALIGN));
        assert(0);
    }
    #endif
    MP_STATE_THREAD(pystack_cur) = (uint8_t *)ptr;
}

static inline void mp_pystack_realloc(void *ptr, size_t n_bytes) {
    mp_pystack_free(ptr);
    mp_pystack_alloc(n_bytes);
}

static inline size_t mp_pystack_usage(void) {
    return MP_STATE_THREAD(pystack_cur) - MP_STATE_THREAD(pystack_start);
}

static inline size_t mp_pystack_limit(void) {
    return MP_STATE_THREAD(pystack_end) - MP_STATE_THREAD(pystack_start);
}

#endif

#if !MICROPY_ENABLE_PYSTACK

#define mp_local_alloc(n_bytes) alloca(n_bytes)

static inline void mp_local_free(void *ptr) {
    (void)ptr;
}

static inline void *mp_nonlocal_alloc(size_t n_bytes) {
    return m_new(uint8_t, n_bytes);
}

static inline void *mp_nonlocal_realloc(void *ptr, size_t old_n_bytes, size_t new_n_bytes) {
    return m_renew(uint8_t, ptr, old_n_bytes, new_n_bytes);
}

static inline void mp_nonlocal_free(void *ptr, size_t n_bytes) {
    m_del(uint8_t, ptr, n_bytes);
}

#else

static inline void *mp_local_alloc(size_t n_bytes) {
    return mp_pystack_alloc(n_bytes);
}

static inline void mp_local_free(void *ptr) {
    mp_pystack_free(ptr);
}

static inline void *mp_nonlocal_alloc(size_t n_bytes) {
    return mp_pystack_alloc(n_bytes);
}

static inline void *mp_nonlocal_realloc(void *ptr, size_t old_n_bytes, size_t new_n_bytes) {
    (void)old_n_bytes;
    mp_pystack_realloc(ptr, new_n_bytes);
    return ptr;
}

static inline void mp_nonlocal_free(void *ptr, size_t n_bytes) {
    (void)n_bytes;
    mp_pystack_free(ptr);
}

#endif

#endif // MICROPY_INCLUDED_PY_PYSTACK_H
