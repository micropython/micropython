/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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
#ifndef MICROPY_INCLUDED_RP2_MPTHREADPORT_H
#define MICROPY_INCLUDED_RP2_MPTHREADPORT_H

#include "mutex_extra.h"

typedef struct mutex mp_thread_mutex_t;
typedef recursive_mutex_nowait_t mp_thread_recursive_mutex_t;

extern void *core_state[2];

typedef void *(*core_entry_func_t)(void *);

extern core_entry_func_t core1_entry;

void mp_thread_init(void);
void mp_thread_deinit(void);
void mp_thread_gc_others(void);

static inline void mp_thread_set_state(struct _mp_state_thread_t *state) {
    core_state[get_core_num()] = state;
}

static inline struct _mp_state_thread_t *mp_thread_get_state(void) {
    return (struct _mp_state_thread_t *)core_state[get_core_num()];
}

static inline void mp_thread_mutex_init(mp_thread_mutex_t *m) {
    mutex_init(m);
}

static inline int mp_thread_mutex_lock(mp_thread_mutex_t *m, int wait) {
    if (wait) {
        mutex_enter_blocking(m);
        return 1;
    } else {
        return mutex_try_enter(m, NULL);
    }
}

static inline void mp_thread_mutex_unlock(mp_thread_mutex_t *m) {
    mutex_exit(m);
}

static inline void mp_thread_recursive_mutex_init(mp_thread_recursive_mutex_t *m) {
    recursive_mutex_nowait_init(m);
}

static inline int mp_thread_recursive_mutex_lock(mp_thread_recursive_mutex_t *m, int wait) {
    if (wait) {
        recursive_mutex_nowait_enter_blocking(m);
        return 1;
    } else {
        return recursive_mutex_nowait_try_enter(m, NULL);
    }
}

static inline void mp_thread_recursive_mutex_unlock(mp_thread_recursive_mutex_t *m) {
    recursive_mutex_nowait_exit(m);
}

#endif // MICROPY_INCLUDED_RP2_MPTHREADPORT_H
