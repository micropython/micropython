/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd
 * Copyright (c) 2017 Pycom Limited
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

#include "stdio.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "wm_osal.h"
#include "wm_watchdog.h"

#include "py/mpconfig.h"
#include "py/mpstate.h"
#include "py/gc.h"
#include "py/mpthread.h"
#include "mpthreadport.h"

#if MICROPY_PY_THREAD

#define MP_THREAD_MIN_STACK_SIZE                        (2 * 1024)
#define MP_THREAD_DEFAULT_STACK_SIZE                    (MP_THREAD_MIN_STACK_SIZE + 2048)

// this structure forms a linked list, one node per active thread
typedef struct _thread_t {
    xTaskHandle id;        // system id of thread
    int ready;              // whether the thread is ready and running
    void *arg;              // thread Python args, a GC root pointer
    void *stack;            // pointer to the stack
    size_t stack_len;       // number of words in the stack
    struct _thread_t *next;
    void *p;
} thread_t;

// the mutex controls access to the linked list
STATIC mp_thread_mutex_t thread_mutex;
STATIC thread_t thread_entry0;
STATIC thread_t *thread; // root pointer, handled bp mp_thread_gc_others


void mp_thread_init(void *stack, uint32_t stack_len) {
    mp_thread_mutex_init(&thread_mutex);

    // create first entry in linked list of all threads
    thread = &thread_entry0;
    thread->id = xTaskGetCurrentTaskHandle();
    thread->ready = 1;
    thread->arg = NULL;
    thread->stack = stack;
    thread->stack_len = stack_len;
    thread->next = NULL;

    mp_thread_set_state(&mp_state_ctx.thread);

}

void mp_thread_gc_others(void) {
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (thread_t *th = thread; th != NULL; th = th->next) {
        gc_collect_root((void **)&th, 1);
        gc_collect_root(&th->arg, 1); // probably not needed
        if (th->id == xTaskGetCurrentTaskHandle()) {
            continue;
        }
        if (!th->ready) {
            continue;
        }
        gc_collect_root(th->stack, th->stack_len);
    }
    mp_thread_mutex_unlock(&thread_mutex);
}

mp_state_thread_t *mp_thread_get_state(void) {
    mp_state_thread_t *p;
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (thread_t *th = thread; th != NULL; th = th->next) {
        if (th->id == xTaskGetCurrentTaskHandle()) {
            p = th->p;
            break;
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
    return (mp_state_thread_t *)p;
}

void mp_thread_set_state(void *state) {
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (thread_t *th = thread; th != NULL; th = th->next) {
        if (th->id == xTaskGetCurrentTaskHandle()) {
            th->p = state;
            break;
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
}

void mp_thread_start(void) {
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (thread_t *th = thread; th != NULL; th = th->next) {
        if (th->id == xTaskGetCurrentTaskHandle()) {
            th->ready = 1;
            break;
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
}

STATIC void *(*ext_thread_entry)(void *) = NULL;

STATIC void freertos_entry(void *arg) {
    if (ext_thread_entry) {
        ext_thread_entry(arg);
    }
    vTaskSuspend(NULL);
    // Never get to the following lines
    for (;;) {
    }
}

void mp_thread_create(void *(*entry)(void *), void *arg, size_t *stack_size) {
    // store thread entry function into a global variable so we can access it
    ext_thread_entry = entry;

    if (*stack_size == 0) {
        *stack_size = MP_THREAD_DEFAULT_STACK_SIZE; // default stack size
    } else if (*stack_size < MP_THREAD_MIN_STACK_SIZE) {
        *stack_size = MP_THREAD_MIN_STACK_SIZE; // minimum stack size
    }

    // allocate TCB, stack and linked-list node (must be outside thread_mutex lock)
    OS_STK *stack = m_new(OS_STK, *stack_size / sizeof(OS_STK)); 
    thread_t *th = m_new_obj(thread_t);

    mp_thread_mutex_lock(&thread_mutex, 1);

    // create thread
    xTaskHandle id = NULL;
    u8 err = tls_os_task_create(&id, "MPY_Thread",
                                freertos_entry,
                                (void *)arg,
                                (void *)stack, // Bottom of the stack  任务栈的起始地址 
                                *stack_size, // Size of the stack in bytes 任务栈的大小
                                MPY_TASK_PRIO,
                                0);
    if (id == NULL) {
        mp_thread_mutex_unlock(&thread_mutex);
        mp_raise_msg(&mp_type_OSError, "can't create thread");
    }

    // add thread to linked list of all threads
    th->id = id;
    th->ready = 0;
    th->arg = arg;
    th->stack = stack;
    th->stack_len = *stack_size / sizeof(OS_STK);
    th->next = thread;
    th->p = NULL;
    thread = th;

    // adjust stack_size to provide room to recover from hitting the limit
    *stack_size -= 1024;
    
    mp_thread_mutex_unlock(&thread_mutex);
}

void mp_thread_finish(void) {
    mp_thread_mutex_lock(&thread_mutex, 1);
    // TODO unlink from list
    for (thread_t *th = thread; th != NULL; th = th->next) {
        if (th->id == xTaskGetCurrentTaskHandle()) {
            th->ready = 0;
            break;
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
}

void mp_thread_deinit(void) {
    // If more than the main thread exists, do a hard reset. 
    // That will also end all threads, so cleaning is not required. 
    // And free'ing the heap was anyhow not yet implemented
    // If it is just the main thread, there is nothing to clean
    if (thread && thread != &thread_entry0) {
        tls_sys_reset();
    }
}

void mp_thread_mutex_init(mp_thread_mutex_t *mutex) {
    tls_os_mutex_create(0, &mutex->sem);
}

// To allow hard interrupts to work with threading we only take/give the semaphore
// if we are not within an interrupt context and interrupts are enabled.

int mp_thread_mutex_lock(mp_thread_mutex_t *mutex, int wait) {
    //return (pdTRUE == xSemaphoreTake(mutex->sem, wait ? portMAX_DELAY : 0));
    return (TLS_OS_SUCCESS == tls_os_mutex_acquire(mutex->sem, wait ? 0 : 1));
}

void mp_thread_mutex_unlock(mp_thread_mutex_t *mutex) {
    tls_os_mutex_release(mutex->sem);
}

#endif // MICROPY_PY_THREAD

