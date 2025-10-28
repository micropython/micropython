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

#ifndef MICROPY_INCLUDED_MPTHREADPORT_H
#define MICROPY_INCLUDED_MPTHREADPORT_H

#include "helios_os.h"

#if defined(PLAT_EIGEN) || defined(PLAT_EIGEN_718)
#define MP_THREAD_MIN_STACK_SIZE                        (4 * 1024)
#define MP_THREAD_DEFAULT_STACK_SIZE                    (MP_THREAD_MIN_STACK_SIZE + 1024 + 1024)
#define MP_THREAD_PRIORITY                              100
#else
#define MP_THREAD_MIN_STACK_SIZE                        (32 * 1024)
#define MP_THREAD_DEFAULT_STACK_SIZE                    (MP_THREAD_MIN_STACK_SIZE + 1024)
#define MP_THREAD_PRIORITY                              100
#endif


typedef Helios_Mutex_t mp_thread_mutex_t;
typedef Helios_Sem_t mp_thread_semphore_t;


#define QPY_WAIT_FOREVER    HELIOS_WAIT_FOREVER
#define QPY_NO_WAIT         HELIOS_NO_WAIT

void mp_thread_init(void *stack, uint32_t stack_len);
void mp_thread_gc_others(void);
void mp_thread_deinit(void);
unsigned int mp_get_available_memory_size(void);
bool mp_is_python_thread(void);
int mp_thread_get_current_tsknode(void);
int mp_thread_get_tskid_by_tsknode(void *th_node);

void mp_thread_mutex_init(mp_thread_mutex_t *mutex);
int mp_thread_mutex_lock(mp_thread_mutex_t *mutex, int wait);
void mp_thread_mutex_unlock(mp_thread_mutex_t *mutex);
void mp_thread_mutex_del(mp_thread_mutex_t *mutex);

void mp_thread_semphore_init(mp_thread_semphore_t *sem, uint32_t initcount);
int mp_thread_semphore_acquire(mp_thread_semphore_t *sem, int wait);
void mp_thread_semphore_release(mp_thread_semphore_t *sem);
void mp_thread_semphore_del(mp_thread_semphore_t *sem);

void mp_new_thread_add(uint32_t th_id, uint32_t stack_size);

#endif // MICROPY_INCLUDED_MPTHREADPORT_H

