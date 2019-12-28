/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Yonatan Goldschmidt
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

#ifndef MICROPY_INCLUDED_LINUX_KERNEL_MPTHREADPORT_H
#define MICROPY_INCLUDED_LINUX_KERNEL_MPTHREADPORT_H

#if MICROPY_PY_THREAD

#include <linux/spinlock.h>


typedef spinlock_t mp_thread_mutex_t;

void mp_thread_init(void);
void mp_thread_deinit(void);
void mp_thread_gc_others(void);

static inline void mp_thread_mutex_init(mp_thread_mutex_t *m) {
    spin_lock_init(m);
}

static inline int mp_thread_mutex_lock(mp_thread_mutex_t *m, int wait) {
    if (wait) {
        spin_lock(m);
        return 1;
    } else {
        // returns 1 if successfully taken, 0 if busy.
        return spin_trylock(m);
    }
}

static inline void mp_thread_mutex_unlock(mp_thread_mutex_t *m) {
    spin_unlock(m);
}


bool __register_new_thread(struct task_struct *k, void *arg, bool pythread, void *ts);

static inline void register_new_pythread(struct task_struct *k, void *arg) {
    (void)__register_new_thread(k, arg, true, NULL);
}

static inline bool register_new_context(void *ts) {
    return __register_new_thread(current, NULL, false, ts);
}

#endif // MICROPY_PY_THREAD

#endif // MICROPY_INCLUDED_LINUX_KERNEL_MPTHREADPORT_H
