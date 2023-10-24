/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd
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
#ifndef MICROPY_INCLUDED_PY_MPTHREAD_H
#define MICROPY_INCLUDED_PY_MPTHREAD_H

#include <stdbool.h>

#include "py/mpconfig.h"

#if MICROPY_PY_THREAD

struct _mp_state_thread_t;

#ifdef MICROPY_MPTHREADPORT_H
#include MICROPY_MPTHREADPORT_H
#else
#include <mpthreadport.h>
#endif

struct _mp_state_thread_t *mp_thread_get_state(void);
void mp_thread_set_state(struct _mp_state_thread_t *state);
mp_uint_t mp_thread_create(void *(*entry)(void *), void *arg, size_t *stack_size);
mp_uint_t mp_thread_get_id(void);
void mp_thread_start(void);
void mp_thread_finish(void);

void mp_thread_mutex_init(mp_thread_mutex_t *mutex);
void mp_thread_mutex_init_recursive(mp_thread_mutex_t *mutex);
int mp_thread_mutex_lock(mp_thread_mutex_t *mutex, int wait);
void mp_thread_mutex_unlock(mp_thread_mutex_t *mutex);

void mp_thread_sem_init(mp_thread_sem_t *sem, mp_uint_t value);
bool mp_thread_sem_wait(mp_thread_sem_t *sem, bool wait);
void mp_thread_sem_post(mp_thread_sem_t *sem);
int mp_thread_sem_value(mp_thread_sem_t *sem);

#else // !MICROPY_PY_THREAD

// When threading is disabled, provide no-op implementation of
// mp_thread_mutex_t and mp_thread_sem_t.

typedef struct {
} mp_thread_mutex_t;

static inline void mp_thread_mutex_init(mp_thread_mutex_t *mutex) {
}
static inline void mp_thread_mutex_init_recursive(mp_thread_mutex_t *mutex) {
}
static inline int mp_thread_mutex_lock(mp_thread_mutex_t *mutex, int wait) {
    return 1;
}
static inline void mp_thread_mutex_unlock(mp_thread_mutex_t *mutex) {
}

typedef struct {
    volatile mp_uint_t value;
} mp_thread_sem_t;

static inline void mp_thread_sem_init(mp_thread_sem_t *sem, mp_uint_t value) {
    sem->value = value;
}
static inline bool mp_thread_sem_wait(mp_thread_sem_t *sem, bool wait) {
    if (wait) {
        while (sem->value == 0) {
        }
        --sem->value;
        return true;
    } else {
        if (sem->value == 0) {
            return false;
        } else {
            --sem->value;
            return true;
        }
    }
}
static inline void mp_thread_sem_post(mp_thread_sem_t *sem) {
    ++sem->value;
}
static inline int mp_thread_sem_value(mp_thread_sem_t *sem) {
    return sem->value;
}

#endif // MICROPY_PY_THREAD

#if MICROPY_PY_THREAD && MICROPY_PY_THREAD_GIL
#include "py/mpstate.h"
#define MP_THREAD_GIL_ENTER() mp_thread_mutex_lock(&MP_STATE_VM(gil_mutex), 1)
#define MP_THREAD_GIL_EXIT() mp_thread_mutex_unlock(&MP_STATE_VM(gil_mutex))
#else
#define MP_THREAD_GIL_ENTER()
#define MP_THREAD_GIL_EXIT()
#endif

#if MICROPY_PY_THREAD && MICROPY_PY_THREAD_RTOS
// Helper functions to assist with RTOS (or OS) threads/tasks that want to
// call into the MicroPython VM. For example, a Unix pthread or FreeRTOS that
// wants to execute Python code on a MicroPython thread. For example, on ESP32
// the BLE host stack runs in a FreeRTOS task.
typedef void (*mp_run_on_thread_function_t)(void *arg);
void mp_thread_run_on_mp_thread(const mp_run_on_thread_function_t fn, void *arg, mp_uint_t stack_size);
#else
// When not using an RTOS (e.g. bare-metal STM32), all threads are MicroPython
// threads.
#define mp_thread_run_on_mp_thread(fn, arg, stack_size) fn(arg)
#endif

#endif // MICROPY_INCLUDED_PY_MPTHREAD_H
