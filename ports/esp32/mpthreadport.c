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

#include "py/runtime.h"
#include "py/gc.h"
#include "py/mpthread.h"
#include "py/mphal.h"
#include "mpthreadport.h"

#include "esp_task.h"

#if MICROPY_PY_THREAD

#define MP_THREAD_MIN_STACK_SIZE                        (4 * 1024)
#define MP_THREAD_DEFAULT_STACK_SIZE                    (MP_THREAD_MIN_STACK_SIZE + MICROPY_STACK_CHECK_MARGIN)
#define MP_THREAD_PRIORITY                              (ESP_TASK_PRIO_MIN + 1)

// this structure forms a linked list, one node per active thread
typedef struct _mp_thread_t {
    TaskHandle_t id;        // system id of thread
    int ready;              // whether the thread is ready and running
    void *arg;              // thread Python args, a GC root pointer
    void *stack;            // pointer to the stack
    size_t stack_len;       // number of words in the stack
    struct _mp_thread_t *next;
} mp_thread_t;

// the mutex controls access to the linked list
static mp_thread_mutex_t thread_mutex;
static mp_thread_t thread_entry0;
static mp_thread_t *thread = NULL; // root pointer, handled by mp_thread_gc_others

void mp_thread_init(void *stack, uint32_t stack_len) {
    mp_thread_set_state(&mp_state_ctx.thread);
    // create the first entry in the linked list of all threads
    thread_entry0.id = xTaskGetCurrentTaskHandle();
    thread_entry0.ready = 1;
    thread_entry0.arg = NULL;
    thread_entry0.stack = stack;
    thread_entry0.stack_len = stack_len;
    thread_entry0.next = NULL;
    mp_thread_mutex_init(&thread_mutex);

    // memory barrier to ensure above data is committed
    __sync_synchronize();

    // vTaskPreDeletionHook needs the thread ready after thread_mutex is ready
    thread = &thread_entry0;
}

void mp_thread_gc_others(void) {
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (mp_thread_t *th = thread; th != NULL; th = th->next) {
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
    return pvTaskGetThreadLocalStoragePointer(NULL, 1);
}

void mp_thread_set_state(mp_state_thread_t *state) {
    vTaskSetThreadLocalStoragePointer(NULL, 1, state);
}

mp_uint_t mp_thread_get_id(void) {
    return (mp_uint_t)xTaskGetCurrentTaskHandle();
}

void mp_thread_start(void) {
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (mp_thread_t *th = thread; th != NULL; th = th->next) {
        if (th->id == xTaskGetCurrentTaskHandle()) {
            th->ready = 1;
            break;
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
}

static void *(*ext_thread_entry)(void *) = NULL;

static void freertos_entry(void *arg) {
    if (ext_thread_entry) {
        ext_thread_entry(arg);
    }
    vTaskDelete(NULL);
    for (;;) {;
    }
}

mp_uint_t mp_thread_create_ex(void *(*entry)(void *), void *arg, size_t *stack_size, int priority, char *name) {
    // store thread entry function into a global variable so we can access it
    ext_thread_entry = entry;

    if (*stack_size == 0) {
        *stack_size = MP_THREAD_DEFAULT_STACK_SIZE; // default stack size
    } else if (*stack_size < MP_THREAD_MIN_STACK_SIZE) {
        *stack_size = MP_THREAD_MIN_STACK_SIZE; // minimum stack size
    }

    // Allocate linked-list node (must be outside thread_mutex lock)
    mp_thread_t *th = m_new_obj(mp_thread_t);

    mp_thread_mutex_lock(&thread_mutex, 1);

    // create thread
    BaseType_t result = xTaskCreatePinnedToCore(freertos_entry, name, *stack_size / sizeof(StackType_t), arg, priority, &th->id, MP_TASK_COREID);
    if (result != pdPASS) {
        mp_thread_mutex_unlock(&thread_mutex);
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("can't create thread"));
    }

    // add thread to linked list of all threads
    th->ready = 0;
    th->arg = arg;
    th->stack = pxTaskGetStackStart(th->id);
    th->stack_len = *stack_size / sizeof(uintptr_t);
    th->next = thread;
    thread = th;

    mp_thread_mutex_unlock(&thread_mutex);

    return (mp_uint_t)th->id;
}

mp_uint_t mp_thread_create(void *(*entry)(void *), void *arg, size_t *stack_size) {
    return mp_thread_create_ex(entry, arg, stack_size, MP_THREAD_PRIORITY, "mp_thread");
}

void mp_thread_finish(void) {
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (mp_thread_t *th = thread; th != NULL; th = th->next) {
        if (th->id == xTaskGetCurrentTaskHandle()) {
            th->ready = 0;
            break;
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
}

// This is called either from vTaskDelete() or from the FreeRTOS idle task, so
// may not be within Python context. Therefore MP_STATE_THREAD may not be valid
// and it does not have the GIL.
void vTaskPreDeletionHook(void *tcb) {
    if (thread == NULL) {
        // threading not yet initialised
        return;
    }
    mp_thread_t *prev = NULL;
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (mp_thread_t *th = thread; th != NULL; prev = th, th = th->next) {
        // unlink the node from the list
        if ((void *)th->id == tcb) {
            if (prev != NULL) {
                prev->next = th->next;
            } else {
                // move the start pointer
                thread = th->next;
            }
            // The "th" memory will eventually be reclaimed by the GC.
            break;
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
}

void mp_thread_mutex_init(mp_thread_mutex_t *mutex) {
    // Need a binary semaphore so a lock can be acquired on one Python thread
    // and then released on another.
    mutex->handle = xSemaphoreCreateBinaryStatic(&mutex->buffer);
    xSemaphoreGive(mutex->handle);
}

int mp_thread_mutex_lock(mp_thread_mutex_t *mutex, int wait) {
    return pdTRUE == xSemaphoreTake(mutex->handle, wait ? portMAX_DELAY : 0);
}

void mp_thread_mutex_unlock(mp_thread_mutex_t *mutex) {
    xSemaphoreGive(mutex->handle);
    // Python threads run at equal priority, so pre-emptively yield here to
    // prevent pathological imbalances where a thread unlocks and then
    // immediately re-locks a mutex before a context switch can occur, leaving
    // another thread waiting for an unbounded period of time.
    taskYIELD();
}

void mp_thread_deinit(void) {
    for (;;) {
        // Find a task to delete
        TaskHandle_t id = NULL;
        mp_thread_mutex_lock(&thread_mutex, 1);
        for (mp_thread_t *th = thread; th != NULL; th = th->next) {
            // Don't delete the current task
            if (th->id != xTaskGetCurrentTaskHandle()) {
                id = th->id;
                break;
            }
        }
        mp_thread_mutex_unlock(&thread_mutex);

        if (id == NULL) {
            // No tasks left to delete
            break;
        } else {
            // Call FreeRTOS to delete the task (it will call vTaskPreDeletionHook)
            vTaskDelete(id);
        }
    }
}

#else

void vTaskPreDeletionHook(void *tcb) {
}

#endif // MICROPY_PY_THREAD
